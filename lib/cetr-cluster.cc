// Copyright 2010 Susumu Yata <syata@acm.org>

#include <nwc-toolkit/cetr-cluster.h>

namespace nwc_toolkit {

void CetrCluster::Cluster(const std::vector<CetrPoint> &points,
    std::vector<int> *clusters, int num_clusters,
    int num_tries, int num_steps) {
  if (num_clusters < MIN_NUM_CLUSTERS) {
    num_clusters = DEFAULT_NUM_CLUSTERS;
  }
  if (num_tries < MIN_NUM_TRIES) {
    num_tries = DEFAULT_NUM_TRIES;
  }
  if (num_steps < MIN_NUM_STEPS) {
    num_steps = DEFAULT_NUM_STEPS;
  }

  CetrCluster clustering_module(num_clusters, num_tries, num_steps);
  clustering_module.Do(points, clusters);
}

CetrCluster::CetrCluster(int num_clusters, int num_tries, int num_steps)
    : num_clusters_(num_clusters),
      num_tries_(num_tries),
      num_steps_(num_steps),
      mt_(),
      clusters_(),
      centroids_(),
      numbers_() {}

void CetrCluster::Clear() {
  clusters_.clear();
  centroids_.clear();
  numbers_.clear();
}

void CetrCluster::Do(const std::vector<CetrPoint> &points,
    std::vector<int> *clusters) {
  double min_total_sq_dist = 1.0 * (1 << 30);
  for (int i = 0; i < num_tries_; ++i) {
    double total_sq_dist;
    UpdateClusters(points, &total_sq_dist);
    for (int i = 0; i < num_steps_; ++i) {
      UpdateCentroids(points);
      double last_total_sq_dist = total_sq_dist;
      UpdateClusters(points, &total_sq_dist);
      if (total_sq_dist == last_total_sq_dist) {
        break;
      }
    }
    if (total_sq_dist < min_total_sq_dist) {
      *clusters = clusters_;
      min_total_sq_dist = total_sq_dist;
    }
    Clear();
  }
}

void CetrCluster::UpdateClusters(const std::vector<CetrPoint> &points,
    double *total_sq_dist) {
  clusters_.resize(points.size());
  if (centroids_.empty()) {
    centroids_.resize(num_clusters_);
    numbers_.resize(num_clusters_);
    for (std::size_t i = 0; i < points.size(); ++i) {
      int cluster = static_cast<int>(mt_() % num_clusters_);
      clusters_[i] = cluster;
      ++numbers_[cluster];
    }
    *total_sq_dist = 1.0 * (1 << 29);
  } else {
    for (std::size_t i = 0; i < numbers_.size(); ++i) {
      numbers_[i] = 0;
    }
    *total_sq_dist = 0.0;
    for (std::size_t i = 0; i < points.size(); ++i) {
      int closest_cluster = 0;
      double min_sq_dist = 1.0 * (1 << 29);
      for (std::size_t j = 0; j < centroids_.size(); ++j) {
        double x_diff = points[i].x() - centroids_[j].x();
        double y_diff = points[i].y() - centroids_[j].y();
        double sq_dist = (x_diff * x_diff) + (y_diff * y_diff);
        if (sq_dist < min_sq_dist) {
          closest_cluster = j;
          min_sq_dist = sq_dist;
        }
      }
      *total_sq_dist += min_sq_dist;
      clusters_[i] = closest_cluster;
      ++numbers_[clusters_[i]];
    }
  }
}

void CetrCluster::UpdateCentroids(const std::vector<CetrPoint> &points) {
  centroids_.resize(num_clusters_);
  for (std::size_t i = 0; i < centroids_.size(); ++i) {
    centroids_[i].set_x(0.0);
    centroids_[i].set_y(0.0);
  }
  for (std::size_t i = 0; i < points.size(); ++i) {
    centroids_[clusters_[i]].set_x(
        centroids_[clusters_[i]].x() + points[i].x());
    centroids_[clusters_[i]].set_y(
        centroids_[clusters_[i]].y() + points[i].y());
  }
  for (std::size_t i = 0; i < centroids_.size(); ++i) {
    if (numbers_[i] != 0.0) {
      centroids_[i].set_x(centroids_[i].x() / numbers_[i]);
      centroids_[i].set_y(centroids_[i].y() / numbers_[i]);
    }
  }

  std::size_t origin_id = 0;
  double min_sq_dist = 1.0 * (1 << 29);
  for (std::size_t i = 0; i < centroids_.size(); ++i) {
    double x_diff = centroids_[i].x();
    double y_diff = centroids_[i].y();
    double sq_dist = (x_diff * x_diff) + (y_diff * y_diff);
    min_sq_dist = centroids_[i].x() * centroids_[i].x();
    if (sq_dist < min_sq_dist) {
      origin_id = i;
      min_sq_dist = sq_dist;
    }
  }
  centroids_[origin_id].set_x(0.0);
  centroids_[origin_id].set_y(0.0);
}

}  // namespace nwc_toolkit

// Copyright 2010 Susumu Yata <syata@acm.org>

#include <cassert>
#include <tr1/random>
#include <vector>

#include <nwc-toolkit/cetr-cluster.h>

namespace {

std::tr1::mt19937 mt_rand(static_cast<unsigned int>(time(NULL)));

void RandomizePoints(double max_x, double max_y,
    std::vector<nwc_toolkit::CetrPoint> *points) {
  for (std::size_t i = 0; i < points->size(); ++i) {
    (*points)[i].set_x(max_x * mt_rand() / mt_rand.max());
    (*points)[i].set_y(max_y * mt_rand() / mt_rand.max());
  }
}

void GetCentroids(const std::vector<nwc_toolkit::CetrPoint> &points,
    const std::vector<int> &clusters,
    std::vector<nwc_toolkit::CetrPoint> *centroids) {
  for (std::size_t i = 0; i < centroids->size(); ++i) {
    (*centroids)[i].set_x(0.0);
    (*centroids)[i].set_y(0.0);
  }
  std::vector<std::size_t> numbers(centroids->size(), 0);
  for (std::size_t i = 0; i < points.size(); ++i) {
    int cluster = clusters[i];
    (*centroids)[cluster].set_x((*centroids)[cluster].x() + points[i].x());
    (*centroids)[cluster].set_y((*centroids)[cluster].y() + points[i].y());
    ++numbers[cluster];
  }
  for (std::size_t i = 0; i < centroids->size(); ++i) {
    (*centroids)[i].set_x((*centroids)[i].x() / numbers[i]);
    (*centroids)[i].set_y((*centroids)[i].y() / numbers[i]);
  }
}

double TotalizeDistances(const std::vector<nwc_toolkit::CetrPoint> &points,
    const std::vector<nwc_toolkit::CetrPoint> &centroids) {
  double total_dist = 0.0;
  for (std::size_t i = 0; i < points.size(); ++i) {
    double min_dist = 1.0 * (1 << 30);
    for (std::size_t j = 0; j < centroids.size(); ++j) {
      double diff_x = points[i].x() - centroids[j].x();
      double diff_y = points[i].y() - centroids[j].y();
      double dist = std::sqrt((diff_x * diff_x) + (diff_y * diff_y));
      if (dist < min_dist) {
        min_dist = dist;
      }
    }
    total_dist += min_dist;
  }
  return total_dist;
}

}  // namespace

int main() {
  enum { NUM_TESTS = 100, NUM_POINTS = 500, MAX_X = 20, MAX_Y = 100 };
  enum { NUM_CLUSTERS = nwc_toolkit::CetrCluster::DEFAULT_NUM_CLUSTERS };

  std::vector<nwc_toolkit::CetrPoint> default_centroids(NUM_CLUSTERS);
  default_centroids[0].set_x(0.0);
  default_centroids[0].set_y(0.0);
  for (int i = 1; i < NUM_CLUSTERS; ++i) {
    nwc_toolkit::CetrPoint point;
    if (MAX_X < MAX_Y) {
      point.set_x(MAX_X / 2.0);
      point.set_y((1.0 + (i * 2.0)) * MAX_Y / NUM_CLUSTERS / 2.0);
    } else {
      point.set_x((1.0 + (i * 2.0)) * MAX_X / NUM_CLUSTERS / 2.0);
      point.set_y(MAX_Y / 2.0);
    }
    default_centroids[i] = point;
  }

  std::vector<nwc_toolkit::CetrPoint> points(NUM_POINTS);
  std::vector<int> clusters;
  std::vector<nwc_toolkit::CetrPoint> centroids(NUM_CLUSTERS);

  for (int i = 0; i < NUM_TESTS; ++i) {
    RandomizePoints(MAX_X, MAX_Y, &points);
    nwc_toolkit::CetrCluster::Cluster(points, &clusters);
    GetCentroids(points, clusters, &centroids);

    double default_total_dist = TotalizeDistances(points, default_centroids);
    double total_dist = TotalizeDistances(points, centroids);
    assert(total_dist < default_total_dist);
  }

  return 0;
}

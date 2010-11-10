// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_CETR_CLUSTER_H_
#define NWC_TOOLKIT_CETR_CLUSTER_H_

#include <tr1/random>
#include <vector>

#include "./cetr-point.h"

namespace nwc_toolkit {

class CetrCluster {
 public:
  enum {
    MIN_NUM_CLUSTERS = 2,
    DEFAULT_NUM_CLUSTERS = 3
  };

  enum {
    MIN_NUM_TRIES = 1,
    DEFAULT_NUM_TRIES = 3
  };

  enum {
    MIN_NUM_STEPS = 1,
    DEFAULT_NUM_STEPS = 100
  };

  static void Cluster(const std::vector<CetrPoint> &points,
      std::vector<int> *clusters, int num_clusters = 0,
      int num_tries = 0, int num_steps = 0);

 private:
  const int num_clusters_;
  const int num_tries_;
  const int num_steps_;
  std::tr1::mt19937 mt_;
  std::vector<int> clusters_;
  std::vector<CetrPoint> centroids_;
  std::vector<std::size_t> numbers_;

  CetrCluster(int num_clusters, int num_tries, int num_steps);
  ~CetrCluster() {}

  void Clear();

  void Do(const std::vector<CetrPoint> &points, std::vector<int> *clusters);

  void UpdateClusters(const std::vector<CetrPoint> &points,
      double *total_sq_dist);
  void UpdateCentroids(const std::vector<CetrPoint> &points);

  // Disallows copy and assignment.
  CetrCluster(const CetrCluster &);
  CetrCluster &operator=(const CetrCluster &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_CETR_CLUSTER_H_

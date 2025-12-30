#ifndef SPATIAL_INDEX_HPP
#define SPATIAL_INDEX_HPP

#include <memory>
#include <vector>
#include <cstdint>
#include <limits>

namespace spatio {

// KD-tree node with subtree bounding boxes
struct KDNode {
    float point[2];      // [lat, lon]
    uint64_t id;
    int axis;            // 0=lat, 1=lon
    
    // Subtree spatial bounds (optimization 1)
    float min_lat, max_lat;
    float min_lon, max_lon;
    
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;
    
    KDNode(float lat, float lon, uint64_t id_, int axis_)
        : id(id_), axis(axis_),
          min_lat(lat), max_lat(lat),
          min_lon(lon), max_lon(lon) {
        point[0] = lat;
        point[1] = lon;
    }
    
    // Update bounding box to include child bounds
    void update_bounds() {
        if (left) {
            min_lat = std::min(min_lat, left->min_lat);
            max_lat = std::max(max_lat, left->max_lat);
            min_lon = std::min(min_lon, left->min_lon);
            max_lon = std::max(max_lon, left->max_lon);
        }
        if (right) {
            min_lat = std::min(min_lat, right->min_lat);
            max_lat = std::max(max_lat, right->max_lat);
            min_lon = std::min(min_lon, right->min_lon);
            max_lon = std::max(max_lon, right->max_lon);
        }
    }
};

// Query statistics for instrumentation
struct SpatialQueryStats {
    size_t nodes_visited = 0;
    size_t distance_checks = 0;
    size_t bbox_prunes = 0;      // Pruned by bounding box
    size_t distance_prunes = 0;   // Pruned by distance check
    
    void reset() {
        nodes_visited = 0;
        distance_checks = 0;
        bbox_prunes = 0;
        distance_prunes = 0;
    }
};

class SpatialIndex {
public:
    SpatialIndex() = default;
    
    void insert(float lat, float lon, uint64_t id);
    
    // Radius queries
    std::vector<uint64_t> radius_query(float center_lat, float center_lon, 
                                       double radius_km) const;
    std::vector<uint64_t> radius_query_instrumented(float center_lat, float center_lon,
                                                    double radius_km,
                                                    SpatialQueryStats& stats) const;
    
    // Bounding box queries
    std::vector<uint64_t> box_query(float lat_min, float lon_min,
                                   float lat_max, float lon_max) const;
    
    // K-nearest neighbors
    std::vector<uint64_t> knn_query(float lat, float lon, size_t k) const;
    
    size_t size() const { return size_; }
    void clear();

private:
    std::unique_ptr<KDNode> root_;
    size_t size_ = 0;
    
    // Insertion helpers
    void insert_recursive(std::unique_ptr<KDNode>& node, float lat, float lon,
                         uint64_t id, int depth);
    void update_bounds_upward(KDNode* node);
    
    // Radius query helpers
    void radius_query_recursive(const KDNode* node, float center_lat, float center_lon,
                               double radius_m, std::vector<uint64_t>& results) const;
    void radius_query_instrumented_recursive(const KDNode* node, float center_lat, 
                                            float center_lon, double radius_m,
                                            std::vector<uint64_t>& results,
                                            SpatialQueryStats& stats) const;
    
    // Box query helpers
    void box_query_recursive(const KDNode* node, float lat_min, float lon_min,
                            float lat_max, float lon_max,
                            std::vector<uint64_t>& results) const;
    
    // KNN helpers
    struct KNNCandidate {
        uint64_t id;
        double distance;
        bool operator>(const KNNCandidate& other) const {
            return distance > other.distance;
        }
    };
    
    void knn_recursive(const KDNode* node, float query_lat, float query_lon,
                      size_t k, std::vector<KNNCandidate>& candidates) const;
    
    // Utility functions
    bool in_box(float lat, float lon, float lat_min, float lon_min,
               float lat_max, float lon_max) const;
    
    // Optimization 2: cheap distance precheck using bounding box
    bool bbox_possibly_intersects(const KDNode* node, float center_lat, 
                                 float center_lon, double radius_m) const;
    
    double min_distance_to_bbox(float query_lat, float query_lon,
                               float min_lat, float max_lat,
                               float min_lon, float max_lon) const;
};

} // namespace spatio

#endif // SPATIAL_INDEX_HPP
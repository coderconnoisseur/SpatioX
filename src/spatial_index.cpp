#include "spatial_index.hpp"
#include "utils.hpp"
#include <algorithm>
#include <queue>

namespace spatio {

void SpatialIndex::insert(float lat, float lon, uint64_t id) {
    insert_recursive(root_, lat, lon, id, 0);
    size_++;
}

void SpatialIndex::insert_recursive(std::unique_ptr<KDNode>& node, float lat, float lon, 
                                   uint64_t id, int depth) {
    if (!node) {
        int axis = depth % 2;  // 0 for lat, 1 for lon
        node = std::make_unique<KDNode>(lat, lon, id, axis);
        return;
    }
    
    int axis = node->axis;
    float value = (axis == 0) ? lat : lon;
    float node_value = node->point[axis];
    
    if (value < node_value) {
        insert_recursive(node->left, lat, lon, id, depth + 1);
    } else {
        insert_recursive(node->right, lat, lon, id, depth + 1);
    }
    
    // Update bounding box after insertion
    node->update_bounds();
}

std::vector<uint64_t> SpatialIndex::radius_query(float center_lat, float center_lon, 
                                                  double radius_km) const {
    std::vector<uint64_t> results;
    double radius_m = radius_km * 1000.0;
    radius_query_recursive(root_.get(), center_lat, center_lon, radius_m, results);
    return results;
}

std::vector<uint64_t> SpatialIndex::radius_query_instrumented(float center_lat, float center_lon,
                                                              double radius_km,
                                                              SpatialQueryStats& stats) const {
    std::vector<uint64_t> results;
    stats.reset();
    double radius_m = radius_km * 1000.0;
    radius_query_instrumented_recursive(root_.get(), center_lat, center_lon, radius_m, results, stats);
    return results;
}

void SpatialIndex::radius_query_recursive(const KDNode* node, float center_lat, 
                                          float center_lon, double radius_m,
                                          std::vector<uint64_t>& results) const {
    if (!node) return;
    
    // Check if current point is within radius
    float dist = haversine_distance(center_lat, center_lon, node->point[0], node->point[1]);
    if (dist <= radius_m) {
        results.push_back(node->id);
    }
    
    // Determine which subtrees to explore using correct Haversine distance
    int axis = node->axis;
    float center_value = (axis == 0) ? center_lat : center_lon;
    float node_value = node->point[axis];
    
    // Calculate actual distance from query center to the splitting plane
    double plane_dist_m;
    if (axis == 0) {  // Latitude axis
        plane_dist_m = haversine_distance(center_lat, center_lon, node_value, center_lon);
    } else {  // Longitude axis
        plane_dist_m = haversine_distance(center_lat, center_lon, center_lat, node_value);
    }
    
    // If plane is outside radius, only explore the side containing the query point
    // If plane intersects radius, must explore both sides
    bool explore_left = true;
    bool explore_right = true;
    
    if (plane_dist_m > radius_m) {
        if (center_value < node_value) {
            explore_right = false;
        } else {
            explore_left = false;
        }
    }
    
    if (explore_left) {
        radius_query_recursive(node->left.get(), center_lat, center_lon, radius_m, results);
    }
    if (explore_right) {
        radius_query_recursive(node->right.get(), center_lat, center_lon, radius_m, results);
    }
}

void SpatialIndex::radius_query_instrumented_recursive(const KDNode* node, float center_lat,
                                                       float center_lon, double radius_m,
                                                       std::vector<uint64_t>& results,
                                                       SpatialQueryStats& stats) const {
    if (!node) return;
    
    stats.nodes_visited++;
    
    // Check if current point is within radius
    stats.distance_checks++;  
    float dist = haversine_distance(center_lat, center_lon, node->point[0], node->point[1]);
    if (dist <= radius_m) {
        results.push_back(node->id);
    }
    
    // Determine which subtrees to explore
    int axis = node->axis;
    float center_value = (axis == 0) ? center_lat : center_lon;
    float node_value = node->point[axis];
    
    // Calculate distance to splitting plane  
    stats.distance_checks++;
    double plane_dist_m;
    if (axis == 0) {
        plane_dist_m = haversine_distance(center_lat, center_lon, node_value, center_lon);
    } else {
        plane_dist_m = haversine_distance(center_lat, center_lon, center_lat, node_value);
    }
    
    bool explore_left = true;
    bool explore_right = true;
    
    if (plane_dist_m > radius_m) {
        stats.distance_prunes++;
        if (center_value < node_value) {
            explore_right = false;
        } else {
            explore_left = false;
        }
    }
    
    if (explore_left) {
        radius_query_instrumented_recursive(node->left.get(), center_lat, center_lon, radius_m, results, stats);
    }
    if (explore_right) {
        radius_query_instrumented_recursive(node->right.get(), center_lat, center_lon, radius_m, results, stats);
    }
}

std::vector<uint64_t> SpatialIndex::box_query(float lat_min, float lon_min,
                                              float lat_max, float lon_max) const {
    std::vector<uint64_t> results;
    box_query_recursive(root_.get(), lat_min, lon_min, lat_max, lon_max, results);
    return results;
}

void SpatialIndex::box_query_recursive(const KDNode* node, float lat_min, float lon_min,
                                      float lat_max, float lon_max,
                                      std::vector<uint64_t>& results) const {
    if (!node) return;
    
    // Check if current point is in the box
    if (in_box(node->point[0], node->point[1], lat_min, lon_min, lat_max, lon_max)) {
        results.push_back(node->id);
    }
    
    // Determine which subtrees to explore
    int axis = node->axis;
    float node_value = node->point[axis];
    
    if (axis == 0) {  // Latitude axis
        if (lat_min <= node_value) {
            box_query_recursive(node->left.get(), lat_min, lon_min, lat_max, lon_max, results);
        }
        if (lat_max >= node_value) {
            box_query_recursive(node->right.get(), lat_min, lon_min, lat_max, lon_max, results);
        }
    } else {  // Longitude axis
        if (lon_min <= node_value) {
            box_query_recursive(node->left.get(), lat_min, lon_min, lat_max, lon_max, results);
        }
        if (lon_max >= node_value) {
            box_query_recursive(node->right.get(), lat_min, lon_min, lat_max, lon_max, results);
        }
    }
}

std::vector<uint64_t> SpatialIndex::knn_query(float lat, float lon, size_t k) const {
    if (k == 0 || !root_) return {};
    
    std::vector<KNNCandidate> candidates;
    knn_recursive(root_.get(), lat, lon, k, candidates);
    
    // Extract IDs from candidates
    std::vector<uint64_t> results;
    results.reserve(candidates.size());
    for (const auto& cand : candidates) {
        results.push_back(cand.id);
    }
    
    return results;
}

void SpatialIndex::knn_recursive(const KDNode* node, float query_lat, float query_lon,
                                 size_t k, std::vector<KNNCandidate>& candidates) const {
    if (!node) return;
    
    //  Calculate distance to current node
    double dist = haversine_distance(query_lat, query_lon, node->point[0], node->point[1]);
    
    // Add to candidates (using max heap to keep k smallest)
    if (candidates.size() < k) {
        candidates.push_back({node->id, dist});
        std::push_heap(candidates.begin(), candidates.end());
    } else if (dist < candidates.front().distance) {
        std::pop_heap(candidates.begin(), candidates.end());
        candidates.back() = {node->id, dist};
        std::push_heap(candidates.begin(), candidates.end());
    }
    
    // Determine which subtree to explore first
    int axis = node->axis;
    float query_value = (axis == 0) ? query_lat : query_lon;
    float node_value = node->point[axis];
    
    const KDNode* first = (query_value < node_value) ? node->left.get() : node->right.get();
    const KDNode* second = (query_value < node_value) ? node->right.get() : node->left.get();
    
    // Search the closer subtree
    knn_recursive(first, query_lat, query_lon, k, candidates);
    
    // Check if we need to search the other subtree
    double plane_dist;
    if (axis == 0) {
        plane_dist = haversine_distance(query_lat, query_lon, node_value, query_lon);
    } else {
        plane_dist = haversine_distance(query_lat, query_lon, query_lat, node_value);
    }
    
    if (candidates.size() < k || plane_dist < candidates[0].distance) {
        knn_recursive(second, query_lat, query_lon, k, candidates);
    }
}

bool SpatialIndex::in_box(float lat, float lon, float lat_min, float lon_min,
                         float lat_max, float lon_max) const {
    return lat >= lat_min && lat <= lat_max && lon >= lon_min && lon <= lon_max;
}

void SpatialIndex::clear() {
    root_.reset();
    size_ = 0;
}

} // namespace spatio

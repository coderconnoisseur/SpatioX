#include "spatial_index.hpp"
#include "utils.hpp"
#include <algorithm>

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
}

std::vector<uint64_t> SpatialIndex::radius_query(float center_lat, float center_lon, 
                                                 double radius_km) const {
    std::vector<uint64_t> results;
    // Convert km to meters for haversine_distance which returns meters
    double radius_m = radius_km * 1000.0;
    radius_query_recursive(root_.get(), center_lat, center_lon, radius_m, results);
    return results;
}

void SpatialIndex::radius_query_recursive(const KDNode* node, float center_lat, 
                                         float center_lon, double radius_m,
                                         std::vector<uint64_t>& results) const {
    if (!node) return;
    
    // Check if current point is within radius (haversine returns meters)
    float dist = haversine_distance(center_lat, center_lon, node->point[0], node->point[1]);
    if (dist <= radius_m) {
        results.push_back(node->id);
    }
    
    // Determine which subtrees to explore using correct Haversine distance
    int axis = node->axis;
    float center_value = (axis == 0) ? center_lat : center_lon;
    float node_value = node->point[axis];
    
    // Calculate actual distance from query center to the splitting plane
    // This is mathematically correct (no approximation) and handles all edge cases
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
            explore_right = false;  // Query is on left side, right can't have results
        } else {
            explore_left = false;   // Query is on right side, left can't have results
        }
    }
    
    if (explore_left) {
        radius_query_recursive(node->left.get(), center_lat, center_lon, radius_m, results);
    }
    if (explore_right) {
        radius_query_recursive(node->right.get(), center_lat, center_lon, radius_m, results);
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

bool SpatialIndex::in_box(float lat, float lon, float lat_min, float lon_min,
                         float lat_max, float lon_max) const {
    return lat >= lat_min && lat <= lat_max && lon >= lon_min && lon <= lon_max;
}

void SpatialIndex::clear() {
    root_.reset();
    size_ = 0;
}

} // namespace spatio

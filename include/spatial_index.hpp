#ifndef SPATIAL_INDEX_HPP
#define SPATIAL_INDEX_HPP

#include <vector>
#include <cstdint>
#include <memory>

namespace spatio {

/**
 * @brief KD-tree node for 2D spatial indexing
 */
struct KDNode {
    float point[2];     // [lat, lon]
    uint64_t id;        // Record ID
    int axis;           // Split axis: 0 for lat, 1 for lon
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;

    KDNode(float lat, float lon, uint64_t _id, int _axis)
        : id(_id), axis(_axis), left(nullptr), right(nullptr) {
        point[0] = lat;
        point[1] = lon;
    }
};

/**
 * @brief Spatial index using KD-tree for efficient 2D queries
 * 
 * Supports:
 * - Radius queries (find all points within distance R)
 * - Bounding box queries (find all points in rectangular region)
 */
class SpatialIndex {
public:
    SpatialIndex() : root_(nullptr) {}

    /**
     * @brief Insert a point into the KD-tree
     * 
     * @param lat Latitude
     * @param lon Longitude
     * @param id Record ID
     */
    void insert(float lat, float lon, uint64_t id);

    /**
     * @brief Find all IDs within radius R from a center point
     * 
     * @param center_lat Center latitude
     * @param center_lon Center longitude
     * @param radius_km Radius in kilometers
     * @return Vector of record IDs
     */
    std::vector<uint64_t> radius_query(float center_lat, float center_lon, double radius_km) const;

    /**
     * @brief Find all IDs within a bounding box
     * 
     * @param lat_min Minimum latitude
     * @param lon_min Minimum longitude
     * @param lat_max Maximum latitude
     * @param lon_max Maximum longitude
     * @return Vector of record IDs
     */
    std::vector<uint64_t> box_query(float lat_min, float lon_min, 
                                     float lat_max, float lon_max) const;

    /**
     * @brief Clear the spatial index
     */
    void clear();

    /**
     * @brief Get number of points in the index
     */
    size_t size() const { return size_; }

private:
    std::unique_ptr<KDNode> root_;
    size_t size_ = 0;

    // Helper functions
    void insert_recursive(std::unique_ptr<KDNode>& node, float lat, float lon, 
                         uint64_t id, int depth);
    
    void radius_query_recursive(const KDNode* node, float center_lat, float center_lon,
                               double radius_m, std::vector<uint64_t>& results) const;
    
    void box_query_recursive(const KDNode* node, float lat_min, float lon_min,
                            float lat_max, float lon_max, std::vector<uint64_t>& results) const;
    
    bool in_box(float lat, float lon, float lat_min, float lon_min, 
                float lat_max, float lon_max) const;
};

} // namespace spatio

#endif // SPATIAL_INDEX_HPP

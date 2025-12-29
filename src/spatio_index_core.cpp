#include "spatio_index_core.hpp"
#include <algorithm>
#include <unordered_set>

namespace spatio {

uint64_t SpatioIndexCore::insert(float lat, float lon, double t) {
    // Add to record store and get ID
    uint64_t id = record_store_.add_record(lat, lon, t);
    
    // Add to both indexes
    spatial_index_.insert(lat, lon, id);
    temporal_index_.insert(t, id);
    
    return id;
}

std::vector<uint64_t> SpatioIndexCore::query_radius_time(float center_lat, float center_lon,
                                                         double radius_km,
                                                         double t_start, double t_end) const {
    // Spatial-first strategy: query spatial index first
    std::vector<uint64_t> spatial_ids = spatial_index_.radius_query(center_lat, center_lon, radius_km);
    
    // Filter by time
    return filter_by_time(spatial_ids, t_start, t_end);
}

std::vector<uint64_t> SpatioIndexCore::query_box_time(float lat_min, float lon_min,
                                                      float lat_max, float lon_max,
                                                      double t_start, double t_end) const {
    // Spatial-first strategy: query spatial index first
    std::vector<uint64_t> spatial_ids = spatial_index_.box_query(lat_min, lon_min, lat_max, lon_max);
    
    // Filter by time
    return filter_by_time(spatial_ids, t_start, t_end);
}

std::vector<uint64_t> SpatioIndexCore::filter_by_time(const std::vector<uint64_t>& spatial_ids,
                                                      double t_start, double t_end) const {
    std::vector<uint64_t> results;
    
    for (uint64_t id : spatial_ids) {
        auto record = record_store_.get_record(id);
        if (record && record->t >= t_start && record->t <= t_end) {
            results.push_back(id);
        }
    }
    
    return results;
}

void SpatioIndexCore::clear() {
    record_store_.clear();
    spatial_index_.clear();
    temporal_index_.clear();
}

} // namespace spatio

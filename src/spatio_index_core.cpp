#include "spatio_index_core.hpp"
#include <algorithm>

namespace spatio {

// ==================== INSERTION ====================

uint64_t SpatioIndexCore::insert(float lat, float lon, double t) {
    uint64_t id = record_store_.add_record(lat, lon, t);
    spatial_index_.insert(lat, lon, id);
    temporal_index_.insert(t, id);
    build_completed_ = false;
    return id;
}

std::vector<uint64_t> SpatioIndexCore::bulk_insert(const std::vector<RecordInput>& records) {
    std::vector<uint64_t> ids;
    ids.reserve(records.size());
    
    for (const auto& rec : records) {
        uint64_t id = record_store_.add_record(rec.lat, rec.lon, rec.t);
        ids.push_back(id);
        spatial_index_.insert(rec.lat, rec.lon, id);
        temporal_index_.insert(rec.t, id);
    }
    
    build_completed_ = false;
    return ids;
}

void SpatioIndexCore::build() {
    // Phase A: No-op placeholder
    // Future: balanced tree construction, metadata computation
    build_completed_ = true;
}

// ==================== SPATIAL-ONLY QUERIES ====================

std::vector<uint64_t> SpatioIndexCore::query_radius(float center_lat, float center_lon,
                                                    double radius_km) const {
    // No time filter - return all spatial matches
    return spatial_index_.radius_query(center_lat, center_lon, radius_km);
}

std::vector<uint64_t> SpatioIndexCore::query_box(float lat_min, float lon_min,
                                                 float lat_max, float lon_max) const {
    return spatial_index_.box_query(lat_min, lon_min, lat_max, lon_max);
}

std::vector<uint64_t> SpatioIndexCore::query_knn(float lat, float lon, size_t k) const {
    return spatial_index_.knn_query(lat, lon, k);
}

// ==================== SPATIAL + TEMPORAL QUERIES ====================

std::vector<uint64_t> SpatioIndexCore::query_radius_time(float center_lat, float center_lon,
                                                         double radius_km,
                                                         double t_start, double t_end) const {
    // Optimization 3: Early rejection using temporal bounds
    if (t_end < temporal_index_.min_time() || t_start > temporal_index_.max_time()) {
        return {};
    }
    
    // Spatial-first strategy
    std::vector<uint64_t> spatial_ids = spatial_index_.radius_query(center_lat, center_lon, radius_km);
    return filter_by_time(spatial_ids, t_start, t_end);
}

std::vector<uint64_t> SpatioIndexCore::query_box_time(float lat_min, float lon_min,
                                                      float lat_max, float lon_max,
                                                      double t_start, double t_end) const {
    // Early rejection
    if (t_end < temporal_index_.min_time() || t_start > temporal_index_.max_time()) {
        return {};
    }
    
    std::vector<uint64_t> spatial_ids = spatial_index_.box_query(lat_min, lon_min, lat_max, lon_max);
    return filter_by_time(spatial_ids, t_start, t_end);
}

std::vector<uint64_t> SpatioIndexCore::query_knn_time(float lat, float lon, size_t k,
                                                      double t_start, double t_end) const {
    // For KNN with time filter, we need to be careful:
    // We may need to retrieve more than k spatial neighbors to get k valid temporal neighbors
    
    // Early rejection
    if (t_end < temporal_index_.min_time() || t_start > temporal_index_.max_time()) {
        return {};
    }
    
    // Strategy: fetch more spatial neighbors, then filter by time
    // Heuristic: fetch up to 3k neighbors (configurable)
    size_t fetch_k = std::min(k * 3, size());
    if (fetch_k == 0) return {};
    
    std::vector<uint64_t> spatial_ids = spatial_index_.knn_query(lat, lon, fetch_k);
    std::vector<uint64_t> time_filtered = filter_by_time(spatial_ids, t_start, t_end);
    
    // Truncate to k if we got more
    if (time_filtered.size() > k) {
        time_filtered.resize(k);
    }
    
    return time_filtered;
}

// ==================== INSTRUMENTED QUERIES ====================

std::vector<uint64_t> SpatioIndexCore::query_radius_time_instrumented(
    float center_lat, float center_lon, double radius_km,
    double t_start, double t_end, QueryStats& stats) const {
    
    stats.reset();
    
    // Early rejection
    if (t_end < temporal_index_.min_time() || t_start > temporal_index_.max_time()) {
        return {};
    }
    
    // Spatial query with instrumentation
    SpatialQueryStats spatial_stats;
    double radius_m = radius_km * 1000.0;
    std::vector<uint64_t> spatial_ids = spatial_index_.radius_query_instrumented(
        center_lat, center_lon, radius_km, spatial_stats);
    
    // Copy spatial stats
    stats.spatial_nodes_visited = spatial_stats.nodes_visited;
    stats.spatial_distance_checks = spatial_stats.distance_checks;
    stats.spatial_bbox_prunes = spatial_stats.bbox_prunes;
    stats.spatial_distance_prunes = spatial_stats.distance_prunes;
    
    // Time filtering with instrumentation
    std::vector<uint64_t> results = filter_by_time_instrumented(spatial_ids, t_start, t_end, stats);
    stats.result_count = results.size();
    
    return results;
}

// ==================== DATA ACCESS ====================

const Record* SpatioIndexCore::get_record_ptr(uint64_t id) const {
    // TRUE zero-copy: direct pointer into RecordStore's vector
    return record_store_.get_record_ptr(id);
}

// ==================== FILTERING HELPERS ====================

std::vector<uint64_t> SpatioIndexCore::filter_by_time(const std::vector<uint64_t>& spatial_ids,
                                                      double t_start, double t_end) const {
    std::vector<uint64_t> results;
    results.reserve(spatial_ids.size());
    
    for (uint64_t id : spatial_ids) {
        // Use zero-copy pointer access
        const Record* record = record_store_.get_record_ptr(id);
        if (record && record->t >= t_start && record->t <= t_end) {
            results.push_back(id);
        }
    }
    
    return results;
}

std::vector<uint64_t> SpatioIndexCore::filter_by_time_instrumented(
    const std::vector<uint64_t>& spatial_ids,
    double t_start, double t_end,
    QueryStats& stats) const {
    
    std::vector<uint64_t> results;
    results.reserve(spatial_ids.size());
    
    for (uint64_t id : spatial_ids) {
        const Record* record = record_store_.get_record_ptr(id);
        if (record) {
            if (record->t >= t_start && record->t <= t_end) {
                results.push_back(id);
                stats.records_passed_time_filter++;
            } else {
                stats.records_filtered_by_time++;
            }
        }
    }
    
    return results;
}

// ==================== STATISTICS ====================

SpatioIndexCore::IndexStats SpatioIndexCore::get_index_stats() const {
    IndexStats stats;
    stats.total_records = record_store_.size();
    stats.spatial_nodes = spatial_index_.size();
    stats.temporal_entries = temporal_index_.size();
    stats.min_time = temporal_index_.min_time();
    stats.max_time = temporal_index_.max_time();
    stats.is_built = build_completed_;
    return stats;
}

void SpatioIndexCore::clear() {
    record_store_.clear();
    spatial_index_.clear();
    temporal_index_.clear();
    build_completed_ = false;
}

} // namespace spatio
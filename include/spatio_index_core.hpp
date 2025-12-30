#ifndef SPATIO_INDEX_CORE_HPP
#define SPATIO_INDEX_CORE_HPP

#include "spatial_index.hpp"
#include "temporal_index.hpp"
#include "record_store.hpp"
#include <vector>
#include <optional>
#include <limits>

namespace spatio {

struct RecordInput {
    float lat;
    float lon;
    double t;
    
    RecordInput(float lat_, float lon_, double t_) 
        : lat(lat_), lon(lon_), t(t_) {}
};

// Comprehensive query statistics (Optimization 4: Instrumentation)
struct QueryStats {
    // Spatial statistics
    size_t spatial_nodes_visited = 0;
    size_t spatial_distance_checks = 0;
    size_t spatial_bbox_prunes = 0;
    size_t spatial_distance_prunes = 0;
    
    // Temporal statistics
    size_t records_filtered_by_time = 0;
    size_t records_passed_time_filter = 0;
    
    // Overall
    size_t result_count = 0;
    
    void reset() {
        spatial_nodes_visited = 0;
        spatial_distance_checks = 0;
        spatial_bbox_prunes = 0;
        spatial_distance_prunes = 0;
        records_filtered_by_time = 0;
        records_passed_time_filter = 0;
        result_count = 0;
    }
};

class SpatioIndexCore {
public:
    SpatioIndexCore() = default;
    
    // ==================== INSERTION ====================
    
    // Online insert (streaming)
    uint64_t insert(float lat, float lon, double t);
    
    // Bulk insert (batch)
    std::vector<uint64_t> bulk_insert(const std::vector<RecordInput>& records);
    
    // Explicit build phase
    void build();
    
    // ==================== SPATIAL-ONLY QUERIES ====================
    // Product feature: time should be optional
    
    std::vector<uint64_t> query_radius(float center_lat, float center_lon,
                                      double radius_km) const;
    
    std::vector<uint64_t> query_box(float lat_min, float lon_min,
                                   float lat_max, float lon_max) const;
    
    std::vector<uint64_t> query_knn(float lat, float lon, size_t k) const;
    
    // ==================== SPATIAL + TEMPORAL QUERIES ====================
    
    std::vector<uint64_t> query_radius_time(float center_lat, float center_lon,
                                           double radius_km,
                                           double t_start, double t_end) const;
    
    std::vector<uint64_t> query_box_time(float lat_min, float lon_min,
                                        float lat_max, float lon_max,
                                        double t_start, double t_end) const;
    
    std::vector<uint64_t> query_knn_time(float lat, float lon, size_t k,
                                        double t_start, double t_end) const;
    
    // ==================== INSTRUMENTED QUERIES ====================
    // For performance tuning and debugging
    
    std::vector<uint64_t> query_radius_time_instrumented(float center_lat, float center_lon,
                                                         double radius_km,
                                                         double t_start, double t_end,
                                                         QueryStats& stats) const;
    
    // ==================== DATA ACCESS ====================
    
    // Zero-copy record access (Optimization 5A)
    const Record* get_record_ptr(uint64_t id) const;
    
    // Backward compatibility
    std::optional<Record> get_record(uint64_t id) const {
        const Record* ptr = get_record_ptr(id);
        return ptr ? std::optional<Record>(*ptr) : std::nullopt;
    }
    
    size_t size() const { return record_store_.size(); }
    void clear();
    
    // ==================== STATISTICS & DIAGNOSTICS ====================
    
    struct IndexStats {
        size_t total_records = 0;
        size_t spatial_nodes = 0;
        size_t temporal_entries = 0;
        double min_time = 0.0;
        double max_time = 0.0;
        bool is_built = false;
    };
    
    IndexStats get_index_stats() const;

private:
    RecordStore record_store_;
    SpatialIndex spatial_index_;
    TemporalIndex temporal_index_;
    bool build_completed_ = false;
    
    // Shared filtering logic
    std::vector<uint64_t> filter_by_time(const std::vector<uint64_t>& spatial_ids,
                                        double t_start, double t_end) const;
    
    std::vector<uint64_t> filter_by_time_instrumented(const std::vector<uint64_t>& spatial_ids,
                                                      double t_start, double t_end,
                                                      QueryStats& stats) const;
};

} // namespace spatio

#endif // SPATIO_INDEX_CORE_HPP
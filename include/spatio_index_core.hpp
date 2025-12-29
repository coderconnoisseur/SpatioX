#ifndef SPATIO_INDEX_CORE_HPP
#define SPATIO_INDEX_CORE_HPP

#include "record_store.hpp"
#include "spatial_index.hpp"
#include "temporal_index.hpp"
#include <vector>
#include <cstdint>

namespace spatio {

/**
 * @brief Core engine that combines spatial and temporal indexing
 * 
 * SpatioIndexCore provides the main interface for:
 * - Inserting spatial-temporal records
 * - Querying by spatial radius + time range
 * - Querying by bounding box + time range
 * 
 * Query strategy: spatial-first (query spatial index, then filter by time)
 */
class SpatioIndexCore {
public:
    /**
     * @brief Insert a new spatial-temporal record
     * 
     * @param lat Latitude
     * @param lon Longitude
     * @param t Timestamp
     * @return Assigned unique ID
     */
    uint64_t insert(float lat, float lon, double t);

    /**
     * @brief Query by radius and time range
     * 
     * @param center_lat Center latitude
     * @param center_lon Center longitude
     * @param radius_km Radius in kilometers
     * @param t_start Start time (inclusive)
     * @param t_end End time (inclusive)
     * @return Vector of record IDs matching criteria
     */
    std::vector<uint64_t> query_radius_time(float center_lat, float center_lon, 
                                            double radius_km,
                                            double t_start, double t_end) const;

    /**
     * @brief Query by bounding box and time range
     * 
     * @param lat_min Minimum latitude
     * @param lon_min Minimum longitude
     * @param lat_max Maximum latitude
     * @param lon_max Maximum longitude
     * @param t_start Start time (inclusive)
     * @param t_end End time (inclusive)
     * @return Vector of record IDs matching criteria
     */
    std::vector<uint64_t> query_box_time(float lat_min, float lon_min,
                                         float lat_max, float lon_max,
                                         double t_start, double t_end) const;

    /**
     * @brief Get record by ID
     * 
     * @param id Record ID
     * @return Record if found, std::nullopt otherwise
     */
    std::optional<Record> get_record(uint64_t id) const {
        return record_store_.get_record(id);
    }

    /**
     * @brief Get total number of records
     */
    size_t size() const { return record_store_.size(); }

    /**
     * @brief Clear all data
     */
    void clear();

private:
    RecordStore record_store_;
    SpatialIndex spatial_index_;
    TemporalIndex temporal_index_;

    // Helper: filter IDs by time range
    std::vector<uint64_t> filter_by_time(const std::vector<uint64_t>& spatial_ids,
                                         double t_start, double t_end) const;
};

} // namespace spatio

#endif // SPATIO_INDEX_CORE_HPP

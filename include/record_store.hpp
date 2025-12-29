#ifndef RECORD_STORE_HPP
#define RECORD_STORE_HPP

#include "record.hpp"
#include <vector>
#include <unordered_map>
#include <optional>

namespace spatio {

/**
 * @brief Manages all records in the system
 * 
 * RecordStore is responsible for:
 * - Storing all records in a vector
 * - Assigning unique IDs
 * - Providing lookup by ID
 */
class RecordStore {
public:
    /**
     * @brief Add a new record to the store
     * 
     * @param lat Latitude
     * @param lon Longitude
     * @param t Timestamp
     * @return Assigned unique ID
     */
    uint64_t add_record(float lat, float lon, double t);

    /**
     * @brief Get record by ID
     * 
     * @param id Record ID
     * @return Record if found, std::nullopt otherwise
     */
    std::optional<Record> get_record(uint64_t id) const;

    /**
     * @brief Get all records
     * 
     * @return Vector of all records
     */
    const std::vector<Record>& get_all_records() const { return records_; }

    /**
     * @brief Get number of records
     */
    size_t size() const { return records_.size(); }

    /**
     * @brief Clear all records
     */
    void clear();

private:
    std::vector<Record> records_;
    std::unordered_map<uint64_t, size_t> id_to_index_;
    uint64_t next_id_ = 1;  // Start IDs from 1
};

} // namespace spatio

#endif // RECORD_STORE_HPP

#ifndef TEMPORAL_INDEX_HPP
#define TEMPORAL_INDEX_HPP

#include <map>
#include <vector>
#include <cstdint>
#include <limits>

namespace spatio {

/**
 * @brief Temporal index using multimap for time-based queries
 * 
 * Uses std::multimap to maintain records sorted by timestamp,
 * enabling efficient range queries.
 */
class TemporalIndex {
public:
    /**
     * @brief Insert a timestamp-ID pair
     * 
     * @param t Timestamp
     * @param id Record ID
     */
    void insert(double t, uint64_t id);

    /**
     * @brief Find all IDs within a time range
     * 
     * @param t_start Start of time range (inclusive)
     * @param t_end End of time range (inclusive)
     * @return Vector of record IDs
     */
    std::vector<uint64_t> range_query(double t_start, double t_end) const;

    /**
     * @brief Get all record IDs in the index
     * 
     * @return Vector of all record IDs
     */
    std::vector<uint64_t> all_records() const;
    
    /**
     * @brief Get the minimum timestamp in the index
     * 
     * @return The minimum timestamp
     */
    double min_time() const { return min_time_; }

    /**
     * @brief Get the maximum timestamp in the index
     * 
     * @return The maximum timestamp
     */
    double max_time() const { return max_time_; }
    
    /**
     * @brief Clear the temporal index
     */
    void clear();

    /**
     * @brief Get number of entries in the index
     */
    size_t size() const { return time_index_.size(); }

private:
    std::multimap<double, uint64_t> time_index_;
    double min_time_ = std::numeric_limits<double>::max();
    double max_time_ = std::numeric_limits<double>::lowest();
};

} // namespace spatio

#endif // TEMPORAL_INDEX_HPP

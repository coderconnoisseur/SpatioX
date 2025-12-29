#ifndef TEMPORAL_INDEX_HPP
#define TEMPORAL_INDEX_HPP

#include <map>
#include <vector>
#include <cstdint>

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
     * @brief Clear the temporal index
     */
    void clear();

    /**
     * @brief Get number of entries in the index
     */
    size_t size() const { return time_index_.size(); }

private:
    std::multimap<double, uint64_t> time_index_;
};

} // namespace spatio

#endif // TEMPORAL_INDEX_HPP

#include "temporal_index.hpp"

namespace spatio {

void TemporalIndex::insert(double t, uint64_t id) {
    time_index_.insert({t, id});
}

std::vector<uint64_t> TemporalIndex::range_query(double t_start, double t_end) const {
    std::vector<uint64_t> results;
    
    // Find first element >= t_start
    auto it_start = time_index_.lower_bound(t_start);
    // Find first element > t_end
    auto it_end = time_index_.upper_bound(t_end);
    
    // Collect all IDs in range
    for (auto it = it_start; it != it_end; ++it) {
        results.push_back(it->second);
    }
    
    return results;
}

void TemporalIndex::clear() {
    time_index_.clear();
}

} // namespace spatio

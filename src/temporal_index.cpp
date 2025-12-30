#include "temporal_index.hpp"
#include <algorithm>

namespace spatio {

void TemporalIndex::insert(double t, uint64_t id) {
    time_index_.insert({t, id});
    
    // Update time bounds (Optimization 3)
    min_time_ = std::min(min_time_, t);
    max_time_ = std::max(max_time_, t);
}

std::vector<uint64_t> TemporalIndex::range_query(double t_start, double t_end) const {
    std::vector<uint64_t> results;
    
    // Quick rejection if query range doesn't overlap index range
    if (t_end < min_time_ || t_start > max_time_) {
        return results;
    }
    
    // Reserve space for efficiency
    results.reserve(time_index_.size() / 10); // Heuristic
    
    // Find range [t_start, t_end]
    auto it_start = time_index_.lower_bound(t_start);
    auto it_end = time_index_.upper_bound(t_end);
    
    for (auto it = it_start; it != it_end; ++it) {
        results.push_back(it->second);
    }
    
    return results;
}

std::vector<uint64_t> TemporalIndex::all_records() const {
    std::vector<uint64_t> results;
    results.reserve(time_index_.size());
    
    for (const auto& [t, id] : time_index_) {
        results.push_back(id);
    }
    
    return results;
}

void TemporalIndex::clear() {
    time_index_.clear();
    min_time_ = std::numeric_limits<double>::max();
    max_time_ = std::numeric_limits<double>::lowest();
}

} // namespace spatio
#include "record_store.hpp"

namespace spatio {

uint64_t RecordStore::add_record(float lat, float lon, double t) {
    uint64_t id = next_id_++;
    Record rec(lat, lon, t, id);
    
    size_t index = records_.size();
    records_.push_back(rec);
    id_to_index_[id] = index;
    
    return id;
}

std::optional<Record> RecordStore::get_record(uint64_t id) const {
    auto it = id_to_index_.find(id);
    if (it == id_to_index_.end()) {
        return std::nullopt;
    }
    return records_[it->second];
}

void RecordStore::clear() {
    records_.clear();
    id_to_index_.clear();
    next_id_ = 1;
}

} // namespace spatio

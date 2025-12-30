#ifndef RECORD_STORE_HPP
#define RECORD_STORE_HPP

#include "record.hpp"
#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

namespace spatio {

class RecordStore {
public:
    RecordStore() = default;
    
    // Add record and return assigned ID
    uint64_t add_record(float lat, float lon, double t);
    
    // Get record by ID (returns nullopt if not found)
    std::optional<Record> get_record(uint64_t id) const;
    
    // Get direct pointer (for zero-copy access)
    const Record* get_record_ptr(uint64_t id) const;
    
    // Get number of records
    size_t size() const { return records_.size(); }
    
    // Clear all records
    void clear();

private:
    std::vector<Record> records_;
    std::unordered_map<uint64_t, size_t> id_to_index_;
    uint64_t next_id_ = 1;
};

} // namespace spatio

#endif // RECORD_STORE_HPP
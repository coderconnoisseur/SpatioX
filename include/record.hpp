#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstdint>

namespace spatio {

/**
 * @brief Represents a single spatial-temporal record
 * 
 * A Record is a point in space-time with:
 * - Spatial coordinates (latitude, longitude)
 * - Temporal coordinate (timestamp)
 * - Unique identifier
 */
struct Record {
    float lat;        // Latitude in degrees [-90, 90]
    float lon;        // Longitude in degrees [-180, 180]
    double t;         // Timestamp (seconds since epoch or arbitrary time unit)
    uint64_t id;      // Unique identifier

    /**
     * @brief Default constructor
     */
    Record() : lat(0.0f), lon(0.0f), t(0.0), id(0) {}

    /**
     * @brief Parameterized constructor
     */
    Record(float _lat, float _lon, double _t, uint64_t _id = 0)
        : lat(_lat), lon(_lon), t(_t), id(_id) {}
};

} // namespace spatio

#endif // RECORD_HPP

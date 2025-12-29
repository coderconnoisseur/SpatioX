#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace spatio {

// Calculate great-circle distance in meters
inline float haversine_distance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371000.0f; // Earth radius in meters
    float dlat = (lat2 - lat1) * M_PI / 180.0f;
    float dlon = (lon2 - lon1) * M_PI / 180.0f;
    float a = std::sin(dlat/2) * std::sin(dlat/2) +
              std::cos(lat1 * M_PI / 180.0f) * std::cos(lat2 * M_PI / 180.0f) *
              std::sin(dlon/2) * std::sin(dlon/2);
    return R * 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
}

} // namespace spatio

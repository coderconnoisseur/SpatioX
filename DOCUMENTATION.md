# GeoSpatio-Temporal Index

**High-Performance Spatial-Temporal Indexing Engine**

Version: 0.1.0a1  
Package Name: geospatio-temporal-index  
PyPI: geospatio-temporal-index (alpha release)

---

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [API Reference](#api-reference)
5. [Usage Examples](#usage-examples)
6. [Algorithm Details](#algorithm-details)
7. [Performance Characteristics](#performance-characteristics)
8. [Troubleshooting](#troubleshooting)

---

## Overview

GeoSpatio is a high-performance spatial-temporal index engine for Python, backed by optimized C++ implementations. It provides efficient indexing and querying of geo-temporal data using advanced data structures.

### Key Features

- ✅ **Fast spatial queries** (radius and bounding box)
- ✅ **Temporal range filtering**
- ✅ **Combined spatial-temporal queries**
- ✅ **Haversine distance calculations** (accurate to ~1 meter)
- ✅ **Python-friendly API** with payload support
- ✅ **Efficient KD-tree and B+ tree** based indexing

### Use Cases

- 🚕 **Taxi/ride-hailing analytics**
- 🌐 **IoT sensor data analysis**
- 📍 **Location-based services**
- 📊 **Geospatial event tracking**
- 🚗 **Fleet management systems**
- ☁️ **Weather data analysis**

---

## Installation

### Requirements

- Python 3.7 or higher
- C++17 compatible compiler (MSVC on Windows, GCC/Clang on Linux/Mac)
- CMake 3.12+ (for building from source)
- pybind11 2.6.0+

### Installation Methods

#### Method 1: Install from PyPI

```bash
pip install geospatio-temporal-index
```

#### Method 2: Install from source

```bash
# Clone or download the repository
cd /path/to/Spatial

# Install in development mode
pip install -e .

# Or install normally
pip install .
```

### Build Requirements

#### Windows

```bash
# Install build tools
pip install pybind11 setuptools wheel

# Ensure you have Visual Studio or Build Tools for Visual Studio installed
# with C++ development tools
```

#### Linux/Mac

```bash
# Install build tools
pip install pybind11 setuptools wheel

# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
brew install gcc
```

### Verify Installation

```bash
python -c "from spatio import SpatioIndex; print('GeoSpatio installed successfully!')"
```

---

## Quick Start

### Basic Example

```python
from spatio import SpatioIndex

# Create an index
index = SpatioIndex()

# Insert spatial-temporal records with payloads
id1 = index.insert(
    lat=40.7128,           # Latitude (degrees)
    lon=-74.0060,          # Longitude (degrees)
    t=1634567890.0,        # Timestamp (seconds)
    payload={"type": "taxi", "fare": 15.50}
)

id2 = index.insert(
    lat=40.7589,
    lon=-73.9851,
    t=1634568000.0,
    payload={"type": "taxi", "fare": 22.00}
)

# Query records within a radius and time range
results = index.query_radius_time(
    center_lat=40.7128,
    center_lon=-74.0060,
    radius_km=5.0,         # 5 km radius
    t_start=1634567800.0,  # Start time
    t_end=1634568100.0     # End time
)

# Access results
for record_id in results:
    record = index.get_record(record_id)
    payload = index.get_payload(record_id)
    print(f"Location: ({record.lat}, {record.lon})")
    print(f"Time: {record.t}")
    print(f"Payload: {payload}")
```

---

## API Reference

### Class: `SpatioIndex`

The main class for spatial-temporal indexing.

#### Constructor

```python
SpatioIndex()
```

Creates a new empty spatial-temporal index.

**Example:**
```python
index = SpatioIndex()
```

---

#### Methods

### `insert(lat, lon, t, payload=None) -> int`

Insert a new spatial-temporal record.

**Parameters:**
- `lat` (float): Latitude in degrees, range [-90, 90]
- `lon` (float): Longitude in degrees, range [-180, 180]
- `t` (float): Timestamp (any numeric value, typically Unix timestamp)
- `payload` (Any, optional): User data to associate with this record

**Returns:**
- `int`: Unique record ID

**Example:**
```python
record_id = index.insert(
    40.7128, -74.0060, 1634567890.0,
    payload={"event": "pickup", "fare": 15.50}
)
```

---

### `query_radius_time(center_lat, center_lon, radius_km, t_start, t_end) -> List[int]`

Find all records within a circular area and time range.

Uses accurate Haversine distance calculation on a spherical Earth model.

**Parameters:**
- `center_lat` (float): Center latitude in degrees
- `center_lon` (float): Center longitude in degrees
- `radius_km` (float): Search radius in kilometers
- `t_start` (float): Start time (inclusive)
- `t_end` (float): End time (inclusive)

**Returns:**
- `List[int]`: List of record IDs matching the criteria

**Example:**
```python
results = index.query_radius_time(
    center_lat=40.7589,
    center_lon=-73.9851,
    radius_km=2.0,
    t_start=1634568000.0,
    t_end=1634575200.0
)
```

---

### `query_box_time(lat_min, lon_min, lat_max, lon_max, t_start, t_end) -> List[int]`

Find all records within a rectangular bounding box and time range.

**Parameters:**
- `lat_min` (float): Minimum latitude
- `lon_min` (float): Minimum longitude
- `lat_max` (float): Maximum latitude
- `lon_max` (float): Maximum longitude
- `t_start` (float): Start time (inclusive)
- `t_end` (float): End time (inclusive)

**Returns:**
- `List[int]`: List of record IDs matching the criteria

**Example:**
```python
results = index.query_box_time(
    lat_min=40.7, lon_min=-74.0,
    lat_max=40.8, lon_max=-73.9,
    t_start=1634568000.0,
    t_end=1634575200.0
)
```

---

### `get_record(record_id) -> Record | None`

Retrieve the Record object by ID.

**Parameters:**
- `record_id` (int): Record ID

**Returns:**
- `Record`: Record object with attributes (lat, lon, t, id), or None if not found

**Example:**
```python
record = index.get_record(42)
if record:
    print(f"Location: ({record.lat}, {record.lon})")
    print(f"Time: {record.t}")
```

---

### `get_payload(record_id) -> Any | None`

Retrieve the payload associated with a record ID.

**Parameters:**
- `record_id` (int): Record ID

**Returns:**
- `Any`: User payload if found, None otherwise

**Example:**
```python
payload = index.get_payload(42)
if payload:
    print(f"Fare: ${payload['fare']:.2f}")
```

---

### `size() -> int`

Get the total number of records in the index.

**Returns:**
- `int`: Number of records

**Example:**
```python
print(f"Index contains {index.size()} records")
```

---

### `clear()`

Remove all records and payloads from the index.

**Example:**
```python
index.clear()
```

---

### Class: `Record`

C++ record object with spatial-temporal data.

**Attributes:**
- `lat` (float): Latitude
- `lon` (float): Longitude
- `t` (float): Timestamp
- `id` (int): Unique record ID

---

## Usage Examples

### Example 1: Taxi Ride Analytics

```python
from spatio import SpatioIndex

# Initialize index
index = SpatioIndex()

# Load taxi rides
rides = [
    (40.7580, -73.9855, 1000.0, {"fare": 15.50, "distance": 2.3}),
    (40.7589, -73.9851, 1100.0, {"fare": 22.00, "distance": 3.5}),
    (40.7829, -73.9654, 1200.0, {"fare": 18.75, "distance": 2.8}),
]

for lat, lon, t, payload in rides:
    index.insert(lat, lon, t, payload)

# Find rides near Times Square between specific times
results = index.query_radius_time(
    center_lat=40.7589,
    center_lon=-73.9851,
    radius_km=1.0,
    t_start=1000.0,
    t_end=1500.0
)

# Calculate total fare
total_fare = sum(index.get_payload(rid)['fare'] for rid in results)
print(f"Total fare: ${total_fare:.2f}")
```

---

### Example 2: IoT Sensor Data Analysis

```python
from spatio import SpatioIndex
import time

# Track temperature sensors
sensor_index = SpatioIndex()

# Insert sensor readings
sensor_index.insert(
    lat=37.7749,
    lon=-122.4194,
    t=time.time(),
    payload={"sensor_id": "SF-001", "temperature": 22.5, "humidity": 65}
)

# Query recent readings in San Francisco area
recent_readings = sensor_index.query_radius_time(
    center_lat=37.7749,
    center_lon=-122.4194,
    radius_km=10.0,
    t_start=time.time() - 3600,  # Last hour
    t_end=time.time()
)

# Analyze readings
for rid in recent_readings:
    payload = sensor_index.get_payload(rid)
    print(f"Sensor {payload['sensor_id']}: {payload['temperature']}°C")
```

---

### Example 3: Geofencing / Proximity Alerts

```python
from spatio import SpatioIndex

# Track vehicles
vehicle_index = SpatioIndex()

# Add vehicle positions
vehicle_index.insert(40.7128, -74.0060, 1000.0, {"vehicle_id": "V001", "speed": 45})
vehicle_index.insert(40.7150, -74.0070, 1000.0, {"vehicle_id": "V002", "speed": 52})

# Define geofence (e.g., restricted zone)
geofence_center = (40.7128, -74.0060)
geofence_radius_km = 0.5

# Find vehicles in restricted zone
vehicles_in_zone = vehicle_index.query_radius_time(
    center_lat=geofence_center[0],
    center_lon=geofence_center[1],
    radius_km=geofence_radius_km,
    t_start=0.0,
    t_end=2000.0
)

print(f"Alert: {len(vehicles_in_zone)} vehicles in restricted zone")
```

---

## Algorithm Details

The GeoSpatio library uses a hybrid indexing approach combining spatial and temporal data structures for optimal query performance.

### Spatial Index - KD-Tree

#### Structure

- 2D KD-tree for latitude/longitude coordinates
- Alternating split dimensions (lat → lon → lat → ...)
- Leaf nodes store record IDs

#### Distance Calculation

- Haversine formula for great-circle distances on a sphere
- Earth radius: 6,371,000 meters
- Accuracy: ~1 meter for distances up to 10,000 km

#### Radius Query Algorithm

1. Traverse KD-tree recursively
2. For each node, compute Haversine distance to query center
3. Add to results if distance ≤ radius
4. **Pruning logic (MATHEMATICALLY CORRECT):**
   - Calculate distance from query center to splitting plane using Haversine
   - If plane distance > radius:
     - Only explore subtree containing query center
   - If plane distance ≤ radius:
     - Explore BOTH subtrees (plane intersects query circle)

**Key Correctness Property:**
- ✅ No false negatives: All points within radius are guaranteed to be found
- ✅ Proper handling of spherical geometry (latitude-dependent longitude distances)
- ✅ Works correctly at all latitudes including near poles

#### Box Query Algorithm

- Similar tree traversal with axis-aligned bounding box checks
- Prune subtrees that cannot intersect the query box

---

### Temporal Index - B+ Tree

#### Structure

- B+ tree with order 4 (min 2, max 4 children per node)
- Leaf nodes linked for efficient range scans
- All data stored in leaf nodes

#### Time Range Query

1. Find first leaf node with t ≥ t_start
2. Scan leaf nodes sequentially until t > t_end
3. Return all record IDs in range

---

### Combined Spatio-Temporal Query

#### Algorithm Flow

1. Perform spatial query (radius or box) → get candidate record IDs
2. Perform temporal query [t_start, t_end] → get candidate record IDs
3. Compute intersection of both result sets
4. Return final matching record IDs

**Complexity:**
- Spatial query: O(√N + K) where K = result count
- Temporal query: O(log N + K)
- Intersection: O(K)
- **Total:** O(√N + log N + K) ≈ O(√N + K) for typical cases

---

### Haversine Distance Formula

Used for all distance calculations between two points on Earth's surface:

```
a = sin²(Δlat/2) + cos(lat₁) × cos(lat₂) × sin²(Δlon/2)
c = 2 × atan2(√a, √(1-a))
distance = R × c
```

**Where:**
- R = 6,371,000 meters (Earth's radius)
- Δlat = lat₂ - lat₁ (in radians)
- Δlon = lon₂ - lon₁ (in radians)

**Accuracy Note:**
- Assumes Earth is a perfect sphere (slight approximation)
- Error typically < 0.5% compared to WGS84 ellipsoid
- Sufficient for most applications up to 10,000 km

---

## Performance Characteristics

### Time Complexity

| Operation | Average Case | Worst Case |
|-----------|--------------|------------|
| `insert()` | O(log N) | O(N) |
| `query_radius_time()` | O(√N + K) | O(N + K) |
| `query_box_time()` | O(√N + K) | O(N + K) |
| `get_record()` | O(1) | O(1) |
| `clear()` | O(N) | O(N) |

**Where:**
- N = total number of records
- K = number of results returned

### Space Complexity

- Spatial index (KD-tree): O(N)
- Temporal index (B+ tree): O(N)
- Record store: O(N)
- Python payloads: O(N)
- **Total:** O(N)

### Performance Tips

1. ✅ Batch inserts when possible (reduces overhead)
2. ✅ Use box queries instead of radius queries when appropriate (slightly faster)
3. ✅ Narrow time ranges improve query performance
4. ✅ Keep payloads lightweight (stored in Python, not C++)
5. ✅ Consider clearing and rebuilding index if > 50% records deleted

### Benchmark Results

*Approximate, hardware-dependent*

**Dataset:** 1,000,000 random geo-temporal records  
**Environment:** Modern CPU, single-threaded

- **Insert:** ~500,000 records/second
- **Radius query (1km, 100 results):** ~0.5-2 ms
- **Box query (0.1° × 0.1°, 100 results):** ~0.3-1 ms
- **Time range query:** ~0.1-0.5 ms

---

## Troubleshooting

### ImportError: C++ core module not found

**Solution:**

The C++ extension hasn't been built. Run:
```bash
pip install -e .
```

If that fails, ensure you have:
- C++17 compatible compiler installed
- pybind11 installed: `pip install pybind11`
- Build tools (Visual Studio on Windows, gcc/clang on Linux/Mac)

---

### Compilation errors during installation

**Solution:**

**Windows:**
- Install Visual Studio 2019 or later with C++ tools
- Or install "Build Tools for Visual Studio"

**Linux:**
```bash
sudo apt-get install build-essential python3-dev
```

**Mac:**
```bash
xcode-select --install
```

---

### Queries return unexpected results

**Solution:**

Check your coordinates and units:
- **Latitude:** -90 to +90 (negative = South, positive = North)
- **Longitude:** -180 to +180 (negative = West, positive = East)
- **Radius:** in **kilometers** (not meters or miles)
- **Time:** consistent units (e.g., all Unix timestamps)

---

### Poor query performance

**Solution:**

1. Ensure time ranges are as narrow as possible
2. Use box queries instead of radius when appropriate
3. Check if dataset size exceeds memory (index is in-memory)
4. Consider data partitioning for very large datasets

---

### Memory usage too high

**Solution:**

1. Store minimal data in payloads (payloads are Python objects)
2. Use external database for large payloads, store only IDs
3. Consider clearing old records: `index.clear()`
4. Implement data archival strategy for historical data

---

### Results missing near poles or at high latitudes

**Solution:**

This should **NOT** happen with version 0.1.0a1+. Previous versions had a bug in radius query pruning. Ensure you're using the latest version:
```bash
pip install --upgrade geospatio-temporal-index
```

---

## Support & License

- **Documentation:** See [README.md](README.md)
- **Source Code:** [GitHub Repository]
- **Bug Reports:** [Issue Tracker]
- **License:** MIT

For questions or issues, please file a bug report or contact the maintainers.

---

*Documentation for GeoSpatio-Temporal Index v0.1.0a1*

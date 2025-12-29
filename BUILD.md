# Building and Testing Spatio

## Quick Start (Windows)

### Option 1: Development Install (Recommended)
```bash
# This installs the package in "editable" mode
pip install -e .
```

### Option 2: Regular Install
```bash
pip install .
```

### Option 3: Build in-place (Advanced)
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

# Copy the .pyd file to python/spatio/
# (CMakeLists.txt should do this automatically)
```

## Prerequisites

Before building, ensure you have:

1. **Python 3.7+** (you have this)
2. **CMake 3.15+**
   ```bash
   cmake --version
   ```
   If missing: Download from https://cmake.org/download/

3. **C++ Compiler**
   - Windows: Visual Studio 2017+ or MinGW
   - Check with: `cl` (MSVC) or `g++` (MinGW)

4. **pybind11**
   ```bash
   pip install pybind11
   ```

## Build Steps

```bash
# 1. Install pybind11
pip install pybind11

# 2. Build and install spatio in development mode
pip install -e .

# 3. Test the installation
python -c "from spatio import SpatioIndex; print('Success!')"

# 4. Run the example
python example.py
```

## Troubleshooting

### "CMake not found"
Install CMake from: https://cmake.org/download/
Or use: `pip install cmake`

### "pybind11 not found"
```bash
pip install pybind11
```

### "No C++ compiler"
Windows: Install Visual Studio Community (free)
- Download: https://visualstudio.microsoft.com/downloads/
- Select "Desktop development with C++" workload

### "Module still not found after install"
Check the build output for errors:
```bash
pip install -e . -v  # Verbose mode
```

## Quick Test Script

Create a simple test to verify everything works:

```python
# test_basic.py
from spatio import SpatioIndex

print("Creating index...")
index = SpatioIndex()

print("Inserting records...")
id1 = index.insert(40.7128, -74.0060, 1000.0, payload={"name": "Point A"})
id2 = index.insert(40.7589, -73.9851, 1500.0, payload={"name": "Point B"})

print(f"Inserted {index.size()} records")

print("Querying...")
results = index.query_radius_time(40.7128, -74.0060, 50.0, 0.0, 2000.0)
print(f"Found {len(results)} results: {results}")

for rid in results:
    payload = index.get_payload(rid)
    print(f"  - {rid}: {payload}")

print("✓ All tests passed!")
```

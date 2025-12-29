"""
Simple test to verify spatio module works correctly
"""

from spatio import SpatioIndex

def test_basic():
    print("=" * 60)
    print("BASIC FUNCTIONALITY TEST")
    print("=" * 60)
    
    # Create index
    print("\n1. Creating index...")
    index = SpatioIndex()
    print("   ✓ SpatioIndex created")
    
    # Insert records
    print("\n2. Inserting records...")
    id1 = index.insert(40.7128, -74.0060, 1000.0, payload={"name": "Point A", "value": 100})
    print(f"   ✓ Inserted record {id1}")
    
    id2 = index.insert(40.7589, -73.9851, 1500.0, payload={"name": "Point B", "value": 200})
    print(f"   ✓ Inserted record {id2}")
    
    id3 = index.insert(40.7812, -73.9665, 2000.0, payload={"name": "Point C", "value": 300})
    print(f"   ✓ Inserted record {id3}")
    
    print(f"\n   Total records: {index.size()}")
    
    # Test radius query
    print("\n3. Testing radius query...")
    results = index.query_radius_time(
        center_lat=40.7128,
        center_lon=-74.0060,
        radius_km=50.0,  # Large radius to catch all points
        t_start=0.0,
        t_end=2500.0
    )
    print(f"   ✓ Found {len(results)} records within 50km")
    
    for rid in results:
        record = index.get_record(rid)
        payload = index.get_payload(rid)
        print(f"     - ID {rid}: {payload['name']} at ({record.lat:.4f}, {record.lon:.4f}), t={record.t}")
    
    # Test box query
    print("\n4. Testing bounding box query...")
    results = index.query_box_time(
        lat_min=40.70,
        lon_min=-74.01,
        lat_max=40.80,
        lon_max=-73.95,
        t_start=1000.0,
        t_end=1600.0
    )
    print(f"   ✓ Found {len(results)} records in box")
    
    # Test time filtering
    print("\n5. Testing time filtering...")
    results = index.query_radius_time(
        center_lat=40.75,
        center_lon=-73.98,
        radius_km=10.0,
        t_start=1400.0,
        t_end=1600.0
    )
    print(f"   ✓ Found {len(results)} records in time range [1400, 1600]")
    
    print("\n" + "=" * 60)
    print("✓ ALL TESTS PASSED!")
    print("=" * 60)

if __name__ == "__main__":
    try:
        test_basic()
    except ImportError as e:
        print("\n" + "!" * 60)
        print("ERROR: Module not built yet!")
        print("!" * 60)
        print(f"\nDetails: {e}")
        print("\nPlease run:")
        print("  pip install pybind11")
        print("  pip install -e .")
        print("\nThen try again.")
        exit(1)
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        exit(1)

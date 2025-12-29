"""
Example: Manhattan Taxi Rides Analysis

This example demonstrates how to use the Spatio-Temporal Index Engine
to query taxi rides in Manhattan during specific time periods.
"""

from spatio import SpatioIndex
import time

def main():
    print("=== Spatio-Temporal Index Engine Demo ===\n")
    
    # Create index
    index = SpatioIndex()
    print("Created new SpatioIndex\n")
    
    # Sample taxi ride data (lat, lon, timestamp, payload)
    rides = [
        # Times Square area
        (40.7580, -73.9855, 1000.0, {"location": "Times Square", "fare": 15.50, "distance": 2.3}),
        (40.7589, -73.9851, 1100.0, {"location": "Times Square", "fare": 22.00, "distance": 3.5}),
        
        # Central Park area
        (40.7829, -73.9654, 1200.0, {"location": "Central Park", "fare": 18.75, "distance": 2.8}),
        (40.7812, -73.9665, 1300.0, {"location": "Central Park", "fare": 12.25, "distance": 1.5}),
        
        # Financial District
        (40.7074, -74.0113, 1400.0, {"location": "Financial District", "fare": 28.50, "distance": 5.2}),
        (40.7092, -74.0132, 1500.0, {"location": "Financial District", "fare": 19.00, "distance": 3.1}),
        
        # Lower Manhattan (later time)
        (40.7128, -74.0060, 2000.0, {"location": "Lower Manhattan", "fare": 16.75, "distance": 2.5}),
        (40.7150, -74.0070, 2100.0, {"location": "Lower Manhattan", "fare": 21.00, "distance": 3.4}),
        
        # Midtown
        (40.7549, -73.9840, 2200.0, {"location": "Midtown", "fare": 14.50, "distance": 1.8}),
        (40.7580, -73.9862, 2300.0, {"location": "Midtown", "fare": 17.25, "distance": 2.2}),
    ]
    
    # Insert all rides
    print(f"Inserting {len(rides)} taxi rides...")
    for lat, lon, t, payload in rides:
        record_id = index.insert(lat, lon, t, payload)
        print(f"  Inserted ride {record_id}: {payload['location']} @ t={t}")
    
    print(f"\nTotal records in index: {index.size()}\n")
    
    # Query 1: Find rides near Times Square in the first half of the time period
    print("=" * 60)
    print("QUERY 1: Rides within 1km of Times Square (t: 1000-1500)")
    print("=" * 60)
    
    results = index.query_radius_time(
        center_lat=40.7589,   # Times Square coordinates
        center_lon=-73.9851,
        radius_km=1.0,
        t_start=1000.0,
        t_end=1500.0
    )
    
    print(f"Found {len(results)} rides:\n")
    for record_id in results:
        record = index.get_record(record_id)
        payload = index.get_payload(record_id)
        print(f"  ID {record_id}: {payload['location']}")
        print(f"    Coordinates: ({record.lat:.4f}, {record.lon:.4f})")
        print(f"    Time: {record.t}, Fare: ${payload['fare']:.2f}, Distance: {payload['distance']}km")
        print()
    
    # Query 2: Find rides in Lower Manhattan during later period
    print("=" * 60)
    print("QUERY 2: Rides in Lower Manhattan box (t: 1400-2400)")
    print("=" * 60)
    
    results = index.query_box_time(
        lat_min=40.70,
        lon_min=-74.02,
        lat_max=40.72,
        lon_max=-73.99,
        t_start=1400.0,
        t_end=2400.0
    )
    
    print(f"Found {len(results)} rides:\n")
    total_fare = 0.0
    for record_id in results:
        record = index.get_record(record_id)
        payload = index.get_payload(record_id)
        print(f"  ID {record_id}: {payload['location']}")
        print(f"    Coordinates: ({record.lat:.4f}, {record.lon:.4f})")
        print(f"    Time: {record.t}, Fare: ${payload['fare']:.2f}")
        total_fare += payload['fare']
        print()
    
    if results:
        print(f"Total fare from these rides: ${total_fare:.2f}")
        print(f"Average fare: ${total_fare / len(results):.2f}\n")
    
    # Query 3: All rides in a large area early in the day
    print("=" * 60)
    print("QUERY 3: All rides within 3km of Central Manhattan (t: 1000-1600)")
    print("=" * 60)
    
    results = index.query_radius_time(
        center_lat=40.7589,   # Central Manhattan
        center_lon=-73.9851,
        radius_km=3.0,
        t_start=1000.0,
        t_end=1600.0
    )
    
    print(f"Found {len(results)} rides:\n")
    for record_id in results:
        payload = index.get_payload(record_id)
        print(f"  {payload['location']}: ${payload['fare']:.2f}")
    
    print("\n=== Demo Complete ===")

if __name__ == "__main__":
    main()

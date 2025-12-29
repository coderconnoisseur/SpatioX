"""
Spatio - Spatial-Temporal Index Engine

A high-performance indexing system for spatial-temporal data with efficient
querying capabilities.

Example:
    >>> from spatio import SpatioIndex
    >>> index = SpatioIndex()
    >>> 
    >>> # Insert records with payloads
    >>> id1 = index.insert(40.7128, -74.0060, 1000.0, payload={"type": "taxi", "fare": 15.50})
    >>> id2 = index.insert(40.7589, -73.9851, 2000.0, payload={"type": "taxi", "fare": 22.00})
    >>> 
    >>> # Query by radius and time
    >>> results = index.query_radius_time(
    ...     center_lat=40.7128, 
    ...     center_lon=-74.0060, 
    ...     radius_km=5.0,
    ...     t_start=500.0,
    ...     t_end=1500.0
    ... )
    >>> 
    >>> # Get payloads
    >>> for record_id in results:
    ...     payload = index.get_payload(record_id)
    ...     print(payload)
"""

from typing import Any, List, Optional, Dict
try:
    from ._spatio_core import SpatioIndexCore, Record
except ImportError:
    # Module not built yet
    SpatioIndexCore = None
    Record = None

__version__ = "0.1.0"
__all__ = ["SpatioIndex", "Record"]


class SpatioIndex:
    """
    User-facing Python wrapper for the Spatial-Temporal Index Engine.
    
    This class combines the C++ core engine with Python-side payload storage,
    providing a simple interface for indexing and querying spatial-temporal data.
    
    Attributes:
        _core: C++ SpatioIndexCore instance
        _payloads: Dictionary mapping record IDs to user payloads
    """
    
    def __init__(self):
        """Initialize a new SpatioIndex"""
        if SpatioIndexCore is None:
            raise ImportError(
                "C++ core module not found. Please build the package first:\n"
                "  pip install -e ."
            )
        self._core = SpatioIndexCore()
        self._payloads: Dict[int, Any] = {}
    
    def insert(self, lat: float, lon: float, t: float, payload: Any = None) -> int:
        """
        Insert a new spatial-temporal record with optional payload.
        
        Args:
            lat: Latitude in degrees [-90, 90]
            lon: Longitude in degrees [-180, 180]
            t: Timestamp (seconds since epoch or arbitrary time unit)
            payload: Optional user data to associate with this record
        
        Returns:
            Unique record ID
        
        Example:
            >>> index = SpatioIndex()
            >>> record_id = index.insert(40.7128, -74.0060, 1634567890.0, 
            ...                          payload={"event": "taxi_pickup", "fare": 15.50})
        """
        record_id = self._core.insert(lat, lon, t)
        if payload is not None:
            self._payloads[record_id] = payload
        return record_id
    
    def query_radius_time(self, center_lat: float, center_lon: float, 
                         radius_km: float, t_start: float, t_end: float) -> List[int]:
        """
        Find all records within a radius and time range.
        
        Args:
            center_lat: Center latitude
            center_lon: Center longitude
            radius_km: Radius in kilometers
            t_start: Start time (inclusive)
            t_end: End time (inclusive)
        
        Returns:
            List of record IDs matching the criteria
        
        Example:
            >>> # Find all taxi rides in Manhattan between 3-5 PM
            >>> results = index.query_radius_time(
            ...     center_lat=40.7589, 
            ...     center_lon=-73.9851,
            ...     radius_km=2.0,
            ...     t_start=1634568000.0,  # 3 PM
            ...     t_end=1634575200.0     # 5 PM
            ... )
        """
        return self._core.query_radius_time(center_lat, center_lon, radius_km, t_start, t_end)
    
    def query_box_time(self, lat_min: float, lon_min: float,
                      lat_max: float, lon_max: float,
                      t_start: float, t_end: float) -> List[int]:
        """
        Find all records within a bounding box and time range.
        
        Args:
            lat_min: Minimum latitude
            lon_min: Minimum longitude
            lat_max: Maximum latitude
            lon_max: Maximum longitude
            t_start: Start time (inclusive)
            t_end: End time (inclusive)
        
        Returns:
            List of record IDs matching the criteria
        
        Example:
            >>> # Find all events in a rectangular area during a time range
            >>> results = index.query_box_time(
            ...     lat_min=40.7, lon_min=-74.0,
            ...     lat_max=40.8, lon_max=-73.9,
            ...     t_start=1634568000.0,
            ...     t_end=1634575200.0
            ... )
        """
        return self._core.query_box_time(lat_min, lon_min, lat_max, lon_max, t_start, t_end)
    
    def get_record(self, record_id: int) -> Optional[Record]:
        """
        Get the Record object by ID.
        
        Args:
            record_id: Record ID
        
        Returns:
            Record object if found, None otherwise
        """
        return self._core.get_record(record_id)
    
    def get_payload(self, record_id: int) -> Optional[Any]:
        """
        Get the payload associated with a record ID.
        
        Args:
            record_id: Record ID
        
        Returns:
            Payload if found, None otherwise
        """
        return self._payloads.get(record_id)
    
    def size(self) -> int:
        """
        Get the total number of records in the index.
        
        Returns:
            Number of records
        """
        return self._core.size()
    
    def clear(self):
        """
        Clear all records and payloads from the index.
        """
        self._core.clear()
        self._payloads.clear()

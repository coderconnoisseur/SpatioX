#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "spatio_index_core.hpp"
#include "record.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_spatio_core, m) {
    m.doc() = "Spatial-Temporal Index Engine - Production C++ Core";

    // ==================== BASIC TYPES ====================
    
    py::class_<spatio::RecordInput>(m, "RecordInput")
        .def(py::init<float, float, double>(),
             py::arg("lat"), py::arg("lon"), py::arg("t"))
        .def_readwrite("lat", &spatio::RecordInput::lat)
        .def_readwrite("lon", &spatio::RecordInput::lon)
        .def_readwrite("t", &spatio::RecordInput::t);

    py::class_<spatio::Record>(m, "Record")
        .def(py::init<>())
        .def(py::init<float, float, double, uint64_t>(),
             py::arg("lat"), py::arg("lon"), py::arg("t"), py::arg("id") = 0)
        .def_readwrite("lat", &spatio::Record::lat)
        .def_readwrite("lon", &spatio::Record::lon)
        .def_readwrite("t", &spatio::Record::t)
        .def_readwrite("id", &spatio::Record::id)
        .def("__repr__", [](const spatio::Record &r) {
            return "Record(lat=" + std::to_string(r.lat) + 
                   ", lon=" + std::to_string(r.lon) +
                   ", t=" + std::to_string(r.t) +
                   ", id=" + std::to_string(r.id) + ")";
        });

    // ==================== STATISTICS ====================
    
    py::class_<spatio::SpatioIndexCore::IndexStats>(m, "IndexStats")
        .def_readonly("total_records", &spatio::SpatioIndexCore::IndexStats::total_records)
        .def_readonly("spatial_nodes", &spatio::SpatioIndexCore::IndexStats::spatial_nodes)
        .def_readonly("temporal_entries", &spatio::SpatioIndexCore::IndexStats::temporal_entries)
        .def_readonly("min_time", &spatio::SpatioIndexCore::IndexStats::min_time)
        .def_readonly("max_time", &spatio::SpatioIndexCore::IndexStats::max_time)
        .def_readonly("is_built", &spatio::SpatioIndexCore::IndexStats::is_built)
        .def("__repr__", [](const spatio::SpatioIndexCore::IndexStats &s) {
            return "IndexStats(records=" + std::to_string(s.total_records) + 
                   ", spatial_nodes=" + std::to_string(s.spatial_nodes) +
                   ", temporal=[" + std::to_string(s.min_time) + ", " + 
                   std::to_string(s.max_time) + "], built=" + 
                   std::string(s.is_built ? "True" : "False") + ")";
        });

    py::class_<spatio::QueryStats>(m, "QueryStats")
        .def_readonly("spatial_nodes_visited", &spatio::QueryStats::spatial_nodes_visited)
        .def_readonly("spatial_distance_checks", &spatio::QueryStats::spatial_distance_checks)
        .def_readonly("spatial_bbox_prunes", &spatio::QueryStats::spatial_bbox_prunes)
        .def_readonly("spatial_distance_prunes", &spatio::QueryStats::spatial_distance_prunes)
        .def_readonly("records_filtered_by_time", &spatio::QueryStats::records_filtered_by_time)
        .def_readonly("records_passed_time_filter", &spatio::QueryStats::records_passed_time_filter)
        .def_readonly("result_count", &spatio::QueryStats::result_count)
        .def("__repr__", [](const spatio::QueryStats &s) {
            return "QueryStats(nodes=" + std::to_string(s.spatial_nodes_visited) +
                   ", dist_checks=" + std::to_string(s.spatial_distance_checks) +
                   ", bbox_prunes=" + std::to_string(s.spatial_bbox_prunes) +
                   ", time_filtered=" + std::to_string(s.records_filtered_by_time) +
                   ", results=" + std::to_string(s.result_count) + ")";
        });

    // ==================== MAIN INDEX CLASS ====================
    
    py::class_<spatio::SpatioIndexCore>(m, "SpatioIndexCore")
        .def(py::init<>())
        
        // ===== INSERTION =====
        .def("insert", &spatio::SpatioIndexCore::insert,
             py::arg("lat"), py::arg("lon"), py::arg("t"),
             "Insert a single record (online/streaming path)")
        
        .def("bulk_insert", 
             [](spatio::SpatioIndexCore& self, const std::vector<std::tuple<float, float, double>>& data) {
                 std::vector<spatio::RecordInput> records;
                 records.reserve(data.size());
                 for (const auto& [lat, lon, t] : data) {
                     records.emplace_back(lat, lon, t);
                 }
                 return self.bulk_insert(records);
             },
             py::arg("records"),
             "Bulk insert from list of (lat, lon, t) tuples")
        
        .def("build", &spatio::SpatioIndexCore::build,
             "Explicit build phase")
        
        // ===== SPATIAL-ONLY QUERIES =====
        .def("query_radius", &spatio::SpatioIndexCore::query_radius,
             py::arg("center_lat"), py::arg("center_lon"), py::arg("radius_km"),
             "Query by radius (no time filter)")
        
        .def("query_box", &spatio::SpatioIndexCore::query_box,
             py::arg("lat_min"), py::arg("lon_min"), 
             py::arg("lat_max"), py::arg("lon_max"),
             "Query by bounding box (no time filter)")
        
        .def("query_knn", &spatio::SpatioIndexCore::query_knn,
             py::arg("lat"), py::arg("lon"), py::arg("k"),
             "K-nearest neighbors (no time filter)")
        
        // ===== SPATIAL + TEMPORAL QUERIES =====
        .def("query_radius_time", &spatio::SpatioIndexCore::query_radius_time,
             py::arg("center_lat"), py::arg("center_lon"), py::arg("radius_km"),
             py::arg("t_start"), py::arg("t_end"),
             "Query by radius and time range")
        
        .def("query_box_time", &spatio::SpatioIndexCore::query_box_time,
             py::arg("lat_min"), py::arg("lon_min"), 
             py::arg("lat_max"), py::arg("lon_max"),
             py::arg("t_start"), py::arg("t_end"),
             "Query by bounding box and time range")
        
        .def("query_knn_time", &spatio::SpatioIndexCore::query_knn_time,
             py::arg("lat"), py::arg("lon"), py::arg("k"),
             py::arg("t_start"), py::arg("t_end"),
             "K-nearest neighbors with time filter")
        
        // ===== INSTRUMENTED QUERIES =====
        .def("query_radius_time_instrumented",
             [](const spatio::SpatioIndexCore& self, float lat, float lon, double radius,
                double t_start, double t_end) {
                 spatio::QueryStats stats;
                 auto results = self.query_radius_time_instrumented(lat, lon, radius, 
                                                                    t_start, t_end, stats);
                 return py::make_tuple(results, stats);
             },
             py::arg("center_lat"), py::arg("center_lon"), py::arg("radius_km"),
             py::arg("t_start"), py::arg("t_end"),
             "Query with performance statistics. Returns (results, stats)")
        
        // ===== DATA ACCESS =====
        .def("get_record", &spatio::SpatioIndexCore::get_record,
             py::arg("id"),
             "Get record by ID (returns None if not found)")
        
        .def("size", &spatio::SpatioIndexCore::size,
             "Get total number of records")
        
        .def("clear", &spatio::SpatioIndexCore::clear,
             "Clear all data")
        
        // ===== STATISTICS =====
        .def("get_index_stats", &spatio::SpatioIndexCore::get_index_stats,
             "Get comprehensive index statistics");
}
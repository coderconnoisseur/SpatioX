#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "spatio_index_core.hpp"
#include "record.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_spatio_core, m) {
    m.doc() = "Spatial-Temporal Index Engine - C++ Core";

    // Bind Record struct
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

    // Bind SpatioIndexCore
    py::class_<spatio::SpatioIndexCore>(m, "SpatioIndexCore")
        .def(py::init<>())
        .def("insert", &spatio::SpatioIndexCore::insert,
             py::arg("lat"), py::arg("lon"), py::arg("t"),
             "Insert a new spatial-temporal record")
        .def("query_radius_time", &spatio::SpatioIndexCore::query_radius_time,
             py::arg("center_lat"), py::arg("center_lon"), py::arg("radius_km"),
             py::arg("t_start"), py::arg("t_end"),
             "Query by radius and time range")
        .def("query_box_time", &spatio::SpatioIndexCore::query_box_time,
             py::arg("lat_min"), py::arg("lon_min"), 
             py::arg("lat_max"), py::arg("lon_max"),
             py::arg("t_start"), py::arg("t_end"),
             "Query by bounding box and time range")
        .def("get_record", &spatio::SpatioIndexCore::get_record,
             py::arg("id"),
             "Get record by ID")
        .def("size", &spatio::SpatioIndexCore::size,
             "Get total number of records")
        .def("clear", &spatio::SpatioIndexCore::clear,
             "Clear all data");
}

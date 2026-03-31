namespace py = pybind11;
namespace UC::Metrics {

void bind_monitor(py::module_& m)
{
    m.def("set_up", &SetUp);
    m.def("create_stats", &CreateStats);
    m.def("update_stats", py::overload_cast<const std::string&, double>(&UpdateStats));
    m.def("update_stats",
          py::overload_cast<const std::unordered_map<std::string, double>&>(&UpdateStats));
    m.def("get_all_stats_and_clear", []() {
        py::gil_scoped_release releaseGil;
        return GetAllStatsAndClear();
    });
}

}  // namespace UC::Metrics

PYBIND11_MODULE(ucmmetrics, module)
{
    module.attr("project") = UCM_PROJECT_NAME;
    module.attr("version") = UCM_PROJECT_VERSION;
    module.attr("commit_id") = UCM_COMMIT_ID;
    module.attr("build_type") = UCM_BUILD_TYPE;
    UC::Metrics::bind_monitor(module);
}
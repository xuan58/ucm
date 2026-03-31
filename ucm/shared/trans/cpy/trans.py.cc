namespace py = pybind11;

namespace UC::Trans {

using Ptr = uintptr_t;
using PtrArray = py::array_t<uintptr_t>;

inline void ThrowIfFailed(const Status& s)
{
    if (s.Failure()) [[unlikely]] { throw std::runtime_error{s.ToString()}; }
}

inline void DeviceToHost(Stream& self, Ptr src, Ptr dst, size_t size)
{
    ThrowIfFailed(self.DeviceToHost((void*)src, (void*)dst, size));
}

inline void DeviceToHostBatch(Stream& self, py::object src, py::object dst, size_t size,
                              size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto device = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        auto host = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.DeviceToHost(device, host, size, number));
    } else {
        auto device = static_cast<void**>((void*)src.cast<Ptr>());
        auto host = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.DeviceToHost(device, host, size, number));
    }
}

inline void DeviceToHostGather(Stream& self, py::object src, Ptr dst, size_t size, size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto device = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.DeviceToHost(device, (void*)dst, size, number));
    } else {
        auto device = static_cast<void**>((void*)src.cast<Ptr>());
        ThrowIfFailed(self.DeviceToHost(device, (void*)dst, size, number));
    }
}

inline void DeviceToHostAsync(Stream& self, Ptr src, Ptr dst, size_t size)
{
    ThrowIfFailed(self.DeviceToHostAsync((void*)src, (void*)dst, size));
}

inline void DeviceToHostBatchAsync(Stream& self, py::object src, py::object dst, size_t size,
                                   size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto device = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        auto host = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.DeviceToHostAsync(device, host, size, number));
    } else {
        auto device = static_cast<void**>((void*)src.cast<Ptr>());
        auto host = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.DeviceToHostAsync(device, host, size, number));
    }
}

inline void DeviceToHostGatherAsync(Stream& self, py::object src, Ptr dst, size_t size,
                                    size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto device = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.DeviceToHostAsync(device, (void*)dst, size, number));
    } else {
        auto device = static_cast<void**>((void*)src.cast<Ptr>());
        ThrowIfFailed(self.DeviceToHostAsync(device, (void*)dst, size, number));
    }
}

inline void HostToDevice(Stream& self, Ptr src, Ptr dst, size_t size)
{
    ThrowIfFailed(self.HostToDevice((void*)src, (void*)dst, size));
}

inline void HostToDeviceBatch(Stream& self, py::object src, py::object dst, size_t size,
                              size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto host = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        auto device = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.HostToDevice(host, device, size, number));
    } else {
        auto host = static_cast<void**>((void*)src.cast<Ptr>());
        auto device = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.HostToDevice(host, device, size, number));
    }
}

inline void HostToDeviceScatter(Stream& self, Ptr src, py::object dst, size_t size, size_t number)
{
    if (py::isinstance<PtrArray>(dst)) {
        auto device = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.HostToDevice((void*)src, device, size, number));
    } else {
        auto device = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.HostToDevice((void*)src, device, size, number));
    }
}

inline void HostToDeviceAsync(Stream& self, Ptr src, Ptr dst, size_t size)
{
    ThrowIfFailed(self.HostToDeviceAsync((void*)src, (void*)dst, size));
}

inline void HostToDeviceBatchAsync(Stream& self, py::object src, py::object dst, size_t size,
                                   size_t number)
{
    if (py::isinstance<PtrArray>(src)) {
        auto host = static_cast<void**>(src.cast<PtrArray>().request().ptr);
        auto device = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.HostToDeviceAsync(host, device, size, number));
    } else {
        auto host = static_cast<void**>((void*)src.cast<Ptr>());
        auto device = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.HostToDeviceAsync(host, device, size, number));
    }
}

inline void HostToDeviceScatterAsync(Stream& self, Ptr src, py::object dst, size_t size,
                                     size_t number)
{
    if (py::isinstance<PtrArray>(dst)) {
        auto device = static_cast<void**>(dst.cast<PtrArray>().request().ptr);
        ThrowIfFailed(self.HostToDeviceAsync((void*)src, device, size, number));
    } else {
        auto device = static_cast<void**>((void*)dst.cast<Ptr>());
        ThrowIfFailed(self.HostToDeviceAsync((void*)src, device, size, number));
    }
}

} // namespace UC::Trans

PYBIND11_MODULE(ucmtrans, m)
{
    using namespace UC::Trans;
    m.attr("project") = UCM_PROJECT_NAME;
    m.attr("version") = UCM_PROJECT_VERSION;
    m.attr("commit_id") = UCM_COMMIT_ID;
    m.attr("build_type") = UCM_BUILD_TYPE;

    auto s = py::class_<Stream, std::unique_ptr<Stream>>(m, "Stream");
    s.def("DeviceToHost", &DeviceToHost);
    s.def("DeviceToHostBatch", &DeviceToHostBatch);
    s.def("DeviceToHostGather", &DeviceToHostGather);
    s.def("DeviceToHostAsync", &DeviceToHostAsync);
    s.def("DeviceToHostBatchAsync", &DeviceToHostBatchAsync);
    s.def("DeviceToHostGatherAsync", &DeviceToHostGatherAsync);
    s.def("HostToDevice", &HostToDevice);
    s.def("HostToDeviceBatch", &HostToDeviceBatch);
    s.def("HostToDeviceScatter", &HostToDeviceScatter);
    s.def("HostToDeviceAsync", &HostToDeviceAsync);
    s.def("HostToDeviceBatchAsync", &HostToDeviceBatchAsync);
    s.def("HostToDeviceScatterAsync", &HostToDeviceScatterAsync);
    s.def("Synchronized", [](Stream& self) { ThrowIfFailed(self.Synchronized()); });

    auto d = py::class_<Device>(m, "Device");
    d.def(py::init<>());
    d.def("Setup", [](Device& self, int32_t deviceId) { ThrowIfFailed(self.Setup(deviceId)); });
    d.def("MakeStream", &Device::MakeStream);
    d.def("MakeSMStream", &Device::MakeSMStream);
}

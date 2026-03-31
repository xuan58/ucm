namespace vllm_ascend {

class NPUBridge {
public:
    // at::tensor to NPUStorageImpl
    static NPUStorageImpl* GetNpuStorageImpl(const at::Tensor& tensor);

    // c10::StorageImpl to NPUStorageImpl
    static NPUStorageImpl* GetNpuStorageImpl(c10::StorageImpl* storageImpl);

    // c10::Storage to NPUStorageImpl
    static NPUStorageImpl* GetNpuStorageImpl(c10::Storage&& storage);

    // tensor to NPUStorageDesc
    static NPUStorageDesc& GetNpuStorageImplDesc(const at::Tensor& tensor);
};
}  // namespace vllm_ascend

namespace UC::Trans {

#define CUDA_TRANS_UNIT_SIZE (sizeof(uint4) * 2)
#define CUDA_TRANS_BLOCK_NUMBER (32)
#define CUDA_TRANS_BLOCK_SIZE (256)
#define CUDA_TRANS_THREAD_NUMBER (CUDA_TRANS_BLOCK_NUMBER * CUDA_TRANS_BLOCK_SIZE)

inline __device__ void CudaCopyUnit(const uint8_t* __restrict__ src,
                                    volatile uint8_t* __restrict__ dst)
{
    const uint4* src4 = reinterpret_cast<const uint4*>(src);
    uint4 lo = __ldcs(src4);
    uint4 hi = __ldcs(src4 + 1);

    uint8_t* nv_dst = const_cast<uint8_t*>(dst);
    uint4* dst4 = reinterpret_cast<uint4*>(nv_dst);
    __stcg(dst4, lo);
    __stcg(dst4 + 1, hi);
}

__global__ void CudaCopyKernel(const void** src, void** dst, size_t size, size_t num)
{
    auto length = size * num;
    auto offset = (blockIdx.x * blockDim.x + threadIdx.x) * CUDA_TRANS_UNIT_SIZE;
    while (offset + CUDA_TRANS_UNIT_SIZE <= length) {
        auto idx = offset / size;
        auto off = offset % size;
        auto host = ((const uint8_t*)src[idx]) + off;
        auto device = ((uint8_t*)dst[idx]) + off;
        CudaCopyUnit(host, device);
        offset += CUDA_TRANS_THREAD_NUMBER * CUDA_TRANS_UNIT_SIZE;
    }
}

__global__ void CudaCopyKernel(const void** src, void* dst, size_t size, size_t num)
{
    auto length = size * num;
    auto offset = (blockIdx.x * blockDim.x + threadIdx.x) * CUDA_TRANS_UNIT_SIZE;
    while (offset + CUDA_TRANS_UNIT_SIZE <= length) {
        auto idx = offset / size;
        auto off = offset % size;
        auto host = ((const uint8_t*)src[idx]) + off;
        auto device = ((uint8_t*)dst) + offset;
        CudaCopyUnit(host, device);
        offset += CUDA_TRANS_THREAD_NUMBER * CUDA_TRANS_UNIT_SIZE;
    }
}

__global__ void CudaCopyKernel(const void* src, void** dst, size_t size, size_t num)
{
    auto length = size * num;
    auto offset = (blockIdx.x * blockDim.x + threadIdx.x) * CUDA_TRANS_UNIT_SIZE;
    while (offset + CUDA_TRANS_UNIT_SIZE <= length) {
        auto idx = offset / size;
        auto off = offset % size;
        auto host = ((const uint8_t*)src) + offset;
        auto device = ((uint8_t*)dst[idx]) + off;
        CudaCopyUnit(host, device);
        offset += CUDA_TRANS_THREAD_NUMBER * CUDA_TRANS_UNIT_SIZE;
    }
}

cudaError_t CudaSMCopyAsync(void* src[], void* dst[], size_t size, size_t number,
                            cudaStream_t stream)
{
    CudaCopyKernel<<<CUDA_TRANS_BLOCK_NUMBER, CUDA_TRANS_BLOCK_SIZE, 0, stream>>>(
        (const void**)src, dst, size, number);
    return cudaGetLastError();
}

cudaError_t CudaSMCopyAsync(void* src[], void* dst, size_t size, size_t number, cudaStream_t stream)
{
    CudaCopyKernel<<<CUDA_TRANS_BLOCK_NUMBER, CUDA_TRANS_BLOCK_SIZE, 0, stream>>>(
        (const void**)src, dst, size, number);
    return cudaGetLastError();
}

cudaError_t CudaSMCopyAsync(void* src, void* dst[], size_t size, size_t number, cudaStream_t stream)
{
    CudaCopyKernel<<<CUDA_TRANS_BLOCK_NUMBER, CUDA_TRANS_BLOCK_SIZE, 0, stream>>>(
        (const void*)src, dst, size, number);
    return cudaGetLastError();
}

} // namespace UC::Trans

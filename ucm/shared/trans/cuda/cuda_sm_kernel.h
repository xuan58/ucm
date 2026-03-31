namespace UC::Trans {

cudaError_t CudaSMCopyAsync(void* src[], void* dst[], size_t size, size_t number,
                            cudaStream_t stream);
cudaError_t CudaSMCopyAsync(void* src[], void* dst, size_t size, size_t number,
                            cudaStream_t stream);
cudaError_t CudaSMCopyAsync(void* src, void* dst[], size_t size, size_t number,
                            cudaStream_t stream);

} // namespace UC::Trans

#endif

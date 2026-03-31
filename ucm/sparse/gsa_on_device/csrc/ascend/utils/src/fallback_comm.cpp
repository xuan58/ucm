extern "C" {
#endif

namespace fallback {
using namespace std;
using namespace gert;
using namespace ge;

aclDataType ToAclDataType(ge::DataType dtype)
{
    static const std::vector<DataType> CANN_CONVERT_TO_ACL_DataType_LIST = {
        ge::DataType::DT_FLOAT,     ge::DataType::DT_FLOAT16,    ge::DataType::DT_INT8,
        ge::DataType::DT_INT32,     ge::DataType::DT_UINT8,      ge::DataType::DT_INT16,
        ge::DataType::DT_UINT16,    ge::DataType::DT_UINT32,     ge::DataType::DT_INT64,
        ge::DataType::DT_DOUBLE,    ge::DataType::DT_BOOL,       ge::DataType::DT_STRING,
        ge::DataType::DT_COMPLEX64, ge::DataType::DT_COMPLEX128, ge::DataType::DT_BF16,
        ge::DataType::DT_UINT64,    ge::DataType::DT_INT4};
    auto iter = std::find(CANN_CONVERT_TO_ACL_DataType_LIST.begin(),
                          CANN_CONVERT_TO_ACL_DataType_LIST.end(), dtype);
    if (iter == CANN_CONVERT_TO_ACL_DataType_LIST.end()) { return aclDataType::ACL_DT_UNDEFINED; }
    return static_cast<aclDataType>(dtype);
}

}  // namespace fallback

#ifdef __cplusplus
}
#endif

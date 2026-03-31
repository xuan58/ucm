    OPS_LOG_STUB_IF(COND, OPS_LOG_D(OP_DESC, __VA_ARGS__), EXPR)
#define OPS_LOG_I_IF(COND, OP_DESC, EXPR, ...) \
    OPS_LOG_STUB_IF(COND, OPS_LOG_I(OP_DESC, __VA_ARGS__), EXPR)
#define OPS_LOG_W_IF(COND, OP_DESC, EXPR, ...) \
    OPS_LOG_STUB_IF(COND, OPS_LOG_W(OP_DESC, __VA_ARGS__), EXPR)
#define OPS_LOG_E_IF(COND, OP_DESC, EXPR, ...) \
    OPS_LOG_STUB_IF(COND, OPS_LOG_E(OP_DESC, __VA_ARGS__), EXPR)
#define OPS_LOG_EVENT_IF(COND, OP_DESC, EXPR, ...) \
    OPS_LOG_STUB_IF(COND, OPS_LOG_EVENT(OP_DESC, __VA_ARGS__), EXPR)

#define OPS_LOG_E_IF_NULL(OPS_DESC, PTR, EXPR)                         \
    if (__builtin_expect((PTR) == nullptr, 0)) {                       \
        OPS_LOG_STUB_E(OPS_DESC, "%s is nullptr!", #PTR);              \
        OPS_CALL_ERR_STUB("EZ9999", OPS_DESC, "%s is nullptr!", #PTR); \
        EXPR;                                                          \
    }

#define OPS_CHECK(COND, LOG_FUNC, EXPR) \
    if (COND) {                         \
        LOG_FUNC;                       \
        EXPR;                           \
    }

#define OP_CHECK(COND, LOG_FUNC, EXPR) \
    if (COND) {                        \
        LOG_FUNC;                      \
        EXPR;                          \
    }

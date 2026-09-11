/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#if defined(lbfw51Tensor_DLL)
#if defined(_WIN32)
#if defined(lbfw51Tensor_EXPORTS)
#define TENSOR_EXPORT __declspec(dllexport)
#define TENSOR_EXPORT_TEMPLATE
#else
#define TENSOR_EXPORT __declspec(dllimport)
#define TENSOR_EXPORT_TEMPLATE extern
#endif
#else
#define TENSOR_EXPORT
#define TENSOR_EXPORT_TEMPLATE
#endif
#else
#define TENSOR_EXPORT
#define TENSOR_EXPORT_TEMPLATE
#endif

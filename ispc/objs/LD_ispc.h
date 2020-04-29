//
// objs/LD_ispc.h
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#pragma once
#include <stdint.h>



#ifdef __cplusplus
namespace ispc { /* namespace */
#endif // __cplusplus

#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif


///////////////////////////////////////////////////////////////////////////
// Functions exported from ispc code
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
extern "C" {
#endif // __cplusplus
    extern void LD_diag_ispc(int32_t diag_start, int32_t start, int32_t end, int32_t left, int32_t top_left, int32_t * D);
    extern void LD_ispc(int32_t * str1, int32_t * str2, int32_t start_i, int32_t start_j, int32_t diag_len, int32_t D[][1000]);
    extern void LD_ispc_withtasks(int32_t * str1, int32_t * str2, int32_t start_i, int32_t start_j, int32_t diag_len, int32_t D[][1000]);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus

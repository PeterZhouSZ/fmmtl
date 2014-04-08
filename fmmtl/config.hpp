#pragma once

// Whole suite of system configuration flags
#include <boost/version.hpp>
#include <boost/config.hpp>

#include <omp.h>

#if defined(__CUDACC__)
#  define FMMTL_WITH_CUDA
#endif

// FMMTL_INLINE
#if defined(__CUDACC__)        // Compiling with nvcc
#  define FMMTL_INLINE __host__ __device__ inline
#else                          // Not compiling with nvcc
#  define FMMTL_INLINE inline
#endif

#if defined(FMMTL_WITH_CUDA)  // Enable CUDA/Thrust accleration
#  include <thrust/version.h>
#  if (THRUST_VERSION < 100700)
#    error Need Thrust v1.7. Please upgrade to CUDA 5.5.
#  endif
#  include <thrust/detail/config.h>
#  include <cublas.h>
#endif

// Enable performance options in NDEBUG mode
#if defined(FMMTL_NDEBUG)
#  define FMMTL_DISABLE_ASSERTS
#  define FMMTL_DISABLE_CUDA_CHECKS
#endif

// Disable performance options in DEBUG mode
#if defined(FMMTL_DEBUG)
#  undef FMMTL_DISABLE_ASSERTS
#  undef FMMTL_DISABLE_CUDA_CHECKS
#endif

// FMMTL_ASSERT
#undef FMMTL_ASSERT
#if defined(FMMTL_DISABLE_ASSERTS)
#  define FMMTL_ASSERT(expr) ((void)0)
#else
#  include <cassert>
#  define FMMTL_ASSERT(expr) assert(expr)
#endif
// FMMTL_STATIC_ASSERT (for C++03 regions)
#include <boost/static_assert.hpp>
#define FMMTL_STATIC_ASSERT BOOST_STATIC_ASSERT_MSG

// FMMTL_CUDA_CHECK
#undef FMMTL_CUDA_CHECK
#if !defined(FMMTL_WITH_CUDA) || defined(FMMTL_DISABLE_CUDA_CHECKS)
#  define FMMTL_CUDA_CHECK ((void)0)
#else
#  include <cstdlib>
#  include <cstdio>
inline void cuda_check(char* file, int line) {
  cudaError_t code = cudaDeviceSynchronize();
  if (code != cudaSuccess) {
    fprintf(stderr,"CUDA assert: %s %s %d\n",cudaGetErrorString(code),file,line);
    exit(code);
  }
}
#  define FMMTL_CUDA_CHECK cuda_check(__FILE__, __LINE__)
#endif

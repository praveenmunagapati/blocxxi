//        Copyright The Authors 2018.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#pragma once

// ==== Darwin/BSD ===
#if (defined __APPLE__ && defined __MACH__) || defined __FreeBSD__ || \
    defined __NetBSD__ || defined __OpenBSD__ || defined __bsdi__ ||  \
    defined __DragonFly__ || defined __FreeBSD_kernel__
#define BLOCXXI_BSD
#if defined __APPLE__

// execinfo.h is available in the MacOS X 10.5 SDK.
#define BLOCXXI_USE_EXECINFO 1

#endif  // __APPLE__

// ==== LINUX ===
#elif defined __linux__
#define BLOCXXI_LINUX

#if defined __GLIBC__ && (defined __x86_64__ || defined __i386 || \
                          defined _M_X64 || defined _M_IX86)
#define BLOCXXI_USE_EXECINFO 1
#endif

// ==== MINGW ===
#elif defined __MINGW32__ || defined __MINGW64__
#define BLOCXXI_MINGW
#define BLOCXXI_WINDOWS

// ==== WINDOWS ===
#elif defined _WIN32
#define BLOCXXI_WINDOWS

#endif  // Platform checks

// SSE is x86 / amd64 specific. On top of that, we only
// know how to access it on msvc and gcc (and gcc compatibles).
// GCC requires the user to enable SSE support in order for
// the program to have access to the intrinsics, this is
// indicated by the __SSE4_1__ macro
#ifndef BLOCXXI_HAS_SSE

#if (defined _M_AMD64 || defined _M_IX86 || defined _M_X64 ||   \
     defined __amd64__ || defined __i386 || defined __i386__ || \
     defined __x86_64__ || defined __x86_64) &&                 \
    (defined __GNUC__ || (defined _MSC_VER && _MSC_VER >= 1600))
#define BLOCXXI_HAS_SSE 1
#else
#define BLOCXXI_HAS_SSE 0
#endif

#endif  // BLOCXXI_HAS_SSE

#if (defined __arm__ || defined __aarch64__ || defined _M_ARM || \
     defined _M_ARM64)
#define BLOCXXI_HAS_ARM 1
#else
#define BLOCXXI_HAS_ARM 0
#endif  // BLOCXXI_HAS_ARM

#ifndef __has_builtin
#define __has_builtin(x) 0  // for non-clang compilers
#endif

#if (BLOCXXI_HAS_SSE && defined __GNUC__)
#define BLOCXXI_HAS_BUILTIN_CLZ 1
#elif (BLOCXXI_HAS_ARM && defined __GNUC__ && !defined __clang__)
#define BLOCXXI_HAS_BUILTIN_CLZ 1
#elif (defined __clang__ && __has_builtin(__builtin_clz))
#define BLOCXXI_HAS_BUILTIN_CLZ 1
#else
#define BLOCXXI_HAS_BUILTIN_CLZ 0
#endif  // BLOCXXI_HAS_BUILTIN_CLZ

#if (BLOCXXI_HAS_SSE && defined __GNUC__)
#define BLOCXXI_HAS_BUILTIN_CTZ 1
#elif (BLOCXXI_HAS_ARM && defined __GNUC__ && !defined __clang__)
#define BLOCXXI_HAS_BUILTIN_CTZ 1
#elif (defined __clang__ && __has_builtin(__builtin_ctz))
#define BLOCXXI_HAS_BUILTIN_CTZ 1
#else
#define BLOCXXI_HAS_BUILTIN_CTZ 0
#endif  // BLOCXXI_HAS_BUILTIN_CTZ

#if BLOCXXI_HAS_ARM && defined __ARM_NEON
#define BLOCXXI_HAS_ARM_NEON 1
#else
#define BLOCXXI_HAS_ARM_NEON 0
#endif  // BLOCXXI_HAS_ARM_NEON

#if BLOCXXI_HAS_ARM && defined __ARM_FEATURE_CRC32
#define BLOCXXI_HAS_ARM_CRC32 1
#else
#if defined BLOCXXI_FORCE_ARM_CRC32
#define BLOCXXI_HAS_ARM_CRC32 1
#else
#define BLOCXXI_HAS_ARM_CRC32 0
#endif
#endif  // BLOCXXI_HAS_ARM_CRC32

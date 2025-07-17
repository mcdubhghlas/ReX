/**************************************************************************/
/*  detect.h                                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             REDOT ENGINE                               */
/*                        https://redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2024-present Redot Engine contributors                   */
/*                                          (see REDOT_AUTHORS.md)        */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

/*
 * Provides a runtime way to detect if current CPU supports AVX/AVX2 SIMD
 * instruction sets.
 *
 * This will allow ReX to avoid crashing due to illegal instruction errors when
 * running on older harder as well.
 */

#include <cstdint>

#if defined(_MSC_VER)
    #include <intrin.h> // MSVC
#else
    #include <cpuid.h> // GCC/Clang
#endif

// SIMD support level
enum class SIMDLevel : int {
	NONE = 0,
	AVX = 1,
	AVX2 = 2
};

/*
 * Use XGETBV to check if OS has enabled saving/restoring YMM state. This is
 * required for using AVX instructions safely.
 */
inline uint64_t get_xcr0() {
	#if defined(_MSC_VER)
		return _xgetbv(0); // Reads XCR0 on Windows.
	#elif defined(__GNUC__) || defined(__clang__)
		uint32_t eax = 0;
		uint32_t edx = 0;

		__asm__ volatile (
			"xgetbv" // reads content of XCR0.
			: "=a"(eax), "=d"(edx) // maps EAX to eax, EDX to edx
			: "c"(0) // Set ECX to zero, specifies index of control reg to read
		);

		// Combine upper bits [edx] and lower bits [eax] 
		return ((uint64_t)edx << 32) | eax;
	#else
		return 0; // unknown platform.
	#endif
}

/*
 * Detect highest SIMD level supported at runtime.
 */
inline SIMDLevel detect_simd_level() {
	#if defined(__GNUC__) || defined(__clang__)
		// Thankfully, GCC/Clang have compiler builts-ins.
		if(__builtin_cpu_supports("avx2")) {
			return SIMDLevel::AVX2;
		} else if(__builtin_cpu_supports("avx")) {
			return SIMDLevel::AVX;
		} else {
			return SIMDLevel::NONE;
		}
	#elif defined(_MSC_VER)
		// TODO: implement this.
		return SIMDLevel::NONE; // default to None.
	#else
		// Fallback to unknown compiler/platform
		return SIMDLevel::NONE;
	#endif
}

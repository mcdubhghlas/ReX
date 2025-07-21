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
	#include <immintrin.h> // GCC/Clang
#endif

namespace simd {

// SIMD support level
enum class SIMDLevel : int {
	NONE = 0,
	AVX = 1,
	AVX2 = 2,
	AVX512 = 3 // TODO: implement AVX512 detection.
};


/*
 * Safe, unsigned bit check to obtain single feature flag.
 *
 * @param val - The value to test.
 *
 * @param bit - The bit index to check.
 *
 * @return - True, if bit is set.
 *           False, if not.
 */
template <typename T>
constexpr bool get_bit(T val, unsigned int bit) {
	return (val & (1ull << bit)) != 0;
}


uint64_t get_xcr0();
SIMDLevel detect_simd_level();

} // namespace: simd

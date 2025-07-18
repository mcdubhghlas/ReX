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
 * Safe, unsigned bit check to obtain single feature flag.
 * (32bit variant.)
 *
 * @param val - The value to test.
 *
 * @param bit - The bit index to check.
 *
 * @return - True, if bit is set.
 *           False, if not.
 */
constexpr bool get_bit32(uint32_t val, int bit) {
	return (val & (1u << bit)) != 0;
}

/*
 * Safe, unsigned bit check to obtain single feature flag.
 * (64bit variant.)
 *
 * @param val - The value to test.
 *
 * @param bit - The bit index to check.
 *
 * @return - True, if bit is set.
 *           False, if not.
 */
constexpr bool get_bit64(uint64_t val, int bit) {
    return (val & (1ull << bit)) != 0;
}

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
		int registers[4] = {};

		// Call CPUID function 1 to check AVX / OSXSAVE.
		__cpuid(registers, 1);

		// Be not afraid of the bitshift.
		// used: https://www.felixcloutier.com/x86/cpuid && https://x86-cpuid.org/
		// because I am lazy and got tired of looking at intel's 2.5K+ page doc

		// [CPUID] BIT #27 | OSXSAVE 
		const bool has_xsave = get_bit32(registers[2], 27); 
		// [CPUID] BIT #28 | AVX  
		const bool has_avx = get_bit32(registers[2], 28);

		if (!has_xsave || !has_avx) {
			return SIMDLevel::NONE;
		}

		// Check OS has enabled saving of XMM/YMM registers
		const uint64_t xcr0 = get_xcr0();

		// [XCR0] BIT #1 | XSAVE support for MXCSR, XMM.
		const bool has_xmm = get_bit64(xcr0, 1);
		// [XCR0] BIT #2 | AVX enabled, XSAVE for (upper-half) YMM.
		const bool has_ymm = get_bit64(xcr0, 2);

		if (!has_xmm || !has_ymm) {
			return SIMDLevel::NONE;
		}

		// Check AVX2 using CPUID function 7 (feature flag), subfunction 0
		// EAX = 7, ECX = 0
		// used: https://sandpile.org/x86/cpuid.htm
		__cpuidex(registers, 7, 0);

		// EBX Bit #5 is AVX2
		const bool has_avx2 = get_bit32(registers[1], 5);

		// This seemed cleaner than an if-statement.
		return has_avx2 ? SIMDLevel::AVX2 : SIMDLevel::AVX;
	#else
		// Fallback to unknown compiler/platform
		return SIMDLevel::NONE;
	#endif
}

/**************************************************************************/
/*  vector2i.h                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             REDOT ENGINE                               */
/*                        https://redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2024-present Redot Engine contributors                   */
/*                                          (see REDOT_AUTHORS.md)        */
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
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

#include "core/error/error_macros.h"
#include "core/math/math_funcs.h"

class String;
struct Vector2;

struct [[nodiscard]] Vector2i {
	static const int AXIS_COUNT = 2;

	enum Axis {
		AXIS_X,
		AXIS_Y,
	};

	union {
		// NOLINTBEGIN(modernize-use-default-member-init)
		struct {
			int32_t x;
			int32_t y;
		};

		struct {
			int32_t width;
			int32_t height;
		};

		int32_t coord[2] = { 0 };
		// NOLINTEND(modernize-use-default-member-init)
	};

	double length() const;
	int64_t length_squared() const;
	// TODO limit_length()

	// TODO zero()
	Vector2i sign() const;
	Vector2i abs() const;


	double distance_to(const Vector2i &p_to) const;
	int64_t distance_squared_to(const Vector2i &p_to) const;


	_FORCE_INLINE_ Vector2i::Axis min_axis_index() const;
	_FORCE_INLINE_ Vector2i::Axis max_axis_index() const;

	Vector2i min(const Vector2i &p_vector2i) const;
	Vector2i mini(int32_t p_scalar) const;
	Vector2i max(const Vector2i &p_vector2i) const;
	Vector2i maxi(int32_t p_scalar) const;
	Vector2i clamp(const Vector2i &p_min, const Vector2i &p_max) const;
	Vector2i clampi(int32_t p_min, int32_t p_max) const;
	// TODO snap()
	// TODO snapi()
	Vector2i snapped(const Vector2i &p_step) const;
	Vector2i snappedi(int32_t p_step) const;

	real_t aspect() const;


	_FORCE_INLINE_ int32_t &operator[](int p_axis);
	_FORCE_INLINE_ const int32_t &operator[](int p_axis) const;

	// Vectors
	constexpr Vector2i operator+(const Vector2i &p_v) const;
	constexpr void operator+=(const Vector2i &p_v);

	constexpr Vector2i operator-(const Vector2i &p_v) const;
	constexpr void operator-=(const Vector2i &p_v);

	constexpr Vector2i operator*(const Vector2i &p_v1) const;
	// TODO operator*=()

	constexpr Vector2i operator/(const Vector2i &p_v1) const;
	// TODO operator/=()

	constexpr Vector2i operator%(const Vector2i &p_v1) const;
	// TODO operator%=()

	// Scalars
	constexpr Vector2i operator*(int32_t p_rvalue) const;
	constexpr void operator*=(int32_t p_rvalue);

	constexpr Vector2i operator/(int32_t p_rvalue) const;
	constexpr void operator/=(int32_t p_rvalue);

	constexpr Vector2i operator%(int32_t p_rvalue) const;
	constexpr void operator%=(int32_t p_rvalue);

	constexpr Vector2i operator-() const;

	constexpr bool operator==(const Vector2i &p_vec2) const;
	constexpr bool operator!=(const Vector2i &p_vec2) const;
	constexpr bool operator<(const Vector2i &p_vec2) const;
	constexpr bool operator>(const Vector2i &p_vec2) const;
	constexpr bool operator<=(const Vector2i &p_vec2) const;
	constexpr bool operator>=(const Vector2i &p_vec2) const;

	operator String() const;
	operator Vector2() const;

	// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
	constexpr Vector2i() :
			x(0), y(0) {}
	constexpr Vector2i(int32_t p_x, int32_t p_y) :
			x(p_x), y(p_y) {}
	// NOLINTEND(cppcoreguidelines-pro-type-member-init)
};


// Multiplication operators required to workaround issues with LLVM using implicit conversion.
constexpr Vector2i operator*(int32_t p_scalar, const Vector2i &p_vector);
constexpr Vector2i operator*(int64_t p_scalar, const Vector2i &p_vector);
constexpr Vector2i operator*(float p_scalar, const Vector2i &p_vector);
constexpr Vector2i operator*(double p_scalar, const Vector2i &p_vector);

using Size2i = Vector2i;
using Point2i = Vector2i;

template <>
struct is_zero_constructible<Vector2i> : std::true_type {};

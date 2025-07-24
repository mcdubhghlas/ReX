/**************************************************************************/
/*  vector3i.h                                                            */
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
struct Vector3;

struct [[nodiscard]] Vector3i {
	static const int AXIS_COUNT = 3;

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {
		// NOLINTBEGIN(modernize-use-default-member-init)
		struct {
			int32_t x;
			int32_t y;
			int32_t z;
		};

		int32_t coord[3] = { 0 };
		// NOLINTEND(modernize-use-default-member-init)
	};

	_FORCE_INLINE_ double length() const;
	_FORCE_INLINE_ int64_t length_squared() const;
	// TODO limit_length()

	_FORCE_INLINE_ void zero();
	_FORCE_INLINE_ Vector3i sign() const;
	_FORCE_INLINE_ Vector3i abs() const;

	_FORCE_INLINE_ double distance_to(const Vector3i &p_to) const;
	_FORCE_INLINE_ int64_t distance_squared_to(const Vector3i &p_to) const;

	Vector3i::Axis min_axis_index() const;
	Vector3i::Axis max_axis_index() const;

	Vector3i min(const Vector3i &p_vector3i) const;
	Vector3i mini(int32_t p_scalar) const;
	Vector3i max(const Vector3i &p_vector3i) const;
	Vector3i maxi(int32_t p_scalar) const;
	Vector3i clamp(const Vector3i &p_min, const Vector3i &p_max) const;
	Vector3i clampi(int32_t p_min, int32_t p_max) const;
	// TODO snap
	// TODO snapi
	Vector3i snapped(const Vector3i &p_step) const;
	Vector3i snappedi(int32_t p_step) const;


	/* Operators */
	_FORCE_INLINE_ const int32_t &operator[](int p_axis) const;
	_FORCE_INLINE_ int32_t &operator[](int p_axis);

	constexpr Vector3i operator+(const Vector3i &p_v) const;
	constexpr Vector3i &operator+=(const Vector3i &p_v);

	constexpr Vector3i operator-(const Vector3i &p_v) const;
	constexpr Vector3i &operator-=(const Vector3i &p_v);

	constexpr Vector3i operator*(const Vector3i &p_v) const;
	constexpr Vector3i &operator*=(const Vector3i &p_v);

	constexpr Vector3i operator/(const Vector3i &p_v) const;
	constexpr Vector3i &operator/=(const Vector3i &p_v);

	constexpr Vector3i operator%(const Vector3i &p_v) const;
	constexpr Vector3i &operator%=(const Vector3i &p_v);

	constexpr Vector3i operator*(int32_t p_scalar) const;
	constexpr Vector3i &operator*=(int32_t p_scalar);

	constexpr Vector3i operator/(int32_t p_scalar) const;
	constexpr Vector3i &operator/=(int32_t p_scalar);

	constexpr Vector3i operator%(int32_t p_scalar) const;
	constexpr Vector3i &operator%=(int32_t p_scalar);

	constexpr Vector3i operator-() const;

	constexpr bool operator==(const Vector3i &p_v) const;
	constexpr bool operator!=(const Vector3i &p_v) const;
	constexpr bool operator<(const Vector3i &p_v) const;
	constexpr bool operator<=(const Vector3i &p_v) const;
	constexpr bool operator>(const Vector3i &p_v) const;
	constexpr bool operator>=(const Vector3i &p_v) const;

	operator String() const;
	operator Vector3() const;

	constexpr Vector3i() :
			x(0), y(0), z(0) {}
	constexpr Vector3i(int32_t p_x, int32_t p_y, int32_t p_z) :
			x(p_x), y(p_y), z(p_z) {}
};


// Multiplication operators required to workaround issues with LLVM using implicit conversion.
constexpr Vector3i operator*(int32_t p_scalar, const Vector3i &p_vector);
constexpr Vector3i operator*(int64_t p_scalar, const Vector3i &p_vector);
constexpr Vector3i operator*(float p_scalar, const Vector3i &p_vector);
constexpr Vector3i operator*(double p_scalar, const Vector3i &p_vector);

template <>
struct is_zero_constructible<Vector3i> : std::true_type {};

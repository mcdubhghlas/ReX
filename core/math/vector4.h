/**************************************************************************/
/*  vector4.h                                                             */
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
#include "core/math/math_defs.h"
#include "core/typedefs.h"

class String;
struct Vector4i;

struct [[nodiscard]] Vector4 {
	static const int AXIS_COUNT = 4;

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_W,
	};

	union {
		// NOLINTBEGIN(modernize-use-default-member-init)
		struct {
			real_t x;
			real_t y;
			real_t z;
			real_t w;
		};
		real_t coord[4] = { 0, 0, 0, 0 };
		// NOLINTEND(modernize-use-default-member-init)
	};

	real_t length() const;
	_FORCE_INLINE_ real_t length_squared() const;
	// TODO limit_length()

	// TODO zero()
	Vector4 sign() const;
	Vector4 abs() const;
	Vector4 floor() const;
	Vector4 ceil() const;
	Vector4 round() const;
	bool is_finite() const;

	void normalize();
	Vector4 normalized() const;
	bool is_normalized() const;

	// These are probably not included for a reason
	// Leaving up to Mcdub's discretion
	// TODO angle()
	// TODO rotate()
	// TODO rotated()
	// TODO angle_to()
	// TODO signed_angle_to()
	// TODO angle_to_point()
	// TODO from_angle()

	real_t distance_to(const Vector4 &p_to) const;
	real_t distance_squared_to(const Vector4 &p_to) const;
	Vector4 direction_to(const Vector4 &p_to) const;


	_FORCE_INLINE_ real_t dot(const Vector4 &p_vec4) const;
	// TODO cross()

	Vector4 inverse() const;
	Vector4::Axis min_axis_index() const;
	Vector4::Axis max_axis_index() const;


	Vector4 lerp(const Vector4 &p_to, real_t p_weight) const;
	// TODO slerp
	Vector4 cubic_interpolate(const Vector4 &p_b, const Vector4 &p_pre_a, const Vector4 &p_post_b, real_t p_weight) const;
	Vector4 cubic_interpolate_in_time(const Vector4 &p_b, const Vector4 &p_pre_a, const Vector4 &p_post_b, real_t p_weight, real_t p_b_t, real_t p_pre_a_t, real_t p_post_b_t) const;

	Vector4 posmod(real_t p_mod) const;
	Vector4 posmodv(const Vector4 &p_modv) const;

	Vector4 min(const Vector4 &p_vector4) const;
	Vector4 minf(real_t p_scalar) const;
	Vector4 max(const Vector4 &p_vector4) const;
	Vector4 maxf(real_t p_scalar) const;
	Vector4 clamp(const Vector4 &p_min, const Vector4 &p_max) const;
	Vector4 clampf(real_t p_min, real_t p_max) const;
	void snap(const Vector4 &p_step);
	void snapf(real_t p_step);
	Vector4 snapped(const Vector4 &p_step) const;
	Vector4 snappedf(real_t p_step) const;

	bool is_equal_approx(const Vector4 &p_vec4) const;
	bool is_zero_approx() const;
	bool is_same(const Vector4 &p_vec4) const;

	_FORCE_INLINE_ real_t &operator[](int p_axis);
	_FORCE_INLINE_ const real_t &operator[](int p_axis) const;

	constexpr Vector4 operator+(const Vector4 &p_vec4) const;
	constexpr void operator+=(const Vector4 &p_vec4);

	constexpr Vector4 operator-(const Vector4 &p_vec4) const;
	constexpr void operator-=(const Vector4 &p_vec4);

	constexpr Vector4 operator*(const Vector4 &p_vec4) const;
	constexpr void operator*=(const Vector4 &p_vec4);

	constexpr Vector4 operator/(const Vector4 &p_vec4) const;
	constexpr void operator/=(const Vector4 &p_vec4);

	constexpr Vector4 operator*(real_t p_s) const;
	constexpr void operator*=(real_t p_s);

	constexpr Vector4 operator/(real_t p_s) const;
	constexpr void operator/=(real_t p_s);


	constexpr Vector4 operator-() const;

	constexpr bool operator==(const Vector4 &p_vec4) const;
	constexpr bool operator!=(const Vector4 &p_vec4) const;
	constexpr bool operator>(const Vector4 &p_vec4) const;
	constexpr bool operator<(const Vector4 &p_vec4) const;
	constexpr bool operator>=(const Vector4 &p_vec4) const;
	constexpr bool operator<=(const Vector4 &p_vec4) const;

	operator String() const;
	operator Vector4i() const;

	constexpr Vector4() :
			x(0), y(0), z(0), w(0) {}
	constexpr Vector4(real_t p_x, real_t p_y, real_t p_z, real_t p_w) :
			x(p_x), y(p_y), z(p_z), w(p_w) {}
};

constexpr Vector4 operator*(float p_scalar, const Vector4 &p_vec);
constexpr Vector4 operator*(double p_scalar, const Vector4 &p_vec);
constexpr Vector4 operator*(int32_t p_scalar, const Vector4 &p_vec);
constexpr Vector4 operator*(int64_t p_scalar, const Vector4 &p_vec);

template <>
struct is_zero_constructible<Vector4> : std::true_type {};

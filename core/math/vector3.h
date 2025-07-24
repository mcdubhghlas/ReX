/**************************************************************************/
/*  vector3.h                                                             */
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
#include "core/string/ustring.h"

struct Basis;
struct Vector2;
struct Vector3i;

struct [[nodiscard]] Vector3 {
	static const int AXIS_COUNT = 3;

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
	};

	union {
		// NOLINTBEGIN(modernize-use-default-member-init)
		struct {
			real_t x;
			real_t y;
			real_t z;
		};

		real_t coord[3] = { 0 };
		// NOLINTEND(modernize-use-default-member-init)
	};

	_FORCE_INLINE_ real_t length() const;
	_FORCE_INLINE_ real_t length_squared() const;
	Vector3 limit_length(real_t p_len = 1.0) const;

	_FORCE_INLINE_ void zero();
	_FORCE_INLINE_ Vector3 sign() const;
	_FORCE_INLINE_ Vector3 abs() const;
	_FORCE_INLINE_ Vector3 floor() const;
	_FORCE_INLINE_ Vector3 ceil() const;
	_FORCE_INLINE_ Vector3 round() const;
	bool is_finite() const;

	_FORCE_INLINE_ void normalize();
	_FORCE_INLINE_ Vector3 normalized() const;
	_FORCE_INLINE_ bool is_normalized() const;

	// TODO angle()
	void rotate(const Vector3 &p_axis, real_t p_angle);
	Vector3 rotated(const Vector3 &p_axis, real_t p_angle) const;
	_FORCE_INLINE_ real_t angle_to(const Vector3 &p_to) const;
	_FORCE_INLINE_ real_t signed_angle_to(const Vector3 &p_to, const Vector3 &p_axis) const;
	// TODO angle_to_point()
	// TODO from_angle()

	_FORCE_INLINE_ real_t distance_to(const Vector3 &p_to) const;
	_FORCE_INLINE_ real_t distance_squared_to(const Vector3 &p_to) const;
	_FORCE_INLINE_ Vector3 direction_to(const Vector3 &p_to) const;

	_FORCE_INLINE_ Vector3 project(const Vector3 &p_to) const;
	// TODO project_plane()

	_FORCE_INLINE_ real_t dot(const Vector3 &p_with) const;
	_FORCE_INLINE_ Vector3 cross(const Vector3 &p_with) const;
	Basis outer(const Vector3 &p_with) const;
	_FORCE_INLINE_ Vector3 get_any_perpendicular() const;
	// TODO orthogonal


	_FORCE_INLINE_ Vector3 inverse() const;
	_FORCE_INLINE_ Vector3::Axis min_axis_index() const;
	_FORCE_INLINE_ Vector3::Axis max_axis_index() const;

	_FORCE_INLINE_ Vector3 lerp(const Vector3 &p_to, real_t p_weight) const;
	_FORCE_INLINE_ Vector3 slerp(const Vector3 &p_to, real_t p_weight) const;
	_FORCE_INLINE_ Vector3 cubic_interpolate(const Vector3 &p_b, const Vector3 &p_pre_a, const Vector3 &p_post_b, real_t p_weight) const;
	_FORCE_INLINE_ Vector3 cubic_interpolate_in_time(const Vector3 &p_b, const Vector3 &p_pre_a, const Vector3 &p_post_b, real_t p_weight, real_t p_b_t, real_t p_pre_a_t, real_t p_post_b_t) const;
	_FORCE_INLINE_ Vector3 bezier_interpolate(const Vector3 &p_control_1, const Vector3 &p_control_2, const Vector3 &p_end, real_t p_t) const;
	_FORCE_INLINE_ Vector3 bezier_derivative(const Vector3 &p_control_1, const Vector3 &p_control_2, const Vector3 &p_end, real_t p_t) const;

	_FORCE_INLINE_ Vector3 slide(const Vector3 &p_normal) const;
	_FORCE_INLINE_ Vector3 bounce(const Vector3 &p_normal) const;
	_FORCE_INLINE_ Vector3 reflect(const Vector3 &p_normal) const;
	Vector3 move_toward(const Vector3 &p_to, real_t p_delta) const;

	_FORCE_INLINE_ Vector3 posmod(real_t p_mod) const;
	_FORCE_INLINE_ Vector3 posmodv(const Vector3 &p_modv) const;

	Vector3 min(const Vector3 &p_vector3) const;
	Vector3 minf(real_t p_scalar) const;
	Vector3 max(const Vector3 &p_vector3) const;
	Vector3 maxf(real_t p_scalar) const;
	Vector3 clamp(const Vector3 &p_min, const Vector3 &p_max) const;
	Vector3 clampf(real_t p_min, real_t p_max) const;
	void snap(const Vector3 &p_step);
	void snapf(real_t p_step);
	Vector3 snapped(const Vector3 &p_step) const;
	Vector3 snappedf(real_t p_step) const;

	Vector2 octahedron_encode() const;
	static Vector3 octahedron_decode(const Vector2 &p_oct);
	Vector2 octahedron_tangent_encode(float p_sign) const;
	static Vector3 octahedron_tangent_decode(const Vector2 &p_oct, float *r_sign);

	bool is_equal_approx(const Vector3 &p_v) const;
	bool is_same(const Vector3 &p_v) const;
	bool is_zero_approx() const;

	/* Operators */

	_FORCE_INLINE_ real_t &operator[](int p_axis);
	_FORCE_INLINE_ const real_t &operator[](int p_axis) const;

	constexpr Vector3 operator+(const Vector3 &p_v) const;
	constexpr Vector3 &operator+=(const Vector3 &p_v);

	constexpr Vector3 operator-(const Vector3 &p_v) const;
	constexpr Vector3 &operator-=(const Vector3 &p_v);

	constexpr Vector3 operator*(const Vector3 &p_v) const;
	constexpr Vector3 &operator*=(const Vector3 &p_v);

	constexpr Vector3 operator/(const Vector3 &p_v) const;
	constexpr Vector3 &operator/=(const Vector3 &p_v);

	constexpr Vector3 operator*(real_t p_scalar) const;
	constexpr Vector3 &operator*=(real_t p_scalar);

	constexpr Vector3 operator/(real_t p_scalar) const;
	constexpr Vector3 &operator/=(real_t p_scalar);

	constexpr Vector3 operator-() const;

	constexpr bool operator==(const Vector3 &p_v) const;
	constexpr bool operator!=(const Vector3 &p_v) const;
	constexpr bool operator<(const Vector3 &p_v) const;
	constexpr bool operator<=(const Vector3 &p_v) const;
	constexpr bool operator>(const Vector3 &p_v) const;
	constexpr bool operator>=(const Vector3 &p_v) const;

	operator String() const;
	operator Vector3i() const;

	constexpr Vector3() :
			x(0), y(0), z(0) {}
	constexpr Vector3(real_t p_x, real_t p_y, real_t p_z) :
			x(p_x), y(p_y), z(p_z) {}
};

// Multiplication operators required to workaround issues with LLVM using implicit conversion
// to Vector3i instead for integers where it should not.
constexpr Vector3 operator*(float p_scalar, const Vector3 &p_vec);
constexpr Vector3 operator*(double p_scalar, const Vector3 &p_vec);
constexpr Vector3 operator*(int32_t p_scalar, const Vector3 &p_vec);
constexpr Vector3 operator*(int64_t p_scalar, const Vector3 &p_vec);

_FORCE_INLINE_ real_t vec3_dot(const Vector3 &p_a, const Vector3 &p_b);
_FORCE_INLINE_ Vector3 vec3_cross(const Vector3 &p_a, const Vector3 &p_b);

template <>
struct is_zero_constructible<Vector3> : std::true_type {};

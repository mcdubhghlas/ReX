/**************************************************************************/
/*  vector2i.cpp                                                          */
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

#include "vector2i.h"

#include "core/math/vector2.h"
#include "core/string/ustring.h"


double Vector2i::length() const {
	return Math::sqrt((double)length_squared());
}

int64_t Vector2i::length_squared() const {
	return x * (int64_t)x + y * (int64_t)y;
}
Vector2i Vector2i::sign() const { return Vector2i(SIGN(x), SIGN(y)); }
Vector2i Vector2i::abs() const { return Vector2i(Math::abs(x), Math::abs(y)); }

double Vector2i::distance_to(const Vector2i &p_to) const {
	return (p_to - *this).length();
}

int64_t Vector2i::distance_squared_to(const Vector2i &p_to) const {
	return (p_to - *this).length_squared();
}




Vector2i Vector2i::min(const Vector2i &p_vector2i) const {
	return Vector2i(MIN(x, p_vector2i.x), MIN(y, p_vector2i.y));
}

Vector2i Vector2i::mini(int32_t p_scalar) const {
	return Vector2i(MIN(x, p_scalar), MIN(y, p_scalar));
}

Vector2i Vector2i::max(const Vector2i &p_vector2i) const {
	return Vector2i(MAX(x, p_vector2i.x), MAX(y, p_vector2i.y));
}

Vector2i Vector2i::maxi(int32_t p_scalar) const {
	return Vector2i(MAX(x, p_scalar), MAX(y, p_scalar));
}

Vector2i Vector2i::clamp(const Vector2i &p_min, const Vector2i &p_max) const {
	return Vector2i(
			CLAMP(x, p_min.x, p_max.x),
			CLAMP(y, p_min.y, p_max.y));
}

Vector2i Vector2i::clampi(int32_t p_min, int32_t p_max) const {
	return Vector2i(
			CLAMP(x, p_min, p_max),
			CLAMP(y, p_min, p_max));
}

Vector2i Vector2i::snapped(const Vector2i &p_step) const {
	return Vector2i(
			Math::snapped(x, p_step.x),
			Math::snapped(y, p_step.y));
}

Vector2i Vector2i::snappedi(int32_t p_step) const {
	return Vector2i(
			Math::snapped(x, p_step),
			Math::snapped(y, p_step));
}

real_t Vector2i::aspect() const { return width / (real_t)height; }




Vector2i::operator String() const {
	return "(" + itos(x) + ", " + itos(y) + ")";
}

Vector2i::operator Vector2() const {
	return Vector2((int32_t)x, (int32_t)y);
}




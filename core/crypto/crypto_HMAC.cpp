/**************************************************************************/
/*  crypto_HMAC.cpp                                                       */
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

#include "crypto.h"

void HMACContext::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start", "hash_type", "key"), &HMACContext::start);
	ClassDB::bind_method(D_METHOD("update", "data"), &HMACContext::update);
	ClassDB::bind_method(D_METHOD("finish"), &HMACContext::finish);
}

/*
 * Creates a new HMACContext instance.
 *
 * @param p_notify_postinitialize - Whether or not to notify after post-init.
 *
 * @return - A pointer to new HMACContext instance, if successful.
 *           nullptr, if not set.
 */
HMACContext *(*HMACContext::_create)(bool p_notify_postinitialize) = nullptr;
// TODO: When we fix types to allow for smart pointers, upgrade to Ref<HMACContext>
HMACContext *HMACContext::create(bool p_notify_postinitialize) {
	if (_create) {
		return _create(p_notify_postinitialize);
	} else {
		ERR_PRINT("HMACContext is not available when the mbedtls module is disabled.");
		return nullptr; //Ref<HMACContext>();
	}
}

/*
 * Computes HMAC digest using given hash type, key, and data.
 *
 * @param p_hash_type - The hash algorithm to use.
 *
 * @param p_key - The secret key for HMAC.
 *
 * @param p_msg - The message to authenticate.
 *
 * @return - HMAC digest as PackedByteArray, if successful.
 *           empty PackedByteArray, if failed.
 */
PackedByteArray Crypto::hmac_digest(HashingContext::HashType p_hash_type, const PackedByteArray &p_key, const PackedByteArray &p_msg) {
	Ref<HMACContext> ctx = HMACContext::create();

	if (ctx.is_null()) {
		ERR_PRINT("HMAC is not available without mbedtls module.");
		return PackedByteArray();
	}

	Error err = ctx->start(p_hash_type, p_key);

	if (err != OK) {
		ERR_PRINT("HMAC context failed to start.");
		return PackedByteArray();
	}

	err = ctx->update(p_msg);

	if (err != OK) {
		ERR_PRINT("HMAC context failed to update.");
		return PackedByteArray();
	}

	return ctx->finish();
}

// Compares two HMACS for equality without leaking timing information in order to prevent timing attacks.
// @see: https://paragonie.com/blog/2015/11/preventing-timing-attacks-on-string-comparison-with-double-hmac-strategy
bool Crypto::constant_time_compare(const PackedByteArray &p_trusted, const PackedByteArray &p_received) {
	const uint8_t *t = p_trusted.ptr();
	const uint8_t *r = p_received.ptr();
	size_t tlen = p_trusted.size();
	size_t rlen = p_received.size();

	// If the lengths are different then nothing else matters.
	if (tlen != rlen) {
		return false;
	}

	// Check for nullptr.
	if (!t || !r) {
		return false;
	}

	uint8_t v = 0;

	for (size_t i = 0; i < tlen; i++) {
		v |= t[i] ^ r[i];
	}

	return v == 0;
}

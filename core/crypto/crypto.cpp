/**************************************************************************/
/*  crypto.cpp                                                            */
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

/// Crypto

/*
 * Load default cert from specified path.
 *
 * @param p_path - Path to directory or file containing default cert.
 */
void (*Crypto::_load_default_certificates)(const String &p_path) = nullptr;

/*
 * Creates new Crypto instance.
 *
 * @param p_notify_postinitialize - Whether or not to notify after post-init.
 *
 * @return - A pointer to new Crypto instance, if successful.
 *           nullptr, if not successful.
 */
Crypto *(*Crypto::_create)(bool p_notify_postinitialize) = nullptr;
Crypto *Crypto::create(bool p_notify_postinitialize) {
	if (_create) {
		return _create(p_notify_postinitialize);
	}
	ERR_FAIL_V_MSG(nullptr, "Crypto is not available when the mbedtls module is disabled.");
}

/*
 * Load default cert from specified path.
 *
 * @param p_path - The path to the default cert store.
 */
void Crypto::load_default_certificates(const String &p_path) {
	if (_load_default_certificates) {
		_load_default_certificates(p_path);
	}
}

void Crypto::_bind_methods() {
	ClassDB::bind_method(D_METHOD("generate_random_bytes", "size"), &Crypto::generate_random_bytes);
	ClassDB::bind_method(D_METHOD("generate_rsa", "size"), &Crypto::generate_rsa);
	ClassDB::bind_method(D_METHOD("generate_self_signed_certificate", "key", "issuer_name", "not_before", "not_after"), &Crypto::generate_self_signed_certificate, DEFVAL("CN=myserver,O=myorganisation,C=IT"), DEFVAL("20140101000000"), DEFVAL("20340101000000"));
	ClassDB::bind_method(D_METHOD("sign", "hash_type", "hash", "key"), &Crypto::sign);
	ClassDB::bind_method(D_METHOD("verify", "hash_type", "hash", "signature", "key"), &Crypto::verify);
	ClassDB::bind_method(D_METHOD("encrypt", "key", "plaintext"), &Crypto::encrypt);
	ClassDB::bind_method(D_METHOD("decrypt", "key", "ciphertext"), &Crypto::decrypt);
	ClassDB::bind_method(D_METHOD("hmac_digest", "hash_type", "key", "msg"), &Crypto::hmac_digest);
	ClassDB::bind_method(D_METHOD("constant_time_compare", "trusted", "received"), &Crypto::constant_time_compare);
}

/// Resource loader/saver

/*
 * Load a cryptographic resource.
 *
 * @param p_path - The path to the resource file.
 *
 * @param p_original_path - The original path before remapped (unused.)
 *
 * @param r_error - Pointer to store resulting err code (optional.)
 *
 * @param p_use_sub_threads - (unused.)
 *
 * @param r_progress - (unused pointer.)
 *
 * @param p_cache_mode - (unused.)
 *
 * @return - Ref<Resource> to the loaded cryptographic obj, if successful.
 *           nullptr, if failed.
 */
Ref<Resource> ResourceFormatLoaderCrypto::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	String el = p_path.get_extension().to_lower();
	if (el == "crt") {
		X509Certificate *cert = X509Certificate::create();
		if (cert) {
			cert->load(p_path);
		}
		return cert;
	} else if (el == "key") {
		CryptoKey *key = CryptoKey::create();
		if (key) {
			key->load(p_path, false);
		}
		return key;
	} else if (el == "pub") {
		CryptoKey *key = CryptoKey::create();
		if (key) {
			key->load(p_path, true);
		}
		return key;
	}
	return nullptr;
}

/*
 * Return list of file extensions recognized by this loader.
 *
 * @param p_extensions - Pointer to list to populate with supported extensions.
 */
void ResourceFormatLoaderCrypto::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("crt");
	p_extensions->push_back("key");
	p_extensions->push_back("pub");
}

/*
 * Check if loader can handle given resource type.
 *
 * @param p_type - Resource type to check.
 *
 * @return - True, if supported.
 *           False, otherwise.
 */
bool ResourceFormatLoaderCrypto::handles_type(const String &p_type) const {
	return p_type == "X509Certificate" || p_type == "CryptoKey";
}

/*
 * Determine resource type based on file extension.
 *
 * @param p_path - file path to eval.
 *
 * @return - Resource type as String, if successful.
 *           Empty String, if unknown.
 */
String ResourceFormatLoaderCrypto::get_resource_type(const String &p_path) const {
	String el = p_path.get_extension().to_lower();
	if (el == "crt") {
		return "X509Certificate";
	} else if (el == "key" || el == "pub") {
		return "CryptoKey";
	}
	return "";
}

/*
 * Save cryptographic resource to given file path.
 *
 * @param p_resource - The cryptographic resource to save.
 *
 * @param p_path - The target file path.
 *
 * @param p_flags - (Unused.)
 *
 * @return - OK, on success.
 *           Appropriate Error code, on failure.
 */
Error ResourceFormatSaverCrypto::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Error err;
	Ref<X509Certificate> cert = p_resource;
	Ref<CryptoKey> key = p_resource;
	if (cert.is_valid()) {
		err = cert->save(p_path);
	} else if (key.is_valid()) {
		String el = p_path.get_extension().to_lower();
		err = key->save(p_path, el == "pub");
	} else {
		ERR_FAIL_V(ERR_INVALID_PARAMETER);
	}
	ERR_FAIL_COND_V_MSG(err != OK, err, vformat("Cannot save Crypto resource to file '%s'.", p_path));
	return OK;
}

/*
 * Return list of file extensions applicable for given crypto resource.
 *
 * @param p_resource - The resource to eval.
 *
 * @param p_extensions - Pointer to list to populate with valid extensions.
 */
void ResourceFormatSaverCrypto::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	const X509Certificate *cert = Object::cast_to<X509Certificate>(*p_resource);
	const CryptoKey *key = Object::cast_to<CryptoKey>(*p_resource);
	if (cert) {
		p_extensions->push_back("crt");
	}
	if (key) {
		if (!key->is_public_only()) {
			p_extensions->push_back("key");
		}
		p_extensions->push_back("pub");
	}
}

/*
 * Check if given resource is supported.
 *
 * @param p_resource - Resource to check.
 *
 * @return - True, if supported.
 *           False, otherwise.
 */
bool ResourceFormatSaverCrypto::recognize(const Ref<Resource> &p_resource) const {
	return Object::cast_to<X509Certificate>(*p_resource) || Object::cast_to<CryptoKey>(*p_resource);
}

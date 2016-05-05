/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under Microsoft Reference Source License (Ms-RSL)
 * see LICENSE.md file for details
 */

#include "py/objstr.h"

#include "trezor-crypto/sha2.h"

#define HASH_SHA256_BLOCK_SIZE   64
#define HASH_SHA256_DIGEST_SIZE  32

typedef struct _mp_obj_Sha256_t {
    mp_obj_base_t base;
    SHA256_CTX ctx;
} mp_obj_Sha256_t;

STATIC mp_obj_t mod_TrezorCrypto_Sha256_update(mp_obj_t self, mp_obj_t data);

/// def trezor.crypto.hashlib.sha256(self, data: bytes=None) -> Sha256
///
/// Creates a hash context object.
///
STATIC mp_obj_t mod_TrezorCrypto_Sha256_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_Sha256_t *o = m_new_obj(mp_obj_Sha256_t);
    o->base.type = type;
    sha256_Init(&(o->ctx));
    // constructor called with bytes/str as first parameter
    if (n_args == 1) {
        mod_TrezorCrypto_Sha256_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

/// def Sha256.update(self, data: bytes) -> None
///
/// Update the hash context with hashed data.
///
STATIC mp_obj_t mod_TrezorCrypto_Sha256_update(mp_obj_t self, mp_obj_t data) {
    mp_obj_Sha256_t *o = MP_OBJ_TO_PTR(self);
    mp_buffer_info_t msg;
    mp_get_buffer_raise(data, &msg, MP_BUFFER_READ);
    sha256_Update(&(o->ctx), msg.buf, msg.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_TrezorCrypto_Sha256_update_obj, mod_TrezorCrypto_Sha256_update);

/// def Sha256.digest(self) -> bytes
///
/// Returns the digest of hashed data.
///
STATIC mp_obj_t mod_TrezorCrypto_Sha256_digest(mp_obj_t self) {
    mp_obj_Sha256_t *o = MP_OBJ_TO_PTR(self);
    vstr_t vstr;
    vstr_init_len(&vstr, HASH_SHA256_DIGEST_SIZE);
    SHA256_CTX ctx;
    memcpy(&ctx, &(o->ctx), sizeof(SHA256_CTX));
    sha256_Final(&ctx, (uint8_t *)vstr.buf);
    memset(&ctx, 0, sizeof(SHA256_CTX));
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Sha256_digest_obj, mod_TrezorCrypto_Sha256_digest);

STATIC mp_obj_t mod_TrezorCrypto_Sha256___del__(mp_obj_t self) {
    mp_obj_Sha256_t *o = MP_OBJ_TO_PTR(self);
    memset(&(o->ctx), 0, sizeof(SHA256_CTX));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Sha256___del___obj, mod_TrezorCrypto_Sha256___del__);

STATIC const mp_rom_map_elem_t mod_TrezorCrypto_Sha256_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&mod_TrezorCrypto_Sha256_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&mod_TrezorCrypto_Sha256_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_TrezorCrypto_Sha256___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_OBJ_NEW_SMALL_INT(HASH_SHA256_BLOCK_SIZE) },
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_OBJ_NEW_SMALL_INT(HASH_SHA256_DIGEST_SIZE) },
};
STATIC MP_DEFINE_CONST_DICT(mod_TrezorCrypto_Sha256_locals_dict, mod_TrezorCrypto_Sha256_locals_dict_table);

STATIC const mp_obj_type_t mod_TrezorCrypto_Sha256_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sha256,
    .make_new = mod_TrezorCrypto_Sha256_make_new,
    .locals_dict = (void*)&mod_TrezorCrypto_Sha256_locals_dict,
};

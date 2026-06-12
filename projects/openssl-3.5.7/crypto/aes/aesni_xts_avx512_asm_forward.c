#include "internal/deprecated.h"

#include <assert.h>

#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/aes.h>
#include "crypto/modes.h"
#include "crypto/aes_platform.h"
#include <stdfil.h>

int aesni_xts_avx512_eligible(void)
{
    return zunsafe_fast_call("aesni_xts_avx512_eligible");
}

void aesni_xts_128_encrypt_avx512(const unsigned char *in, unsigned char *out,
                                  size_t len, const AES_KEY *key1,
                                  const AES_KEY *key2,
                                  const unsigned char iv[16])
{
    zcheck_readonly(in, len);
    zcheck(out, len);
    zcheck_readonly(key1, sizeof(AES_KEY));
    zcheck_readonly(key2, sizeof(AES_KEY));
    zcheck_readonly(iv, 16);
    zunsafe_buf_call(len, "aesni_xts_128_encrypt_avx512", in, out, len, key1, key2, iv);
}

void aesni_xts_256_encrypt_avx512(const unsigned char *in, unsigned char *out,
                                  size_t len, const AES_KEY *key1,
                                  const AES_KEY *key2,
                                  const unsigned char iv[16])
{
    zcheck_readonly(in, len);
    zcheck(out, len);
    zcheck_readonly(key1, sizeof(AES_KEY));
    zcheck_readonly(key2, sizeof(AES_KEY));
    zcheck_readonly(iv, 16);
    zunsafe_buf_call(len, "aesni_xts_256_encrypt_avx512", in, out, len, key1, key2, iv);
}

void aesni_xts_128_decrypt_avx512(const unsigned char *in, unsigned char *out,
                                  size_t len, const AES_KEY *key1,
                                  const AES_KEY *key2,
                                  const unsigned char iv[16])
{
    zcheck_readonly(in, len);
    zcheck(out, len);
    zcheck_readonly(key1, sizeof(AES_KEY));
    zcheck_readonly(key2, sizeof(AES_KEY));
    zcheck_readonly(iv, 16);
    zunsafe_buf_call(len, "aesni_xts_128_decrypt_avx512", in, out, len, key1, key2, iv);
}

void aesni_xts_256_decrypt_avx512(const unsigned char *in, unsigned char *out,
                                  size_t len, const AES_KEY *key1,
                                  const AES_KEY *key2,
                                  const unsigned char iv[16])
{
    zcheck_readonly(in, len);
    zcheck(out, len);
    zcheck_readonly(key1, sizeof(AES_KEY));
    zcheck_readonly(key2, sizeof(AES_KEY));
    zcheck_readonly(iv, 16);
    zunsafe_buf_call(len, "aesni_xts_256_decrypt_avx512", in, out, len, key1, key2, iv);
}

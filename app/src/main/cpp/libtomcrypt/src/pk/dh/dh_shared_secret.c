/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#include "tomcrypt_private.h"

#ifdef LTC_MDH

/**
   Create a DH shared secret.
   @param private_key     The private DH key in the pair
   @param public_key      The public DH key in the pair
   @param out             [out] The destination of the shared data
   @param outlen          [in/out] The max size and resulting size of the shared data.
   @return CRYPT_OK if successful
*/
int dh_shared_secret(const dh_key *private_key, const dh_key *public_key,
                     unsigned char *out, unsigned long *outlen)
{
   void *tmp;
   unsigned long x;
   int err;

   LTC_ARGCHK(private_key != NULL);
   LTC_ARGCHK(public_key  != NULL);
   LTC_ARGCHK(out         != NULL);
   LTC_ARGCHK(outlen      != NULL);

   /* types valid? */
   if (private_key->type != PK_PRIVATE) {
      return CRYPT_PK_NOT_PRIVATE;
   }

   /* same DH group? */
   if (ltc_mp_cmp(private_key->prime, public_key->prime) != LTC_MP_EQ) { return CRYPT_PK_TYPE_MISMATCH; }
   if (ltc_mp_cmp(private_key->base, public_key->base) != LTC_MP_EQ)   { return CRYPT_PK_TYPE_MISMATCH; }

   /* init big numbers */
   if ((err = ltc_mp_init(&tmp)) != CRYPT_OK) {
      return err;
   }

   /* check public key */
   if ((err = dh_check_pubkey(public_key)) != CRYPT_OK) {
      goto error;
   }

   /* compute tmp = y^x mod p */
   if ((err = ltc_mp_exptmod(public_key->y, private_key->x, private_key->prime, tmp)) != CRYPT_OK)  {
      goto error;
   }

   /* enough space for output? */
   x = (unsigned long)ltc_mp_unsigned_bin_size(tmp);
   if (*outlen < x) {
      *outlen = x;
      err = CRYPT_BUFFER_OVERFLOW;
      goto error;
   }
   if ((err = ltc_mp_to_unsigned_bin(tmp, out)) != CRYPT_OK) {
      goto error;
   }
   *outlen = x;
   err = CRYPT_OK;

error:
   ltc_mp_clear(tmp);
   return err;
}

#endif /* LTC_MDH */

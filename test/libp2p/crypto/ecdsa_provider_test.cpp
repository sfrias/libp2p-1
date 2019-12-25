/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libp2p/crypto/ecdsa_provider/ecdsa_provider_impl.hpp"

#include <gtest/gtest.h>
#include <gsl/gsl_util>
#include "testutil/outcome.hpp"

using libp2p::crypto::ecdsa::EcdsaProviderImpl;
using libp2p::crypto::ecdsa::PrivateKey;
using libp2p::crypto::ecdsa::PublicKey;
using libp2p::crypto::ecdsa::Signature;

#define SAMPLE_MESSAGE_BYTES                                                  \
  0x57, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x6f, 0x74, 0x68,     \
      0x65, 0x72, 0x20, 0x70, 0x65, 0x6f, 0x70, 0x6c, 0x65, 0x27, 0x73, 0x20, \
      0x6e, 0x6f, 0x6e, 0x73, 0x65, 0x6e, 0x73, 0x65, 0x20, 0x69, 0x73, 0x20, \
      0x61, 0x6e, 0x20, 0x65, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20, 0x66, \
      0x72, 0x65, 0x65, 0x20, 0x70, 0x6c, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65, \
      0x2e

#define SAMPLE_MESSAGE_LENGTH 61

#define SAMPLE_PRIVATE_KEY_BYTES                                              \
  0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0xae, 0xbd, 0xb3, 0x98, 0xbe,     \
      0x22, 0x0a, 0x96, 0xe0, 0x2b, 0x38, 0x07, 0x86, 0xc7, 0x8a, 0x8e, 0xa2, \
      0xf4, 0xca, 0xeb, 0x76, 0x40, 0x65, 0xcf, 0xde, 0x05, 0xeb, 0xda, 0xe6, \
      0x40, 0x62, 0x56, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, \
      0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x3e, 0xd5, 0x82, \
      0x03, 0x3a, 0x1f, 0xc6, 0x9c, 0x34, 0xd0, 0xc9, 0x7e, 0x40, 0x7d, 0x27, \
      0x42, 0xa4, 0xb2, 0x06, 0x26, 0x3f, 0x18, 0xf5, 0x09, 0x91, 0x8a, 0x21, \
      0xc9, 0x13, 0xa8, 0xc6, 0x43, 0x43, 0x2e, 0x13, 0xc1, 0xe3, 0x5b, 0xe9, \
      0x95, 0xf2, 0xf5, 0xbd, 0x2d, 0xd2, 0xde, 0x4f, 0x02, 0x91, 0x31, 0x0d, \
      0xb7, 0x1e, 0x30, 0x25, 0x7f, 0xea, 0x77, 0x88, 0x80, 0x19, 0xfc, 0x2f, \
      0x72

#define SAMPLE_PUBLIC_KEY_BYTES                                               \
  0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02,     \
      0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, \
      0x42, 0x00, 0x04, 0x3e, 0xd5, 0x82, 0x03, 0x3a, 0x1f, 0xc6, 0x9c, 0x34, \
      0xd0, 0xc9, 0x7e, 0x40, 0x7d, 0x27, 0x42, 0xa4, 0xb2, 0x06, 0x26, 0x3f, \
      0x18, 0xf5, 0x09, 0x91, 0x8a, 0x21, 0xc9, 0x13, 0xa8, 0xc6, 0x43, 0x43, \
      0x2e, 0x13, 0xc1, 0xe3, 0x5b, 0xe9, 0x95, 0xf2, 0xf5, 0xbd, 0x2d, 0xd2, \
      0xde, 0x4f, 0x02, 0x91, 0x31, 0x0d, 0xb7, 0x1e, 0x30, 0x25, 0x7f, 0xea, \
      0x77, 0x88, 0x80, 0x19, 0xfc, 0x2f, 0x72

#define SAMPLE_SIGNATURE_BYTES                                                \
  0x30, 0x45, 0x02, 0x21, 0x00, 0x80, 0xd0, 0x67, 0xef, 0x81, 0x7b, 0x63,     \
      0x6e, 0x5b, 0x7e, 0xa0, 0xfa, 0x46, 0x17, 0x44, 0x1f, 0xf9, 0xe0, 0xb7, \
      0x05, 0x46, 0x22, 0xe4, 0xd6, 0xc3, 0xdc, 0x1c, 0xc5, 0xd1, 0x79, 0x4c, \
      0x8d, 0x02, 0x20, 0x38, 0xe7, 0xc4, 0xe1, 0xd2, 0xf3, 0x85, 0xa7, 0xc5, \
      0xfb, 0x22, 0xd9, 0x9a, 0x48, 0x5d, 0x79, 0x1a, 0x17, 0x30, 0xfd, 0x20, \
      0xcd, 0x96, 0xdb, 0x76, 0x23, 0x45, 0x55, 0xdb, 0x2b, 0xcc, 0x40

class EcdsaProviderTest : public ::testing::Test {
 protected:
  EcdsaProviderImpl provider_;
  PrivateKey private_key_{SAMPLE_PRIVATE_KEY_BYTES};
  PublicKey public_key_{SAMPLE_PUBLIC_KEY_BYTES};
  Signature signature_{SAMPLE_SIGNATURE_BYTES};
  std::array<uint8_t, SAMPLE_MESSAGE_LENGTH> message_{SAMPLE_MESSAGE_BYTES};
};

/**
 * @given Pre-generated ECDSA private and public keys
 * @when Deriving public key from private key
 * @then Derived public must be the same as the pre-generated
 */
TEST_F(EcdsaProviderTest, DerivePublicKeySuccess) {
  EXPECT_OUTCOME_TRUE(derived_key, provider_.DerivePublicKey(private_key_));
  ASSERT_EQ(public_key_, derived_key);
}

/**
 * @given Sample message for signing
 * @when Generating ECDSA key pair, signing and signature verification
 * @then Signature verification must be successful and signature must be valid
 */
TEST_F(EcdsaProviderTest, SignVerifySuccess) {
  EXPECT_OUTCOME_TRUE(key_pair, provider_.GenerateKeyPair());
  EXPECT_OUTCOME_TRUE(signature,
                      provider_.Sign(message_, key_pair.private_key));
  EXPECT_OUTCOME_TRUE(
      verify_result,
      provider_.Verify(message_, signature, key_pair.public_key));
  ASSERT_TRUE(verify_result);
}

/**
 * @given Sample message, ECDSA key pair and sample signature
 * @when Verifying pre-generated signature
 * @then Pre-generated signature must be valid
 */
TEST_F(EcdsaProviderTest, SampleSignVerifySuccess) {
  EXPECT_OUTCOME_TRUE(verify_result,
                      provider_.Verify(message_, signature_, public_key_));
  ASSERT_TRUE(verify_result);
}

/**
 * @given Pre-generated message and ECDSA signature
 * @when Verifying ECDSA signature with invalid public key
 * @then Signature must be invalid
 */
TEST_F(EcdsaProviderTest, VerifyInvalidPubKeyFailure) {
  EXPECT_OUTCOME_TRUE(key_pair, provider_.GenerateKeyPair());
  EXPECT_OUTCOME_TRUE(
      verify_result,
      provider_.Verify(message_, signature_, key_pair.public_key));
  ASSERT_FALSE(verify_result);
}

/**
 * @given Pre-generated ECDSA key pair and signature
 * @when Verifying signature for different message
 * @then Signature must be invalid
 */
TEST_F(EcdsaProviderTest, VerifyInvalidMessageFailure) {
  std::array<uint8_t, 1> different_message{};
  EXPECT_OUTCOME_TRUE(
      verify_result,
      provider_.Verify(different_message, signature_, public_key_));
  ASSERT_FALSE(verify_result);
}

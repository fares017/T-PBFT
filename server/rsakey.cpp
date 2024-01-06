#include "rsakey.h"


int RSAKeyGenerator::orderNumber = 0;
int RSAKeyGenerator::randomValue = 0;
int RSAKeyGenerator::prepared_messages =0;

int RSAKeyGenerator::getOrderNumber() {
    return orderNumber;
}

void RSAKeyGenerator::incrementOrderNumber() {
    orderNumber++;
}

int RSAKeyGenerator::getRandomValue() {
    return randomValue;
}

void RSAKeyGenerator::randomValueGenerator() {
    std::random_device rd;
    std::mt19937 generator(rd());

    // Define a distribution for the range of random values you want
    std::uniform_int_distribution<int> distribution(0,3); // Change the range as needed

    // Generate a random value
    randomValue = distribution(generator);
}

void RSAKeyGenerator::clearPreparedMessages() {
    prepared_messages = 0;
}

void RSAKeyGenerator::incrementPreparedMessages() {
    prepared_messages++;
}

int RSAKeyGenerator::getPreparedMessages() {
    return prepared_messages;
}

RSAKeyGenerator::RSAKeyGenerator() {}
CryptoPP::RSA::PrivateKey RSAKeyGenerator::GeneratePrivateKey() {
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 2048);
    CryptoPP::RSA::PrivateKey privateKey(params);
    return privateKey;
}

CryptoPP::RSA::PublicKey RSAKeyGenerator::GeneratePublicKey(const CryptoPP::RSA::PrivateKey& privateKey) {
    CryptoPP::RSA::PublicKey publicKey(privateKey);
    return publicKey;
}
// BIGNUM *RSAKeyGenerator::bnfromInteger(const CryptoPP::Integer& integer) {
//     CryptoPP::SecByteBlock byteBlock(integer.ByteCount());
//     integer.Encode(byteBlock.BytePtr(), byteBlock.SizeInBytes());
//     return BN_bin2bn(byteBlock.BytePtr(), byteBlock.SizeInBytes(), nullptr);
// }

// EVP_PKEY* RSAKeyGenerator::ConvertPrivatetoEVP(const CryptoPP::RSA::PrivateKey& privateKey) {
//     const CryptoPP::Integer& n = privateKey.GetModulus();
//     const CryptoPP::Integer& d = privateKey.GetPrivateExponent();
//     const CryptoPP::Integer& e = privateKey.GetPublicExponent();
//     const CryptoPP::Integer& p = privateKey.GetPrime1();
//     const CryptoPP::Integer& q = privateKey.GetPrime2();

//     BIGNUM *bn_n = bnfromInteger(n);
//     BIGNUM *bn_d = bnfromInteger(d);
//     BIGNUM *bn_e= bnfromInteger(e);
//     BIGNUM *bn_p= bnfromInteger(p);
//     BIGNUM *bn_q= bnfromInteger(q);

//     RSA* rsa = RSA_new();
//     RSA_set0_key(rsa, bn_n, bn_e, bn_d);
//     RSA_set0_factors(rsa, bn_p, bn_q);
//     EVP_PKEY* evpKey = EVP_PKEY_new();
//     EVP_PKEY_assign_RSA(evpKey, rsa);

//     return evpKey;
// }

// EVP_PKEY* RSAKeyGenerator::ConvertPublictoEVP(const CryptoPP::RSA::PublicKey& publicKey) {
//     const CryptoPP::Integer& n = publicKey.GetModulus();
//     const CryptoPP::Integer& d = publicKey.GetPublicExponent();

    
//     BIGNUM *bn_n = bnfromInteger(n);
//     BIGNUM *bn_d = bnfromInteger(d);

//     RSA* rsa = RSA_new();
//     RSA_set0_key(rsa, bn_n, bn_d, nullptr);
//     EVP_PKEY* evpKey = EVP_PKEY_new();
//     EVP_PKEY_assign_RSA(evpKey, rsa);

//     return evpKey;
// }

bool RSAKeyGenerator::VerifySignature(const std::string& message, const std::string& signature, const CryptoPP::RSA::PublicKey& publicKey) {
    CryptoPP::SHA256 sha256;
    CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    sha256.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(message.c_str()), message.length());

    // Decode the base64-encoded signature
    CryptoPP::Base64Decoder decoder;
    decoder.Put(reinterpret_cast<const CryptoPP::byte*>(signature.c_str()), signature.length());
    decoder.MessageEnd();
    CryptoPP::SecByteBlock signatureBytes(decoder.MaxRetrievable());
    decoder.Get(signatureBytes, signatureBytes.size());

    // Verify the signature
    CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Verifier verifier(publicKey);
    return verifier.VerifyMessage(digest, sha256.DigestSize(), signatureBytes, signatureBytes.size());
}

std::string RSAKeyGenerator::SignMessage(const std::string& message, const CryptoPP::RSA::PrivateKey& privateKey) {
    CryptoPP::SHA256 sha256;
    CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
    sha256.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(message.c_str()), message.length());

    CryptoPP::RSASS<CryptoPP::PSSR, CryptoPP::SHA256>::Signer signer(privateKey);
    CryptoPP::SecByteBlock signature(signer.MaxSignatureLength());
    size_t signatureLength = signer.SignMessage(rng, digest, sha256.DigestSize(), signature);

    std::string signatureBase64;
    CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(signatureBase64), false);
    encoder.Put(signature, signatureLength);
    encoder.MessageEnd();

    return signatureBase64;
}
void RSAKeyGenerator::assignGroupKey() {
    RSAKeyGenerator keyGenerator;
    CryptoPP::RSA::PrivateKey groupPrivateKey = keyGenerator.GeneratePrivateKey();
    CryptoPP::RSA::PublicKey groupPublicKey = keyGenerator.GeneratePublicKey(groupPrivateKey);
    // std::vector<salticidae::PeerId> primary_group = TrustManager::primaryGroup;
    // NodeVector nodes = Handler::get_nodes();
    // for (size_t i = 0; i < NUM_NODES; i++)
    // {
    //     if (std::find(primary_group.begin(), primary_group.end(), nodes[i].peerId) != primary_group.end())
    //     {
    //         nodes[i].set_group_privateKey(groupPrivateKey);
    //     }
    //     nodes[i].set_group_publicKey(groupPublicKey);
    // }

}
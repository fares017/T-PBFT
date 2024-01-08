#include "rsakey.h"


int RSAKeyGenerator::orderNumber = 0;
int RSAKeyGenerator::randomValue = 0;
int RSAKeyGenerator::prepared_messages =0;

CryptoPP::RSA::PrivateKey RSAKeyGenerator::groupPrivateKey;
CryptoPP::RSA::PublicKey RSAKeyGenerator::groupPublicKey;
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
    std::vector<salticidae::PeerId> primary = TrustManager::primaryGroup;
    std::uniform_int_distribution<int> distribution(0,primary.size()-1); // Change the range as needed

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

CryptoPP::RSA::PrivateKey RSAKeyGenerator::getGroupPrivateKey(const salticidae::PeerId peerId) {
    std::vector<salticidae::PeerId> primary_group = TrustManager::primaryGroup;
    if (std::find(primary_group.begin(), primary_group.end(), peerId) != primary_group.end()) {
        return groupPrivateKey;
    } else {
        throw std::invalid_argument("Invalid Peer ID");
    }
}

CryptoPP::RSA::PublicKey RSAKeyGenerator::getGroupPublicKey() {
    return groupPublicKey;
}

void RSAKeyGenerator::changeGroupKey() {
    RSAKeyGenerator keyGenerator;
    CryptoPP::RSA::PrivateKey prKey = keyGenerator.GeneratePrivateKey();
    CryptoPP::RSA::PublicKey puKey = keyGenerator.GeneratePublicKey(prKey);

    const CryptoPP::Integer& n = prKey.GetModulus();
    const CryptoPP::Integer& d = prKey.GetPrivateExponent();
    const CryptoPP::Integer& e = prKey.GetPublicExponent();

    groupPrivateKey.Initialize(n, e, d);
    const CryptoPP::Integer& n2 = puKey.GetModulus();
    const CryptoPP::Integer& e2 = puKey.GetPublicExponent();
    groupPublicKey.Initialize(n2, e2);
    
}
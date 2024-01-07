#ifndef RSA_KEY_GENERATOR_H
#define RSA_KEY_GENERATOR_H


#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <cryptopp/base64.h>
#include <cryptopp/pssr.h>
#include <random>
#include <salticidae/network.h>
#include <node.h>
#include <stdexcept>

#include "TrustManager.h"
#include "config.h"

class RSAKeyGenerator {
    public:
        RSAKeyGenerator();
        CryptoPP::RSA::PrivateKey GeneratePrivateKey(); //Method to generate private key
        CryptoPP::RSA::PublicKey GeneratePublicKey(const CryptoPP::RSA::PrivateKey& privateKey); //Method to generate public key
        // EVP_PKEY* ConvertPrivatetoEVP(const CryptoPP::RSA::PrivateKey& privateKey); //not used
        // EVP_PKEY* ConvertPublictoEVP(const CryptoPP::RSA::PublicKey& publicKey); //not used
        // BIGNUM *bnfromInteger(const CryptoPP::Integer& integer); //not using
        bool VerifySignature(const std::string& message, const std::string& signature, const CryptoPP::RSA::PublicKey& publicKey); //method to verify signature with public key
        std::string SignMessage(const std::string &message, const CryptoPP::RSA::PrivateKey &privateKey); //method to sign the message and order number with private key;
        static int getOrderNumber();  // Method to retrieve the current order number
        static void incrementOrderNumber();  // Method to increment the order number 
        static void randomValueGenerator(); 
        static int getRandomValue();
        //static void assignGroupKey();
        static void incrementPreparedMessages();
        static int getPreparedMessages();
        static void clearPreparedMessages();
        static CryptoPP::RSA::PrivateKey getGroupPrivateKey(const salticidae::PeerId peerId);
        static CryptoPP::RSA::PublicKey getGroupPublicKey();
        static void changeGroupKey();

    private:
        CryptoPP::AutoSeededRandomPool rng; //random generator 
        static int orderNumber;  // Static member variable to store the order number
        static int randomValue; //randomValue for primary
        static int prepared_messages; //for checking 2f
        static CryptoPP::RSA::PrivateKey groupPrivateKey;
        static CryptoPP::RSA::PublicKey groupPublicKey;
};
#endif
//#ifndef TAZPLAINDATA
//#include "Encrypt.h"
//#ifndef ENCRYPT_H
//#include <cryptopp/aes.h>
//#include <cryptopp/modes.h>
//#include <cryptopp/filters.h>
//
//void encryptUint8(std::vector<uint8_t>& regulardata, const char* ivStr, const char* keyStr) {
//    CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(ivStr), 16);
//    CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(keyStr), 16);
//
//    CryptoPP::AES::Encryption aesEncryption((CryptoPP::byte*)key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
//    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, (CryptoPP::byte*)iv.data());
//
//    std::vector<uint8_t> encrypted;
//    encrypted.reserve(regulardata.size()); // Reserve memory to avoid reallocations
//
//    // Stream-based encryption
//    CryptoPP::StreamTransformationFilter encryptor(cbcEncryption, new CryptoPP::VectorSink(encrypted));
//    encryptor.Put((CryptoPP::byte*)regulardata.data(), regulardata.size());
//    encryptor.MessageEnd();
//
//    regulardata = std::move(encrypted);
//}
//void encryptUint8Chunked(std::vector<uint8_t>& regulardata,
//    uint64_t& chunks,
//    std::vector<size_t>& chunkSizes,
//    const char* ivStr,
//    const char* keyStr,
//    size_t chunkSize
//) {
//    size_t dataSize = regulardata.size();
//    size_t numChunks = (dataSize + chunkSize - 1) / chunkSize; // Calculate number of chunks
//    chunks = static_cast<__int64>(numChunks);
//
//    std::vector<std::future<std::vector<uint8_t>>> futures;
//    futures.reserve(numChunks);
//
//    // Encrypt each chunk asynchronously
//    for (size_t i = 0; i < numChunks; i++) {
//        futures.push_back(std::async(std::launch::async, [&, i]() -> std::vector<uint8_t> {
//            size_t offset = i * chunkSize;
//            size_t bytesToProcess = std::min(chunkSize, dataSize - offset);
//
//            CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(ivStr), 16);
//            CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(keyStr), 16);
//
//            CryptoPP::AES::Encryption aesEncryption((CryptoPP::byte*)key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
//            CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, (CryptoPP::byte*)iv.data());
//
//            std::vector<uint8_t> chunkEncrypted;
//            CryptoPP::StreamTransformationFilter encryptor(
//                cbcEncryption,
//                new CryptoPP::VectorSink(chunkEncrypted)
//            );
//
//            encryptor.Put(reinterpret_cast<const CryptoPP::byte*>(regulardata.data() + offset), bytesToProcess);
//            encryptor.MessageEnd();
//
//            return chunkEncrypted;
//            }));
//    }
//
//    // Collect results
//    std::vector<uint8_t> encrypted;
//    for (auto& future : futures) {
//        std::vector<uint8_t> chunk = future.get();
//        chunkSizes.push_back(chunk.size());
//        encrypted.insert(encrypted.end(), chunk.begin(), chunk.end());
//    }
//
//    regulardata = std::move(encrypted);
//}
//#endif
//#endif
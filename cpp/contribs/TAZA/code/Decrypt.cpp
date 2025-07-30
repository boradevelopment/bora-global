//#ifndef TAZPLAINDATA
//#include "Decrypt.h"
//
//#ifndef DECRYPT_H
//#include <cryptopp/aes.h>
//#include <cryptopp/modes.h>
//#include <cryptopp/filters.h>
//#include <iostream>
//
//void decryptUint8(std::vector<uint8_t>& encrypteddata, const char* ivStr, const char* keyStr) {
//    std::cout << "Crypto++ version: " << CRYPTOPP_VERSION << std::endl;
//    std::cout << "Lib Version: " << CryptoPP::LibraryVersion();
//    CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(ivStr), 16);
//    CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(keyStr), 16);
//
//    CryptoPP::AES::Decryption aesDecryption((CryptoPP::byte*)key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
//    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, (CryptoPP::byte*)iv.data());
//
//    std::vector<uint8_t> decrypted;
//
//    std::string ciphertext(encrypteddata.begin(), encrypteddata.end());
//
//    CryptoPP::StreamTransformationFilter decryptor(cbcDecryption, new CryptoPP::VectorSink(decrypted));
//    decryptor.Put((CryptoPP::byte*)ciphertext.data(), ciphertext.size());
//    decryptor.MessageEnd();
//
//    encrypteddata = std::move(decrypted);
//}
//
//void decryptUint8Chunked(std::vector<uint8_t>& regulardata,
//    const char* ivStr,
//    const char* keyStr,
//    const std::vector<size_t>& chunkSizes
//) {
//    size_t totalChunks = chunkSizes.size();
//    std::vector<std::future<std::vector<uint8_t>>> futures;
//    futures.reserve(totalChunks);
//
//    size_t offset = 0;
//
//    // Process each chunk asynchronously
//    for (size_t i = 0; i < totalChunks; i++) {
//        size_t chunkSize = chunkSizes[i];
//        if (offset + chunkSize > regulardata.size()) {
//            throw std::runtime_error("Chunk size exceeds available encrypted data");
//        }
//
//        futures.push_back(std::async(std::launch::async, [&, offset, chunkSize]() -> std::vector<uint8_t> {
//            CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(ivStr), 16);
//            CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(keyStr), 16);
//
//            CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
//            CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
//
//            std::vector<uint8_t> chunkDecrypted;
//            CryptoPP::StreamTransformationFilter decryptor(
//                cbcDecryption,
//                new CryptoPP::VectorSink(chunkDecrypted)
//            );
//
//            decryptor.Put(reinterpret_cast<const CryptoPP::byte*>(regulardata.data() + offset), chunkSize);
//            decryptor.MessageEnd();
//
//            return chunkDecrypted;
//            }));
//
//        offset += chunkSize;
//    }
//
//    // Collect decrypted chunks
//    std::vector<uint8_t> decrypted;
//    for (auto& future : futures) {
//        std::vector<uint8_t> chunk = future.get();
//        decrypted.insert(decrypted.end(), chunk.begin(), chunk.end());
//    }
//
//    regulardata = std::move(decrypted);
//}
//
//#endif
//#endif
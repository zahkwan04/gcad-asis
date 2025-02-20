/**
 * AES256 class for message encryption.
 *
 * Copyright (C) Sapura Secured Technologies, 2021. All Rights Reserved.
 *
 * @file
 * @version $Id: Aes.h 1495 2021-06-24 09:36:06Z rosnin $
 * @author Rosnin Mustaffa
 * @author Mohd Rozaimi
 */
#ifndef AES_H
#define AES_H
#include <string>

#ifndef uchar
typedef unsigned char uchar;
#endif
#ifndef uint
typedef unsigned int uint;
#endif

class Aes
{
public:
    /**
     * Checks a key and modifies it if not of the correct length.
     *
     * @param[in,out] key The key.
     */
    static void validateKey(std::string &key);

    /**
     * Checks whether a given data size is enough for a complete input block,
     * and calculates the excess beyond complete blocks.
     *
     * @param[in] sz The size.
     * @return -1 if insufficient block size. Otherwise the excess size.
     */
    static int getSizeExcess(size_t sz);

    /**
     * Encrypts a message block by block.
     *
     * @param[in] str The message.
     * @param[in] key Encryption key.
     * @return The cipher text.
     */
    static std::string encrypt(const std::string &str, const std::string &key);

    /**
     * Decrypts a message block by block.
     *
     * @param[in] str The message.
     * @param[in] key Decryption key.
     * @return The plain text.
     */
    static std::string decrypt(const std::string &str, const std::string &key);

private:
    /**
     * Generates round keys to encrypt the states.
     *
     * @param[in]  inKey    Input key.
     * @param[out] roundKey Round keys.
     */
    static void keyExpansion(const uchar *inKey, uchar *roundKeys);

    //following are standard-defined AES functions - no headers necessary
    static void addRoundKey(uint *state, uint *key);

    static void mixSubColumns(uchar *state);

    static void invMixSubColumns(uchar *state);

    static void shiftRows(uchar *state);

    static void invShiftRows(uchar *state);
};
#endif //AES_H

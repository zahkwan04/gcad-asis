/**
 * Interface to MD5 Message-Digest Algorithm.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2019. All Rights Reserved.
 *
 * @file
 * @version $Id: Md5Digest.h 1283 2019-11-29 08:52:44Z rosnin $
 * @author Mohd Rashid
 * @author Zahari Hadzir
 */
#ifndef MD5DIGEST_H
#define MD5DIGEST_H

#include <string>

#include "MD5.h"

/**
 * Calculates MD5 digest of a string with another challenge string.
 *
 * @param[in,out] str       In = Input string.
 *                          Out = Hexadecimal digest string.
 * @param[in]     challenge A random hexadecimal string. Not used if empty.
 * @param[in]     lowerCase true for lower case output.
 */
void md5Digest(std::string       &str,
               const std::string &challenge,
               bool               lowerCase = false);

/**
 * Calculates MD5 digest of a string.
 *
 * @param[in] str Input string.
 * @return Lowercase hexadecimal digest string.
 */
std::string md5Digest(const std::string &str);

/**
 * Calculates MD5 digest of a file's entire content.
 *
 * @param[in] filename The filename.
 * @return Uppercase hexadecimal digest string, or empty string on failure.
 */
std::string md5DigestFile(const std::string &filename);

#endif //MD5DIGEST_H

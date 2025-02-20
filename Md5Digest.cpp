/**
 * Implements string digest using MD5 Message-Digest Algorithm.
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2019. All Rights Reserved.
 *
 * @file
 * @version $Id: Md5Digest.cpp 1283 2019-11-29 08:52:44Z rosnin $
 * @author Mohd Rashid
 * @author Zahari Hadzir
 */
#include <fstream>
#include <stdio.h> //sprintf

#include "Md5Digest.h"

using namespace std;

static string finalize(MD5_CTX *ctx, bool lowerCase)
{
    static const int  MD5SIZE = 16;
    unsigned char     digestBin[MD5SIZE];
    char              digestAscii[(2 * MD5SIZE) + 1]; //with null terminator
    unsigned char    *pBin = digestBin;
    char             *pAsc = digestAscii;
    int               i = MD5SIZE;
    MD5Final(digestBin, ctx);
    if (lowerCase)
    {
        for (; i>0; --i, ++pBin, pAsc+=2)
        {
            sprintf(pAsc, "%02x", *pBin);
        }
    }
    else
    {
        for (; i>0; --i, ++pBin, pAsc+=2)
        {
            sprintf(pAsc, "%02X", *pBin);
        }
    }
    *pAsc = 0; //terminate string
    return string(digestAscii);
}

void md5Digest(string &passwd, const string &challenge, bool lowerCase)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char *) passwd.c_str(), passwd.size());
    if (!challenge.empty())
        MD5Update(&ctx, (unsigned char *) challenge.c_str(),
                  challenge.size());
    passwd = finalize(&ctx, lowerCase);
}

string md5Digest(const string &str)
{
    string digest(str);
    md5Digest(digest, "", true);
    return digest;
}

string md5DigestFile(const string &filename)
{
    ifstream ifs(filename.c_str(), ios::binary);
    if (!ifs.is_open())
        return "";
    static const int BLOCKSIZE = 1024;
    char             data[BLOCKSIZE];
    MD5_CTX          ctx;
    unsigned int     i;
    MD5Init(&ctx);
    while (!ifs.eof())
    {
        ifs.read(data, BLOCKSIZE);
        i = ifs.gcount();
        if (i > 0)
            MD5Update(&ctx, (unsigned char *) data, i);
    }
    ifs.close();
    return finalize(&ctx, false);
}

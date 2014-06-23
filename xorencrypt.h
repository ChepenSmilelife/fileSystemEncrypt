/*
 * =====================================================================================
 *
 *       Filename:  xorencrypt.h
 *
 *    Description:  xor bytes
 *
 *        Version:  1.0
 *        Created:  06/23/2014 09:13:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wkyo (none), wblanksky@hotmail.com
 *   Organization:  none
 *
 * =====================================================================================
 */


#ifndef HEAD_XORENCRYPT_H
#define HEAD_XORENCRYPT_H


#ifdef __cplusplus
extern "C" {
#endif


extern int __xorbytesEncrypt( unsigned char *pwd, int pwd_len,
		const char *inputFile, const char *outputFile );


#ifdef __cplusplus
}
#endif

#endif


/*
 * =====================================================================================
 *
 *       Filename:  encrypt.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2014 10:13:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wkyo (none), wblanksky@hotmail.com
 *   Organization:  none
 *
 * =====================================================================================
 */


#ifndef HEADER_ENCRYPT_H
#define HEADER_ENCRYPT_H	1

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MagicHeadLen 16

/* if has been encryped return 1, else return 0 */
int __fcheckEncrypted( FILE *fp );
int __checkMagicHead( const char *name );

void __encryptAs( FILE *inputfp, FILE *outputfp, unsigned char *pwd, int pwd_length );
void __encrypt( const char *name, unsigned char *pwd, int pwd_length );

void __decryptAs( FILE *inputfp, FILE *outputfp, unsigned char *pwd, int pwd_length );
void __decrypt( const char *name, unsigned char *pwd, int pwd_length );

#ifdef __cplusplus
}
#endif

#endif



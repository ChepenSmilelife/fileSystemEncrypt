/*
 * =====================================================================================
 *
 *       Filename:  xorencrypt.c
 *
 *    Description:  xor bytes
 *
 *        Version:  1.0
 *        Created:  06/23/2014 09:21:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wkyo (none), wblanksky@hotmail.com
 *   Organization:  none
 *
 * =====================================================================================
 */


#include "xorencrypt.h"
#include <stdio.h>


extern int __xorbytesEncrypt( unsigned char *pwd, int pwd_len,
		const char *inputFile, const char *outputFile )
{
	FILE *inputFp = fopen( inputFile, "r" );
	FILE *outputFp = fopen( outputFile, "w" );

	if( inputFp && outputFp ) {
		int pwd_pos = 0;
		unsigned char c;
		while( c = fgetc( inputFp ), !feof( inputFp ) ) {
			fputc( c^pwd[pwd_pos++], outputFp );
			if( pwd_pos >= pwd_len-1 )
				pwd_pos = 0;
		}

		fclose( inputFp );
		fclose( outputFp );
        return 1;
	}
    return 0;
}

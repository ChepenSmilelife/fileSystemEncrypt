/*
 * =====================================================================================
 *
 *       Filename:  encrypt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2014 10:33:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wkyo (none), wblanksky@hotmail.com
 *   Organization:  none
 *
 * =====================================================================================
 */

#include "encrypt.h"
#include <unistd.h>
#include <string.h>

static unsigned char MagicHead[MagicHeadLen] = {
	0x70, 0x6a, 0xab, 0x66,
	0xdd, 0xeb, 0x8a, 0xa5,
	0xf5, 0xfa, 0x0c, 0xf6,
	0x46, 0xa8, 0x48, 0x8d
};

/* if has been encryped return 1, else return 0 */
int __fcheckEncrypted( FILE *fp )
{
	unsigned char tmp[MagicHeadLen];
	size_t p = 0;
	size_t num;

	rewind( fp );
	num = fread( tmp, 1, MagicHeadLen, fp );

	for( ; p < num && tmp[p] == MagicHead[p]; ++p )
		;

	return ( p == MagicHeadLen ) ? 1 : 0;
}

int __checkMagicHead( const char *name)
{
    FILE *fp = fopen( name, "r" );
    int r = __fcheckEncrypted( fp );
    fclose( fp );
    return r;
}

void __encryptAs( FILE *inputfp, FILE *outputfp, unsigned char *pwd, int pwd_length )
{
	unsigned char c;
	size_t pos = 0;                                 /* pos in pwd */

	/*  set file offset to 0  */
	rewind( inputfp );
	rewind( outputfp );

	/* add magic head to outputfile */
	fwrite( MagicHead, 1, MagicHeadLen, outputfp );

	/* encrypt input file to output file */
	while( c = fgetc( inputfp ), !feof( inputfp ) )
		fputc( c^pwd[pos++%pwd_length], outputfp );
}

static void updateBuffer( unsigned char *buffer, int *bufferlen, FILE *fp )
{
    if( *bufferlen < MagicHeadLen ) { /* if buffer length less than MagicHeadLen, meet end */
        fwrite( buffer, 1, *bufferlen, fp );
        *bufferlen = -1;
    }
    else {
        unsigned char tmpBuffer[MagicHeadLen];
        int num = fread(tmpBuffer, 1, MagicHeadLen, fp );
        fseek(fp, -num, 1);
        fwrite( buffer, 1, *bufferlen, fp );
        memcpy( buffer, tmpBuffer, num );
        *bufferlen = num;
    }

}

void __encrypt(const char *name, unsigned char *pwd, int pwd_length)
{
    int bufferlen = MagicHeadLen;
    unsigned char buffer[bufferlen];

    int pos = 0; /* pos in pwd */
    FILE *fp = fopen( name, "r+" );
    memcpy( buffer, MagicHead, MagicHeadLen );

    updateBuffer( buffer, &bufferlen, fp );

    while( bufferlen != -1 ) {
        int i;
        for( i = 0; i < bufferlen; ++i ) {
            buffer[i] ^= pwd[pos++];
            if( pos >= pwd_length )
                pos = 0;
        }
        updateBuffer( buffer, &bufferlen, fp );
    }
    fclose( fp );
}

void __decryptAs( FILE *inputfp, FILE *outputfp, unsigned char *pwd, int pwd_length )
{
	unsigned char c;
	size_t pos = 0;

	/* set file offset to 0 */
	rewind( inputfp );
	rewind( outputfp );

	fseek( inputfp, MagicHeadLen, 0 );

	/* decrypt from input file to output file */
	while( c = fgetc( inputfp ), !feof( inputfp ) )
		fputc( c^pwd[pos++%pwd_length], outputfp );
}

void __decrypt(const char *name, unsigned char *pwd, int pwd_length)
{
    unsigned char buffer[MagicHeadLen];
    FILE *fp = fopen( name, "r+" );
    int num = 0;
    int pos = 0;
    int i;
    unsigned long size = 0;
    fseek( fp, MagicHeadLen, 0 );
    while ( ( num = fread( buffer, 1, MagicHeadLen, fp ) ) == MagicHeadLen )
    {
        for( i = 0; i < MagicHeadLen; ++i ) {
            buffer[i] ^= pwd[pos++];
            if( pos >= pwd_length )
                pos = 0;
        }
        fseek( fp, -MagicHeadLen*2, 1 );
        fwrite( buffer, 1, MagicHeadLen, fp );
        fseek( fp, MagicHeadLen, 1);
        size += num;
    }
    for( i = 0; i < num; ++i ) {
        buffer[i] ^= pwd[pos++];
        if( pos >= pwd_length )
            pos = 0;
    }
    fseek( fp, -(MagicHeadLen+num), 1);
    fwrite( buffer, 1, num, fp );
    size += num;
    fclose( fp );

    truncate( name, size );
}


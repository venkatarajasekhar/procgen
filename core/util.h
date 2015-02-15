#ifndef _util_h_
#define _util_h_

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include "ttycolours.h"

// for whitespace
#include <wctype.h>

#ifndef thread_local
#define thread_local __thread
#endif

template<typename T>
T clamp( T inval, T minval, T maxval ) {
	if( inval < minval )
		return minval;
	if( inval > maxval )
		return maxval;
	return inval;
}
// some bit counting funcs from http://graphics.stanford.edu/~seander/bithacks.html
inline int bitweight14( int v ) {
	// option 1, for at most 14-bit values in v:
	return (v * 0x200040008001ULL & 0x111111111111111ULL) % 0xf;
}
inline int bitweight32( int v ) {
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	int c = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24; // count
	return c;
}

inline uint32_t jenkins_one_at_a_time_hash( const char *key, size_t len ) {
	if( len == 0xffffffff )
		len = strlen( key );
	uint32_t hash, i;
	for(hash = i = 0; i < len; ++i) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

time_t get_mtime(const char *path, time_t def );
time_t get_ctime(const char *path, time_t def );
int is_redirected();

inline int eprintf( const char *format, ... ) {
	va_list ap;
	va_start(ap, format);
	time_t clock = time(NULL);
	tm *cal = gmtime( &clock );
	char buffer[ 64 ];
	strftime( buffer, 64, "%F %T", cal );
	if( is_redirected() ) {
		fprintf( stderr, "[err@%s]", buffer );
	} else {
		fprintf( stderr, RED "[err@%s]" CLEAR, buffer );
	}
	int result = vfprintf( stderr, format, ap );
	fflush( stderr );
	return result;
}

void stripcolours( char *str );

inline int Log( int level, const char *format, ... ) {
	extern int loggingLevel;
	if( level > loggingLevel )
		return 0;
	va_list ap;
	va_start(ap, format);
	time_t clock = time(NULL);
	tm *cal = gmtime( &clock );
	char buffer[ 64 ];
	strftime( buffer, 64, "%F %T", cal );
	fprintf( stdout, "[%s]", buffer );
	char fbuf[1024];
	if( is_redirected() ) { strcpy( fbuf, format ); format = fbuf; stripcolours( fbuf ); }
	int result =  vfprintf( stdout, format, ap );
	fflush( stdout );
	return result;
}

void PrintTrace();
extern bool gQuit; // if this is true, any running processes should quit as soon as they can.
void SetupCrashdump();

inline void chomp( char *str ) {
	char * lastchar = str + strlen(str) - 1;
	while( lastchar >= str && iswspace( *lastchar ) ) {
		*lastchar = 0;
		--lastchar;
	}
}

char * ReadFileToString( const char *filename );

uint32_t GoodRandom();

bool EndsWith( const char *str, const char *suffix );

#endif

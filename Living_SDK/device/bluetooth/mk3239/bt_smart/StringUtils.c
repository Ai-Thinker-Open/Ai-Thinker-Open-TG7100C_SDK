#include "StringUtils.h"
#include "debug.h"
#include <stddef.h>
#include <stdarg.h>

#ifndef MIN
extern int MIN (/*@sef@*/ int x, /*@sef@*/ int
                          y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

#ifndef MAX
extern int MAX (/*@sef@*/ int x, /*@sef@*/ int
                          y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MAX(x,y)  ((x) > (y) ? (x) : (y))
#endif /* ifndef MAX */


#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

//===========================================================================================================================
//  formatMACAddr
//
//  Add ":" to every two character
//===========================================================================================================================
void formatMACAddr(char *destAddr, char *srcAddr)
{
    sprintf((char *)destAddr, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c", \
            toupper(*(char *)srcAddr), toupper(*((char *)(srcAddr) + 1)), \
            toupper(*((char *)(srcAddr) + 2)), toupper(*((char *)(srcAddr) + 3)), \
            toupper(*((char *)(srcAddr) + 4)), toupper(*((char *)(srcAddr) + 5)), \
            toupper(*((char *)(srcAddr) + 6)), toupper(*((char *)(srcAddr) + 7)), \
            toupper(*((char *)(srcAddr) + 8)), toupper(*((char *)(srcAddr) + 9)), \
            toupper(*((char *)(srcAddr) + 10)), toupper(*((char *)(srcAddr) + 11)));
}

//===========================================================================================================================
//  __strdup
//
//  Alloc a new memory and store the input content
//===========================================================================================================================
char *__strdup(const char *src)
{
    int len;
    char *dst;

    if (src == NULL) {
        return NULL;
    }

    if (src[0] == 0) {
        return NULL;
    }

    len = strlen(src) + 1;
    dst = (char *)malloc(len);
    if (dst) {
        memcpy(dst, src, len);
    }
    return dst;
}

char *__strdup_trans_dot(char *src)
{
    char *dst, *dstTemp;
    char *srcTemp = src;
    uint32_t newLen = strlen(src) + 1;

    while (*srcTemp != 0) {
        if (*srcTemp == '.') {
            newLen++;
        }
        srcTemp++;
    }

    dst = malloc(newLen);
    dstTemp = dst;
    srcTemp = src;

    while (*srcTemp != 0) {
        if (*srcTemp == '.') {
            *dstTemp++ = '/';
            *dstTemp++ = '.';
            srcTemp++;
        } else {
            *dstTemp++ = *srcTemp++;
        }
    }
    *dstTemp = 0x0;
    return dst;
}




/***
*strnlen - return the length of a null-terminated string
*
*Purpose:
*   Finds the length in bytes of the given string, not including
*   the final null character. Only the first maxsize characters
*   are inspected: if the null character is not found, maxsize is
*   returned.
*
*Entry:
*   const char * str - string whose length is to be computed
*   size_t maxsize
*
*Exit:
*   Length of the string "str", exclusive of the final null byte, or
*   maxsize if the null character is not found.
*
*Exceptions:
*
*******************************************************************************/
#if defined (__CC_ARM)
size_t strnlen(const char *str, size_t maxsize)
{
    size_t n;

    /* Note that we do not check if s == NULL, because we do not
    * return errno_t...
    */

    for (n = 0; n < maxsize && *str; n++, str++)
        ;

    return n;
}
#endif


void Int2Str(uint8_t *str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++) {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j - 1] == '0') & (Status == 0)) {
            j = 0;
        } else {
            Status++;
        }
    }
}

uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')) {
        if (inputstr[2] == '\0') {
            return 0;
        }
        for (i = 2; i < 11; i++) {
            if (inputstr[i] == '\0') {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i])) {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            } else {
                /* Return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* Over 8 digit hex --invalid */
        if (i >= 11) {
            res = 0;
        }
    } else { /* max 10-digit decimal input */
        for (i = 0; i < 11; i++) {
            if (inputstr[i] == '\0') {
                *intnum = val;
                /* return 1 */
                res = 1;
                break;
            } else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)) {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            } else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)) {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            } else if (ISVALIDDEC(inputstr[i])) {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            } else {
                /* return 0, Invalid input */
                res = 0;
                break;
            }
        }
        /* Over 10 digit decimal --invalid */
        if (i >= 11) {
            res = 0;
        }
    }

    return res;
}

unsigned int str2hex(unsigned char *ibuf, unsigned char *obuf,
                     unsigned int olen)
{
    unsigned int i;   /* loop iteration variable */
    unsigned int j = 0; /* current character */
    unsigned int by = 0;  /* byte value for conversion */
    unsigned char ch; /* current character */
    unsigned int len = strlen((char *)ibuf);

    /* process the list of characaters */
    for (i = 0; i < len; i++) {
        if (i == (2 * olen)) {
            // truncated it.
            return j + 1;
        }
        ch = ibuf[i];
        /* do the conversion */
        if (ch >= '0' && ch <= '9') {
            by = (by << 4) + ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            by = (by << 4) + ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
            by = (by << 4) + ch - 'a' + 10;
        } else {    /* error if not hexadecimal */
            return 0;
        }

        /* store a byte for each pair of hexadecimal digits */
        if (i & 1) {
            j = ((i + 1) / 2) - 1;
            obuf[j] = by & 0xff;
        }
    }
    return j + 1;
}

//===========================================================================================================================
//  TextToHardwareAddress
//
//  Parses hardware address text (e.g. AA:BB:CC:00:11:22:33:44 for Fibre Channel) into an n-byte array.
//  Segments can be separated by a colon ':', dash '-', or a space ' '. Segments do not need zero padding
//  (e.g. "0:1:2:3:4:5:6:7" is equivalent to "00:01:02:03:04:05:06:07").
//===========================================================================================================================

OSStatus TextToHardwareAddress( const void *inText, size_t inTextSize, size_t inAddrSize, void *outAddr )
{
    OSStatus            err;
    const char         *src;
    const char         *end;
    int                 i;
    int                 x;
    char                c;
    uint8_t            *dst;

    if ( inTextSize == kSizeCString ) {
        inTextSize = strlen( (const char *) inText );
    }
    src = (const char *) inText;
    end = src + inTextSize;
    dst = (uint8_t *) outAddr;

    while ( inAddrSize-- > 0 ) {
        x = 0;
        i = 0;
        while ( ( i < 2 ) && ( src < end ) ) {
            c = *src++;
            if (      isdigit_safe(  c ) ) {
                x = ( x * 16 )      +               ( c   - '0' );
                ++i;
            } else if ( isxdigit_safe( c ) ) {
                x = ( x * 16 ) + 10 + ( tolower_safe( c ) - 'a' );
                ++i;
            } else if ( ( i != 0 ) || ( ( c != ':' ) && ( c != '-' ) && ( c != ' ' ) ) ) {
                break;
            }
        }
        if ( i == 0 ) {
            err = kMalformedErr;
            goto exit;
        }
        require_action( (( x >= 0x00 ) && ( x <= 0xFF )), exit, err = kRangeErr );
        if ( dst ) {
            *dst++ = (uint8_t) x;
        }
    }
    err = kNoErr;

exit:
    return err;
}

char *DataToHexString( const uint8_t *inBuf, size_t inBufLen )
{
    char *buf_str = NULL;
    char *buf_ptr;
    require_quiet(inBuf, error);
    require_quiet(inBufLen > 0, error);

    buf_str = (char *) malloc (2 * inBufLen + 1);
    require(buf_str, error);
    buf_ptr = buf_str;
    uint32_t i;
    for (i = 0; i < inBufLen; i++) {
        buf_ptr += sprintf(buf_ptr, "%02X", inBuf[i]);
    }
    *buf_ptr = '\0';
    return buf_str;

error:
    if ( buf_str ) {
        free( buf_str );
    }
    return NULL;
}

char *DataToHexStringWithSpaces( const uint8_t *inBuf, size_t inBufLen )
{
    char *buf_str = NULL;
    char *buf_ptr = NULL;
    require_quiet(inBuf, error);
    require_quiet(inBufLen > 0, error);

    buf_str = (char *) malloc (3 * inBufLen + 1);
    require(buf_str, error);
    buf_ptr = buf_str;
    uint32_t i;
    for (i = 0; i < inBufLen; i++) {
        buf_ptr += sprintf(buf_ptr, "%02X ", inBuf[i]);
    }
    *buf_ptr = '\0';
    return buf_str;

error:
    if ( buf_str ) {
        free( buf_str );
    }
    return NULL;
}

char *DataToHexStringWithColons( const uint8_t *inBuf, size_t inBufLen )
{
    char *buf_str = NULL;
    char *buf_ptr = NULL;
    require_quiet(inBuf, error);
    require_quiet(inBufLen > 0, error);

    buf_str = (char *) malloc (3 * inBufLen + 1);
    require(buf_str, error);
    buf_ptr = buf_str;
    uint32_t i;
    for (i = 0; i < inBufLen; i++) {
        if ( i == inBufLen - 1 ) {
            buf_ptr += sprintf(buf_ptr, "%02X", inBuf[i]);
        } else {
            buf_ptr += sprintf(buf_ptr, "%02X:", inBuf[i]);
        }
    }
    *buf_ptr = '\0';
    return buf_str;

error:
    if ( buf_str ) {
        free( buf_str );
    }
    return NULL;
}

char *DataToCString( const uint8_t *inBuf, size_t inBufLen )
{
    char *cString = NULL;
    require_quiet(inBuf, error);
    require_quiet(inBufLen > 0, error);

    cString = (char *) malloc( inBufLen + 1 );
    require(cString, error);
    memcpy( cString, inBuf, inBufLen );
    *(cString + inBufLen ) = '\0';
    return cString;

error:
    if ( cString ) {
        free( cString );
    }
    return NULL;
}

//===========================================================================================================================
//  strnicmp
//
//  Like the ANSI C strncmp routine, but performs a case-insensitive compare.
//===========================================================================================================================

int strnicmp( const char *inS1, const char *inS2, size_t inMax )
{
    const char         *end;
    int                 c1;
    int                 c2;

    end = inS1 + inMax;
    while ( inS1 < end ) {
        c1 = tolower( *( (const unsigned char *) inS1 ) );
        c2 = tolower( *( (const unsigned char *) inS2 ) );
        if ( c1 < c2 ) {
            return ( -1 );
        }
        if ( c1 > c2 ) {
            return (  1 );
        }
        if ( c1 == '\0' ) {
            break;
        }

        ++inS1;
        ++inS2;
    }
    return ( 0 );
}

//===========================================================================================================================
//  strnicmpx
//
//  Like the ANSI C strncmp routine, but case-insensitive and requires all characters in s1 match all characters in s2.
//===========================================================================================================================

int strnicmpx( const void *inS1, size_t inN, const char *inS2 )
{
    const unsigned char        *s1;
    const unsigned char        *s2;
    int                         c1;
    int                         c2;

    s1 = (const unsigned char *) inS1;
    s2 = (const unsigned char *) inS2;
    while ( inN-- > 0 ) {
        c1 = tolower( *s1 );
        c2 = tolower( *s2 );
        if ( c1 < c2 ) {
            return ( -1 );
        }
        if ( c1 > c2 ) {
            return (  1 );
        }
        if ( c2 == 0 ) {
            return (  0 );
        }

        ++s1;
        ++s2;
    }
    if ( *s2 != 0 ) {
        return ( -1 );
    }
    return ( 0 );
}

//===========================================================================================================================
//  VSNScanF - va_list version of SNScanF.
//===========================================================================================================================

int VSNScanF( const void *inString, size_t inSize, const char *inFormat, va_list inArgs )
{
    int                         matched;
    const unsigned char        *src;
    const unsigned char        *end;
    const unsigned char        *fmt;
    const unsigned char        *old;
    const unsigned char        *setStart;
    const unsigned char        *setEnd;
    const unsigned char        *set;
    int                         notSet;
    int                         suppress;
    int                         alt;
    int                         storePtr;
    int                         fieldWidth;
    int                         sizeModifier;
    unsigned char              *s;
    int                        *i;
    int                         base;
    int                         negative;
    unsigned char               c;
    int64_t                     x;
    int                         v;
    void                       *p;
    const unsigned char       **ptrArg;
    size_t                     *sizeArg;
    size_t                      len;

    if ( inSize == kSizeCString ) {
        inSize = strlen( (const char *) inString );
    }
    src = (const unsigned char *) inString;
    end = src + inSize;
    fmt = (const unsigned char *) inFormat;

    matched = 0;
    for ( ;; ) {
        // Skip whitespace. 1 or more whitespace in the format matches 0 or more whitepsace in the string.

        if ( isspace( *fmt ) ) {
            ++fmt;
            while ( isspace( *fmt ) ) {
                ++fmt;
            }
            while ( ( src < end ) && isspace( *src ) ) {
                ++src;
            }
        }
        if ( *fmt == '\0' ) {
            break;
        }

        // If it's not a conversion, it must match exactly. Otherwise, move onto conversion handling.

        if ( *fmt != '%' ) {
            if ( src >= end ) {
                break;
            }
            if ( *fmt++ != *src++ ) {
                break;
            }
            continue;
        }
        ++fmt;

        // Flags

        suppress = 0;
        alt      = 0;
        storePtr = 0;
        for ( ;; ) {
            c = *fmt;
            if (      c == '*' ) {
                suppress = 1;
            } else if ( c == '#' ) {
                alt     += 1;
            } else if ( c == '&' ) {
                storePtr = 1;
            } else {
                break;
            }
            ++fmt;
        }

        // Field width. If none, use INT_MAX to simplify no-width vs width cases.

        if ( isdigit( *fmt ) ) {
            fieldWidth = 0;
            do {
                fieldWidth = ( fieldWidth * 10 ) + ( *fmt++ - '0' );

            }   while ( isdigit( *fmt ) );
        } else if ( *fmt == '.' ) {
            ++fmt;
            fieldWidth = va_arg( inArgs, int );
            if ( fieldWidth < 0 ) {
                goto exit;
            }
        } else {
            fieldWidth = INT_MAX;
        }

        // Size modifier. Note: converts double-char (e.g. hh) into unique char (e.g. H) for easier processing.

        c = *fmt;
        switch ( c ) {
            case 'h':
                if ( *( ++fmt ) == 'h' ) {
                    sizeModifier = 'H';    // hh for char * / unsigned char *
                    ++fmt;
                } else {
                    sizeModifier = 'h';    // h  for short * / unsigned short *
                }
                break;

            case 'l':
                if ( *( ++fmt ) == 'l' ) {
                    sizeModifier = 'L';    // ll for long long * / unsigned long long *
                    ++fmt;
                } else {
                    sizeModifier = 'l';    // l  for long * / unsigned long *
                }
                break;

            case 'j':   // j for intmax_t * / uintmax_t *
            case 'z':   // z for size_t *
            case 't':   // t for ptrdiff_t *
                sizeModifier = c;
                ++fmt;
                break;

            default:
                sizeModifier = 0;
                break;
        }
        if ( *fmt == '\0' ) {
            break;
        }

        // Conversions

        switch ( *fmt++ ) {
            case 'd':   // %d: Signed decimal integer.
                base = 10;
                break;

            case 'u':   // %u: Unsigned decimal integer.
                base = 10;
                break;

            case 'p':   // %x/%X/%p: Hexidecimal integer.
                if ( sizeModifier == 0 ) {
                    sizeModifier = 'p';
                }
            case 'x':
            case 'X':
                base = 16;
                break;

            case 'o':   // %o: Octal integer.
                base = 8;
                break;

            case 'i':   // %i: Integer using an optional prefix to determine the base (e.g. 10, 0xA, 012, 0b1010 for decimal 10).
                base = 0;
                break;

            case 'b':   // %b: Binary integer.
                base = 2;
                break;

            case 'c':   // %c: 1 or more characters.

                if ( sizeModifier != 0 ) {
                    goto exit;
                }
                if ( storePtr ) {
                    len = (size_t)( end - src );
                    if ( len > (size_t) fieldWidth ) {
                        len = (size_t) fieldWidth;
                    }
                    if ( suppress ) {
                        src += len;
                        continue;
                    }

                    ptrArg = va_arg( inArgs, const unsigned char ** );
                    if ( ptrArg ) {
                        *ptrArg = src;
                    }

                    sizeArg = va_arg( inArgs, size_t * );
                    if ( sizeArg ) {
                        *sizeArg = len;
                    }

                    src += len;
                } else {
                    if ( fieldWidth == INT_MAX ) {
                        fieldWidth = 1;
                    }
                    if ( ( end - src ) < fieldWidth ) {
                        goto exit;
                    }
                    if ( suppress )                      {
                        src += fieldWidth;
                        continue;
                    }

                    s = va_arg( inArgs, unsigned char * );
                    if ( !s ) {
                        goto exit;
                    }

                    while ( fieldWidth-- > 0 ) {
                        *s++ = *src++;
                    }
                }
                ++matched;
                continue;

            case 's':   // %s: string of non-whitespace characters with a null terminator.

                if ( sizeModifier != 0 ) {
                    goto exit;
                }

                // Skip leading white space first since fieldWidth does not include leading whitespace.

                while ( ( src < end ) && isspace( *src ) ) {
                    ++src;
                }
                if ( !alt && ( ( src >= end ) || ( *src == '\0' ) ) ) {
                    goto exit;
                }

                // Copy the string until a null terminator, whitespace, or the max fieldWidth is hit.

                if ( suppress ) {
                    while ( ( src < end ) && ( *src != '\0' ) && !isspace( *src ) && ( fieldWidth-- > 0 ) ) {
                        ++src;
                    }
                } else if ( storePtr ) {
                    old = src;
                    while ( ( src < end ) && ( *src != '\0' ) && !isspace( *src ) && ( fieldWidth-- > 0 ) ) {
                        ++src;
                    }

                    ptrArg = va_arg( inArgs, const unsigned char ** );
                    if ( ptrArg ) {
                        *ptrArg = old;
                    }

                    sizeArg = va_arg( inArgs, size_t * );
                    if ( sizeArg ) {
                        *sizeArg = (size_t)( src - old );
                    }

                    ++matched;
                } else {
                    s = va_arg( inArgs, unsigned char * );
                    if ( !s ) {
                        goto exit;
                    }

                    while ( ( src < end ) && ( *src != '\0' ) && !isspace( *src ) && ( fieldWidth-- > 0 ) ) {
                        *s++ = *src++;
                    }
                    *s = '\0';

                    ++matched;
                }
                continue;

            case '[':   // %[: Match a scanset (set between brackets or the compliment set if it starts with ^).

                if ( sizeModifier != 0 ) {
                    goto exit;
                }

                notSet = ( *fmt == '^' );   // A scanlist starting with ^ matches all characters not in the scanlist.
                if ( notSet ) {
                    ++fmt;
                }
                setStart = fmt;
                if ( *fmt == ']' ) {
                    ++fmt;    // A scanlist (after a potential ^) starting with ] includes ] in the set.
                }

                // Find the end of the scanlist.

                while ( ( *fmt != '\0' ) && ( *fmt != ']' ) ) {
                    ++fmt;
                }
                if ( *fmt == '\0' ) {
                    goto exit;
                }
                setEnd = fmt++;

                // Parse until a mismatch, null terminator, or the max fieldWidth is hit.

                old = src;
                if ( notSet ) {
                    while ( ( src < end ) && ( *src != '\0' ) && ( fieldWidth-- > 0 ) ) {
                        c = *src;
                        for ( set = setStart; ( set < setEnd ) && ( *set != c ); ++set ) {}
                        if ( set < setEnd ) {
                            break;
                        }
                        ++src;
                    }
                } else {
                    while ( ( src < end ) && ( *src != '\0' ) && ( fieldWidth-- > 0 ) ) {
                        c = *src;
                        for ( set = setStart; ( set < setEnd ) && ( *set != c ); ++set ) {}
                        if ( set >= setEnd ) {
                            break;
                        }
                        ++src;
                    }
                }
                if ( ( old == src ) && !alt ) {
                    goto exit;
                }
                if ( !suppress ) {
                    if ( storePtr ) {
                        ptrArg = va_arg( inArgs, const unsigned char ** );
                        if ( ptrArg ) {
                            *ptrArg = old;
                        }

                        sizeArg = va_arg( inArgs, size_t * );
                        if ( sizeArg ) {
                            *sizeArg = (size_t)( src - old );
                        }
                    } else {
                        s = va_arg( inArgs, unsigned char * );
                        if ( !s ) {
                            goto exit;
                        }

                        while ( old < src ) {
                            *s++ = *old++;
                        }
                        *s = '\0';
                    }
                    ++matched;
                }
                continue;

            case '%':   // %%: Match a literal % character.

                if ( sizeModifier != 0 ) {
                    goto exit;
                }
                if ( fieldWidth != INT_MAX ) {
                    goto exit;
                }
                if ( suppress ) {
                    goto exit;
                }
                if ( src >= end ) {
                    goto exit;
                }
                if ( *src++ != '%' ) {
                    goto exit;
                }
                continue;

            case 'n':   // %n: Return the number of characters read so far.

                if ( sizeModifier != 0 ) {
                    goto exit;
                }
                if ( fieldWidth != INT_MAX ) {
                    goto exit;
                }
                if ( suppress ) {
                    goto exit;
                }

                i = va_arg( inArgs, int * );
                if ( !i ) {
                    goto exit;
                }

                *i = (int)( src - ( (const unsigned char *) inString ) );
                continue;

            default:    // Unknown conversion.
                goto exit;
        }

        // Number conversion. Skip leading white space since number conversions ignore leading white space.

        while ( ( src < end ) && isspace( *src ) ) {
            ++src;
        }

        // Handle +/- prefix for negative/positive (even for unsigned numbers).

        negative = 0;
        if ( ( ( end - src ) > 1 ) && ( fieldWidth > 0 ) ) {
            if ( src[ 0 ] == '-' ) {
                negative = 1;
                ++src;
                --fieldWidth;
            } else if ( src[ 0 ] == '+' ) {
                ++src;
                --fieldWidth;
            }
        }

        // Detect the base for base 0 and skip valid prefixes.

        old = src;
        if ( base == 0 ) {
            if ( ( ( end - src ) > 2 ) && ( fieldWidth >= 2 ) &&
                 ( src[ 0 ] == '0' ) && ( tolower( src[ 1 ] ) == 'x' ) && isxdigit( src[ 2 ] ) ) {
                base         = 16;
                src         +=  2;
                fieldWidth  -=  2;
            } else if ( ( ( end - src ) > 2 ) && ( fieldWidth >= 2 ) &&
                        ( src[ 0 ] == '0' ) && ( tolower( src[ 1 ] ) == 'b' ) &&
                        ( ( src[ 2 ] == '0' ) || ( src[ 2 ] == '1' ) ) ) {
                base         = 2;
                src         += 2;
                fieldWidth  -= 2;
            } else if ( ( ( end - src ) > 1 ) && ( fieldWidth >= 1 ) &&
                        ( src[ 0 ] == '0' ) && ( src[ 1 ] >= '0' ) && ( src[ 1 ] <= '7' ) ) {
                base         = 8;
                src         += 1;
                fieldWidth  -= 1;
            } else {
                base = 10;
            }
        } else if ( ( base == 16 ) && ( ( end - src ) >= 2 ) && ( fieldWidth >= 2 ) &&
                    ( src[ 0 ] == '0' ) && ( tolower( src[ 1 ] ) == 'x' ) ) {
            src         += 2;
            fieldWidth  -= 2;
        } else if ( ( base == 2 ) && ( ( end - src ) >= 2 ) && ( fieldWidth >= 2 ) &&
                    ( src[ 0 ] == '0' ) && ( tolower( src[ 1 ] ) == 'b' ) ) {
            src         += 2;
            fieldWidth  -= 2;
        }

        // Convert the string to a number.

        x = 0;
        while ( ( src < end ) && ( fieldWidth-- > 0 ) ) {
            c = *src;
            if (      isdigit(  c ) ) {
                v = c - '0';
            } else if ( isxdigit( c ) ) {
                v = 10 + ( tolower( c ) - 'a' );
            } else {
                break;
            }
            if ( v >= base ) {
                break;
            }

            x = ( x * base ) + v;
            ++src;
        }
        if ( src == old ) {
            goto exit;
        }
        if ( suppress ) {
            continue;
        }
        if ( negative ) {
            x = -x;
        }

        // Store the result.

        p = va_arg( inArgs, void * );
        if ( !p ) {
            goto exit;
        }

        switch ( sizeModifier ) {
            case   0:
                *( (int *) p ) = (int)                  x;
                break;
            case 'l':
                *( (long *) p ) = (long)                 x;
                break;
            case 'H':
                *( (char *) p ) = (char)                 x;
                break;
            case 'h':
                *( (short *) p ) = (short)                x;
                break;
            case 'L':
                *( (int64_t *) p ) =                        x;
                break;
            case 'j':
                *( (intmax_t *) p ) = (intmax_t)             x;
                break;
            case 'z':
                *( (size_t *) p ) = (size_t)               x;
                break;
            case 't':
                *( (ptrdiff_t *) p ) = (ptrdiff_t)            x;
                break;
            case 'p':
                *( (void **) p ) = (void *)( (uintptr_t)  x );
                break;

            default:    // Unknown size modifier.
                goto exit;
        }
        ++matched;
    }

exit:
    return ( matched );
}

//===========================================================================================================================
//  strnicmp_suffix
//
//  Like strnicmp routine, but only returns 0 if the entire suffix matches.
//===========================================================================================================================

int strnicmp_suffix( const void *inStr, size_t inMaxLen, const char *inSuffix )
{
    const char         *stringPtr;
    size_t              stringLen;
    size_t              suffixLen;

    stringPtr = (const char *) inStr;
    stringLen = strnlen( stringPtr, inMaxLen );
    suffixLen = strlen( inSuffix );
    if ( suffixLen <= stringLen ) {
        return ( strnicmpx( stringPtr + ( stringLen - suffixLen ), suffixLen, inSuffix ) );
    }
    return ( -1 );
}


//===========================================================================================================================
//  strnstr_suffix
//
//  Like the ANSI C strstr routine, but performs a case-insensitive compare.
//===========================================================================================================================

char *strnstr_suffix( const char *inStr, size_t inMaxLen, const char *inSuffix)
{
    size_t              stringLen;
    size_t              suffixLen;
    char               *instr_tmp;
    char               *inSuffix_tmp;
    size_t              i;
    char               *ret;

    suffixLen = strlen( inSuffix );
    stringLen = strnlen( inStr, inMaxLen );
    instr_tmp = calloc(stringLen + 1, 1);
    inSuffix_tmp = calloc(suffixLen + 1, 1);

    for (i = 0; i < stringLen; i++) {
        instr_tmp[i] = tolower(inStr[i]);
    }
    for (i = 0; i < suffixLen; i++) {
        inSuffix_tmp[i] = tolower(inSuffix[i]);
    }

    if ( suffixLen <= stringLen ) {
        ret = strstr( instr_tmp, inSuffix_tmp );
        if (ret != NULL) {
            ret = (char *)(inStr + (ret - instr_tmp));
        }
    } else {
        ret = NULL;
    }

    free(instr_tmp);
    free(inSuffix_tmp);

    return ret;
}

//#ifndef __GNUC__
void *memmem(void *start, unsigned int s_len, void *find, unsigned int f_len)
{
    char                *p, *q;
    unsigned int        len;

    p = start, q = find;
    len = 0;
    while ((p - (char *)start + f_len) <= s_len) {
        while (*p++ == *q++) {
            len++;
            if (len == f_len) {
                return (p - f_len);
            }
        };
        q = find;
        len = 0;
    };

    return (NULL);
}
//#endif


static uint8_t string_to_generic( const char *string, uint8_t str_length,  uint32_t *value_out, uint8_t is_unsigned,
                                  uint8_t is_hex )
{
    uint8_t nibble;
    uint8_t characters_processed = 0;

    if ( string == NULL ) {
        return 0;
    }

    *value_out = 0;

    while ( ( characters_processed != str_length ) &&
            ( 0 == hexchar_to_nibble( *string, &nibble ) ) &&
            ( ( is_hex != 0 ) || ( nibble < 10 ) )
          ) {
        if ( is_hex != 0 ) {
            if ( ( ( *value_out > ( 0x7fffffff >> 4 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff >> 4 ) )
               ) {
                break;
            }
            *value_out = ( *value_out << 4 ) + nibble;
        } else {
            if ( ( ( *value_out > ( 0x7fffffff / 10 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff / 10 ) )
               ) {
                break;
            }
            *value_out = ( *value_out * 10 ) + nibble;
        }
        string++;
        characters_processed++;
    }

    return characters_processed;
}

/*!
 ******************************************************************************
 * Convert a decimal or hexidecimal string to an integer.
 *
 * @param[in] str  The string containing the value.
 *
 * @return    The value represented by the string.
 */
uint32_t generic_string_to_unsigned( const char *str )
{
    uint32_t val = 0;
    uint8_t is_hex = 0;

    if ( strncmp( str, "0x", 2 ) == 0 ) {
        is_hex = 1;
        str += 2;
    }

    string_to_unsigned( str, (uint8_t)strlen(str), &val, is_hex );

    return val;
}

/**
 * Converts a decimal/hexidecimal string (with optional sign) to a signed long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted (including sign).  i.e. 0 = error
 *
 */
uint8_t string_to_signed( const char *string, uint8_t str_length, int32_t *value_out, uint8_t is_hex )
{
    uint8_t characters_processed = 0;
    uint8_t retval;
    char    first_char;

    if ( string == NULL ) {
        return 0;
    }

    first_char = *string;

    if ( ( first_char == '-' ) || ( *string == '+' ) ) {
        characters_processed++;
        string++;
        str_length--;
    }

    retval = string_to_generic( string, str_length, (uint32_t *)value_out, 0, is_hex );
    if ( retval == 0 ) {
        return 0;
    }

    if ( first_char == '-' ) {
        *value_out = -(*value_out);
    }
    return (uint8_t) ( characters_processed + retval );
}

/**
 * Converts a decimal/hexidecimal string to an unsigned long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted.  i.e. 0 = error
 *
 */
uint8_t string_to_unsigned( const char *string, uint8_t str_length, uint32_t *value_out, uint8_t is_hex )
{
    return string_to_generic( string, str_length,  value_out, 1, is_hex );
}

/**
 * Converts a unsigned long int to a decimal string
 *
 * @param value[in]      : The unsigned long to be converted
 * @param output[out]    : The buffer which will receive the decimal string
 * @param min_length[in] : the minimum number of characters to output (zero padding will apply if required).
 * @param max_length[in] : the maximum number of characters to output (up to 10 ). There must be space for terminating NULL.
 *
 * @note: A terminating NULL is added. Wnsure that there is space in the buffer for this.
 *
 * @return the number of characters returned (excluding terminating null)
 *
 */
uint8_t unsigned_to_decimal_string( uint32_t value, char *output, uint8_t min_length, uint8_t max_length )
{
    uint8_t digits_left;
    char buffer[ 10 ] = "0000000000";
    max_length = (uint8_t) MIN( max_length, sizeof( buffer ) );
    digits_left = max_length;
    while ( ( value != 0 ) && ( digits_left != 0 ) ) {
        --digits_left;
        buffer[ digits_left ] = (char) (( value % 10 ) + '0');
        value = value / 10;
    }

    digits_left = (uint8_t) MIN( ( max_length - min_length ), digits_left );
    memcpy( output, &buffer[ digits_left ], (size_t)( max_length - digits_left ) );

    /* Add terminating null */
    output[( max_length - digits_left )] = '\x00';

    return (uint8_t) ( max_length - digits_left );
}

/**
 * Converts a signed long int to a decimal string
 *
 * @param value[in]      : The signed long to be converted
 * @param output[out]    : The buffer which will receive the decimal string
 * @param min_length[in] : the minimum number of characters to output (zero padding will apply if required)
 * @param max_length[in] : the maximum number of characters to output (up to 10 ). There must be space for terminating NULL.
 *
 * @note: A terminating NULL is added. Wnsure that there is space in the buffer for this.
 *
 * @return the number of characters returned.
 *
 */
uint8_t signed_to_decimal_string( int32_t value, char *output, uint8_t min_length, uint8_t max_length )
{
    uint8_t retval = 0;
    if ( ( value < 0 ) && ( max_length > 0 ) ) {
        *output = '-';
        output++;
        max_length--;
        value = -value;
        retval++;
    }
    retval = (uint8_t) ( retval + unsigned_to_decimal_string( (uint32_t)value, output, min_length, max_length ) );
    return retval;
}

/**
 * Converts a unsigned long int to a hexidecimal string
 *
 * @param value[in]      : The unsigned long to be converted
 * @param output[out]    : The buffer which will receive the hexidecimal string
 * @param min_length[in] : the minimum number of characters to output (zero padding will apply if required)
 * @param max_length[in] : the maximum number of characters to output (up to 8 ) There must be space for terminating NULL.
 *
 * @note: A terminating NULL is added. Wnsure that there is space in the buffer for this.
 * @note: No leading '0x' is added.
 *
 * @return the number of characters returned.
 *
 */
uint8_t unsigned_to_hex_string( uint32_t value, char *output, uint8_t min_length, uint8_t max_length )
{
    uint8_t digits_left;
    char buffer[ 8 ] = "00000000";
    max_length = (uint8_t) MIN( max_length, sizeof( buffer ) );
    digits_left = max_length;
    while ( ( value != 0 ) && ( digits_left != 0 ) ) {
        --digits_left;
        buffer[ digits_left ] = nibble_to_hexchar( value & 0x0000000F );
        value = value >> 4;
    }

    digits_left = (uint8_t) MIN( ( max_length - min_length ), digits_left );
    memcpy( output, &buffer[ digits_left ], (size_t)( max_length - digits_left ) );

    /* Add terminating null */
    output[( max_length - digits_left )] = '\x00';

    return (uint8_t) ( max_length - digits_left );
}

int is_digit_str( const char *str )
{
    int res = 0;
    int i = 0;

    if ( str != NULL ) {
        i = (int)strlen(str);
        res = 1;
        while ( i > 0 ) {
            if ( !IS_09(*str) ) {
                res = 0;
                break;
            }
            str++;
            i--;
        }
    }

    return res;
}

uint8_t match_string_with_wildcard_pattern( const char *string, uint32_t length, const char *pattern )
{
    uint32_t current_string_length = length;
    uint32_t temp_string_length    = 0;
    char    *current_string        = (char *)string;
    char    *current_pattern       = (char *)pattern;
    char    *temp_string           = NULL;
    char    *temp_pattern          = NULL;

    /* Iterate through string and pattern until '*' is found */
    while ( ( current_string_length != 0 ) && ( *current_pattern != '*' ) ) {
        /* Current pattern is not equal current string and current pattern isn't a wildcard character */
        if ( ( *current_pattern != *current_string ) && ( *current_pattern != '?' ) ) {
            return 0;
        }
        current_pattern++;
        current_string++;
        current_string_length--;
    }

    /* '*' is detected in pattern. Consume string until matching pattern is found */
    while ( current_string_length != 0 ) {
        switch ( *current_pattern ) {
            case '*':
                if ( *(++current_pattern) == '\0' ) {
                    /* Last character in the pattern is '*'. Return successful */
                    return 1;
                }

                /* Store temp variables for starting another matching iteration when non-matching character is found. */
                temp_pattern       = current_pattern;
                temp_string_length = current_string_length - 1;
                temp_string        = current_string + 1;
                break;

            case '?':
                current_pattern++;
                current_string++;
                current_string_length--;
                break;

            default:
                if ( *current_pattern == *current_string ) {
                    current_pattern++;
                    current_string++;
                    current_string_length--;
                } else {
                    current_pattern       = temp_pattern;
                    current_string        = temp_string++;
                    current_string_length = temp_string_length--;
                }
                break;
        }
    }

    while ( *current_pattern == '*' ) {
        current_pattern++;
    }

    return ( *current_pattern == '\0' );
}

/*
 ******************************************************************************
 * Length limited version of strstr. Ported from bcmutils.c
 *
 * @param     arg  The string to be searched.
 * @param     arg  The length of the string to be searched.
 * @param     arg  The string to be found.
 * @param     arg  The length of the string to be found.
 *
* @return    pointer to the found string if search successful, otherwise NULL
 */
char *strnstr(const char *s, uint16_t s_len, const char *substr, uint16_t substr_len)
{
    for (; s_len >= substr_len; s++, s_len--) {
        if (strncmp(s, substr, substr_len) == 0) {
            return (char *)s;
        }
    }

    return NULL;
}



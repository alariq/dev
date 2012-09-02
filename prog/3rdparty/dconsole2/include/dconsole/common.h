#ifndef __COMMON_H__
#define __COMMON_H__

/* 
 * Console implementation example.
 * alariq@gmail.com
 * Distributed under terms of the LGPL.
 */

#if defined(WIN32) || defined(_WIN32)
#	undef __WIN32__
#	define __WIN32__	1
//#pragma message ("Compiling for Windows")
#endif

#ifdef DLL_BUILD
#ifndef DECLSPEC
#	if defined(__WIN32__)
#		define DECLSPEC	__declspec(dllexport)
#	else
#		define DECLSPEC	
#	endif
#endif
#else
#	define 	DECLSPEC
#endif

#ifdef DLL_BUILD
#ifndef SDLCALL
#	if defined(__WIN32__) && !defined(__GNUC__)
#		define DCONSOLECALL __cdecl
#	else
#		define DCONSOLECALL
#	endif
#endif /* SDLCALL */
#else
#	define	DCONSOLECALL
#endif


#if defined(__WIN32__)
    #define myitoa _itoa_s
	#define mystricmp _stricmp
    #ifndef _CRT_SECURE_NO_WARNINGS
    #	define _CRT_SECURE_NO_WARNINGS
    #endif
    #ifndef _CRT_SECURE_NO_DEPRECATE
    #	define _CRT_SECURE_NO_DEPRECATE
    #endif
    #ifndef _CRT_NONSTDC_NO_DEPRECATE
    #	define _CRT_NONSTDC_NO_DEPRECATE
    #endif
#else
    char *  myitoa( int value, char * str, int base );
    int mystricmp(const char* str1, const char* str2);
#endif

#include <string>


struct Color {
	unsigned char c[4];
	Color(char r, char g, char b, char a)
	{
		c[2] = b;
		c[1] = g;
		c[0] = r;
		c[3] = a;
	}
	Color(unsigned int color)
	{
		//memcpy(c, &color,4);
		c[2] = 0x000000FF & color;
		c[1] = (0x0000FF00 & color)>>8;
		c[0] = (0x00FF0000 & color)>>16;
		c[3] = (0xFF000000 & color)>>24;
	}
	operator unsigned char*() { return c;}
};

#define MEM_M_Delete	delete
#define MEM_M_New		new

#define TRACE_LogInfo(a,b)	puts(b)


typedef std::string	MyString;

#ifndef min
template <typename T>
inline T min(T a, T b)
{
	return a < b ? a : b;
}
#endif

template <typename T>
inline T clamp(T x, T a, T b)
{
	T t = x < a ? a : x;
	return  t > b ? b : t;
}


#define pnull 0



template<typename T>
bool internal_strmatch(const T *pstr, const T *pwildcard)
{
    const T	*cp = pnull, *mp = pnull;

    while ((*pstr) && (*pwildcard != (T)'*'))
    {
        if ((*pwildcard != *pstr) && (*pwildcard != (T)'?'))
            return false;

        ++pwildcard;
        ++pstr;
    }

    while (*pstr)
    {
        if (*pwildcard == (T)'*')
        {
            if (!* ++pwildcard)
                return true;

            mp = pwildcard;
            cp = pstr + 1;
        }
        else if ((*pwildcard == *pstr) || (*pwildcard == (T)'?'))
        {
            ++pwildcard;
            ++pstr;
        }
        else
        {
            pwildcard = mp;
            pstr = cp++;
        }
    }

    while (*pwildcard == (T)'*')
        ++pwildcard;

    return !*pwildcard;
}

inline bool strmatch(const char* pstr, const char* pwildcard)
{
    return internal_strmatch<char>(pstr, pwildcard) ;
}

inline bool wcsmatch(const wchar_t* pstr, const wchar_t* pwildcard)
{
    return internal_strmatch<wchar_t>(pstr, pwildcard) ;
}



#endif //  _COMMON_H_
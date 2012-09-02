#include "dconsole/base_command.h"
#include <assert.h>

#ifndef __WIN32__
    #include <ctype.h> // isdigit
#endif

namespace dconsole {

bool BaseCmd::Variant::isOfTypeFloat(const char *String)
{
        float number = 0;
        float frac_number = 0;
        int exponent = 0;
        int negative = 0;

        const char* p = String;
                        
        int num_digits = 0;
        int num_decimals = 0;

        // Skip leading whitespace
        while ((*p) == ' ')
            p++;

        // Handle special cases: nan and inf (see FtoA())
        if ((p[0] == (char)'n') && (p[1] == (char)'a') && (p[2] == (char)'n'))
        {
            return false;
        }
        else if ((p[0] == (char)'i') && (p[1] == (char)'n') && (p[2] == (char)'f'))
        {
            return false;
        }

        // Handle optional sign
        negative = 0;
        switch (*p) 
        {             
            case '-': negative = 1; // Fall through to increment position
            case '+': p++;
        }

		
        // Process string of digits
        while (isdigit(*p))
        {
            number = (number * (float)10) + ((float)*p - (float)'0');
            p++;
            num_digits++;
        }

        // Process decimal part
        if (*p == '.') 
        {
            p++;

            while (isdigit(*p))
            {
                frac_number = (frac_number) * (float)10 + ((float)*p - (float)'0');
                p++;
                num_digits++;
                num_decimals++;
            }

            exponent = num_decimals;
        }

        return num_decimals !=0 || num_digits!=0;
}

bool BaseCmd::Variant::isOfTypeInt(const char *String)
{
    // Skip spaces
    while(*String == ' ')
        String++;

    if(*String == '-')
    {
        String++;
    }

    while(isdigit(*String))
    {
        String++;
    };
    // are all chars were digits?
    return *String == '\0';
}

bool BaseCmd::Variant::isOfTypeBool(const char *String)
{
    assert(String);
    MyString t(String);
    if(mystricmp(String, "false")==0 || mystricmp(String, "true")==0)
        return true;
    return false;
}

}
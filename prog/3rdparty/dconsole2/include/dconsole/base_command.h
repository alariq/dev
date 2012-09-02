#ifndef __IO_CONSOLE_BASE_COMMAND_H__
#define __IO_CONSOLE_BASE_COMMAND_H__

#include <strstream>   // for sstream
#include <sstream>
#include <vector> 
#include <string>
#include "dconsole/common.h"


namespace dconsole {

class Console;

class BaseCmd {
public:
    // Variant class to represent command line parameters
    class Variant {
        int m_i;
        float m_f;
        MyString m_str;
        enum eType { T_INT, T_FLOAT, T_STRING };
        eType type;

        MyString toString(const int i) const
        {
            char buf[64]; 
            myitoa(i, buf, 10);
            return MyString(buf);
        }
        MyString toString(const float f) const 
        {
            std::stringstream inout; 
            std::string str;
            inout<<f;
            inout>>str;
            return MyString(str.c_str());
        }
        int toInt(const MyString& str) const 
        {
            float f;
			std::stringstream inout; 
            inout<<str;
            inout>>f;
            return (int)f;
        }
        float toFloat(const MyString& str) const 
        {
            float f;
			std::stringstream inout;
            inout<<str;
			inout>>f;
            return f;
        }
        bool toBool(const MyString& str) const 
        {
            MyString t = str;
			if(mystricmp(str.c_str(),"true")==0)
                return true;
            return false;
        }
		static bool isOfTypeInt(const char *String);
        static bool isOfTypeFloat(const char *String);
        static bool isOfTypeBool(const char *String);
    public:
        

        // constructors
        explicit Variant(int i):m_i(i),m_f(0.0f) { type = T_INT; }
        explicit Variant(float f):m_i(0), m_f(f) { type = T_FLOAT; }
        explicit Variant(MyString str):m_i(0),m_f(0.0f) { m_str = str; type = T_STRING; }

        // cast operators 
        operator int () const 
        {
            return type == T_INT ? m_i : (type == T_FLOAT ? (int)m_f :  toInt(m_str) );
        }

        operator unsigned int () const 
        {
            return (unsigned int) (operator int());
        }

        operator long () const 
        {
            return type == T_INT ? (long)m_i : (type == T_FLOAT ? (long)m_f :  (long)toInt(m_str) );
        }
        operator unsigned long () const 
        {
            return (unsigned long)(operator long());
        }
        operator float () const 
        {
            return type == T_INT ? (float)m_i : (type == T_FLOAT ? m_f :  toFloat(m_str) );
        }

        operator MyString () const 
        {
            return type == T_INT ? toString(m_i) : (type == T_FLOAT ? toString(m_f) :  m_str );
        }
        operator const char* () const 
        {
            return ( type == T_INT ? toString(m_i) : (type == T_FLOAT ? toString(m_f) :  m_str ) ).c_str();
        }
        operator bool () const 
        {
            return type == T_INT ? 0!=m_i : (type == T_FLOAT ? 0.0f!=m_f :  0!=toBool(m_str) );
        }

        // convertation checks
        template<typename T>
        bool canBeConvertedTo() const; 

         // type check
        template<typename T> bool is() const
        {
            return canBeConvertedTo<T>();
        }
    };
    //-------------------------------------------------------------------------------------------------

    template<typename T>
    class Type2String {
    public:
        MyString operator()();
        static MyString name();
    };

protected:
    const MyString m_name;
    const MyString m_description;

    BaseCmd(const MyString& name, const MyString desc):
    m_name(name), m_description(desc)
    {
    }
    virtual bool checkArguments(const std::vector<BaseCmd::Variant>& params) = 0;

public:
    const MyString getName() const { return m_name; }
    const MyString getDescription() const { return m_description; }
    virtual const MyString getUsage() const = 0;
    virtual bool runCommand(const std::vector<BaseCmd::Variant>& params, Console& console) = 0;
    virtual ~BaseCmd() {};

};


// i can use macros for this, but i will not
template<> class BaseCmd::Type2String<bool> { public: static MyString name() { return MyString("BOOL"); } static const int LEN = sizeof("BOOL"); };
template<> class BaseCmd::Type2String<float> {  public: static MyString name() { return MyString("FLOAT"); } static const int LEN = sizeof("FLOAT"); };
template<> class BaseCmd::Type2String<int> {  public: static MyString name() { return MyString("INT"); }  static const int LEN = sizeof("INT"); };
template<> class BaseCmd::Type2String<unsigned int> {  public: static MyString name() { return MyString("INT"); }  static const int LEN = sizeof("INT"); };
template<> class BaseCmd::Type2String<long> { public: static MyString name() { return MyString("LONG"); } static const int LEN = sizeof("LONG"); };
template<> class BaseCmd::Type2String<unsigned long> { public: static MyString name() { return MyString("LONG"); } static const int LEN = sizeof("LONG"); };
template<> class BaseCmd::Type2String<MyString> { public: static MyString name() { return MyString("STRING"); } static const int LEN = sizeof("STRING"); };
template<> class BaseCmd::Type2String<const char*> { public: static MyString name() { return MyString("STRING"); } static const int LEN = sizeof("STRING"); };
template<> class BaseCmd::Type2String<char*> { public: static MyString name() { return MyString("STRING"); } static const int LEN = sizeof("STRING"); };
template<> class BaseCmd::Type2String<char> { public: static MyString name() { return MyString("CHAR"); } static const int LEN = sizeof("CHAR"); };
template<> class BaseCmd::Type2String<void> { public: static MyString name() { return MyString("VOID"); } static const int LEN = sizeof("VOID"); };

template <typename T>
class TypeTraits;

};


#if defined(__WIN32__)
#undef myitoa 
#endif



#endif //__IO_CONSOLE_BASE_COMMAND_H__
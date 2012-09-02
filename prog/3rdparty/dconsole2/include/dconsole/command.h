#ifndef __IO_CONSOLE_COMMAND_H__
#define __IO_CONSOLE_COMMAND_H__

/* 
 * Console implementation example.
 * alariq@gmail.com
 * Distributed under terms of the LGPL.
 */

#include <strstream>   // for sstream
#include <sstream>
#include <vector>

#include "dconsole/base_command.h"

namespace dconsole {

//sorry for that
#define ADD_COMMON(t) template<typename U>    struct is_ ## t        { enum { value = false }; };      
#define ADD_ENUM(t)   enum { e_is_ ## t = is_ ## t <T>::value };
#define ADD_SPEC(t)   template<>              struct BaseTypeTraits::is_ ## t < t >   { enum { value = true };  }; 

#define ADD_COMMON2(qualifier, t) template<typename U>    struct is_ ## qualifier ## t        { enum { value = false }; };
#define ADD_ENUM2(qualifier, t)   enum { e_is_ ## qualifier ## t = is_ ## qualifier ## t <T>::value };
#define ADD_SPEC2(qualifier, t)   template<>              struct BaseTypeTraits::is_ ## qualifier ## t < qualifier t  >   { enum { value = true };  }; 

class BaseTypeTraits {
protected:
    ADD_COMMON(bool)
    ADD_COMMON(int)
    ADD_COMMON(char)
    ADD_COMMON(wchar_t)
    ADD_COMMON(short)
    ADD_COMMON(long)

    ADD_COMMON(float)
    ADD_COMMON(double)

    ADD_COMMON2(unsigned, int)
    ADD_COMMON2(unsigned, char)
    ADD_COMMON2(unsigned, short)
    ADD_COMMON2(unsigned, long)
};

ADD_SPEC(bool)
ADD_SPEC(int)
ADD_SPEC(char)
ADD_SPEC(wchar_t)
ADD_SPEC(short)
ADD_SPEC(long)

ADD_SPEC(float)
ADD_SPEC(double)

ADD_SPEC2(unsigned, int)
ADD_SPEC2(unsigned, char)
ADD_SPEC2(unsigned, short)
ADD_SPEC2(unsigned, long)

template <typename T>
class TypeTraits: public BaseTypeTraits {

    ADD_ENUM(bool)
    ADD_ENUM(int)
    ADD_ENUM(char)
    ADD_ENUM(wchar_t)
    ADD_ENUM(short)
    ADD_ENUM(long)

    ADD_ENUM(float)
    ADD_ENUM(double)

    ADD_ENUM2(unsigned, int)
    ADD_ENUM2(unsigned, char)
    ADD_ENUM2(unsigned, short)
    ADD_ENUM2(unsigned, long)

public:
    enum { isSignedIntegral = is_bool<T>::value || is_int<T>::value || is_char<T>::value || is_wchar_t<T>::value || is_short<T>::value || is_long<T>::value };
    enum { isUnignedIntegral = is_unsignedint<T>::value || is_unsignedchar<T>::value || is_unsignedshort<T>::value || is_unsignedlong<T>::value };

    enum { isIntegral = isSignedIntegral || isUnignedIntegral }; 
    enum { isFundamental = isIntegral || is_float<T>::value || is_double<T>::value };
};

#undef ADD_COMMON
#undef ADD_ENUM
#undef ADD_SPEC

#undef ADD_COMMON2
#undef ADD_ENUM2
#undef ADD_SPEC2


// TypeList class
//==========================================================================================
class NullType {};

template <class T, class U> 
struct TypeList {
	typedef T Head;
	typedef U Tail;
};
// get index of type in a list
// ===========================
template < class TList, class T> struct  indexOf;
template <class T>
struct indexOf<NullType, T> {
	enum { value  = -1 };
};

template <class T, class Tail>
struct indexOf<TypeList<T, Tail>, T > {
	enum { value  = 0 };
};

template <class Head, class Tail, class T>
struct indexOf<TypeList<Head, Tail>, T > {
private:
	enum { temp = indexOf<Tail, T>::value };
public:
	enum { value  = temp == -1 ? -1 : 1 + temp };
};

// end of TypeList class ======================================================================

// to distinguish types that can easily be converted to MyString
typedef TypeList<MyString, TypeList<char*, TypeList<const char*, NullType> > > StringType;

// for static assertions, see below
template <bool b> struct You_can_only_use_variable_commands_with_nonconst_fundamental_types;
template<> struct You_can_only_use_variable_commands_with_nonconst_fundamental_types<true> {};


template < class TList, class T> struct  indexOf2;
template < class TList> struct  NumEl;

template < typename R> struct  TypeConv;
template < typename R, typename T1> struct  TypeConv1;
template < typename R, typename T1, typename T2> struct  TypeConv2;
template < typename R, typename T1, typename T2, typename T3> struct  TypeConv3;
template < typename R, typename T1, typename T2, typename T3, typename T4> struct  TypeConv4;
template < typename R, typename T1, typename T2, typename T3, typename T4, typename T5> struct  TypeConv5;
template <typename R, typename T, int N> struct executor;

template <typename T> struct cmdCreator;

class NullType2 {};

template <class T, class U>
struct TypeList2: public BaseCmd {
    typedef T Head;
    typedef U Tail;
    typedef TypeList2<T, U> MyType;
    enum { myIndex = NumEl< TypeList2<T, U> >::value };

    typedef typename TypeConv< MyType >::fp myfp;
    myfp m_fptr_;

    TypeList2(std::string name, std::string desc, myfp fptr):BaseCmd(name, desc), m_fptr_(fptr) {}

    virtual bool checkArguments(const std::vector<BaseCmd::Variant>& params)
    {
        if(params.size() < myIndex)
            return false;
        return checkArguments_priv(params);
    }

    static bool checkArguments_priv(const std::vector<BaseCmd::Variant>& params, int offset = 0)
    {
        if(!params[offset].is<Tail::Head>())
            return false;
        return Tail::checkArguments_priv(params, offset+1);
    }

    static std::string get_usage()
    {
        std::string p = BaseCmd::Type2String<Tail::Head>::name();
        p.append(" ");
        p.append( Tail::get_usage() );
        return p;
    }

    const MyString getUsage() const
    {
        std::string str;
        std::string rv = dconsole::BaseCmd::Type2String<Head>::name();
        str.append("RV: ").append(rv); str.append(" ").append(getName()).append(" ");
        str.append( MyType::get_usage() ); str.append(" ");
        return str;
    }

    virtual bool runCommand(const std::vector<BaseCmd::Variant>& params, Console& console)
    {
        if( checkArguments(params) )
        {
            executor<Head, myfp, myIndex>::run(m_fptr_, params);
            return true;
        }
        return false;

    }
};


template <class R>
struct TypeList2< R, NullType2>: public BaseCmd {
    typedef R Head;
    typedef NullType2 Tail;
    typedef TypeList2<R, NullType2> MyType;
    enum { myIndex = NumEl< TypeList2<Head, Tail> >::value };

    typedef typename TypeConv< MyType >::fp myfp;
    myfp m_fptr_;

    TypeList2< R, NullType2>(std::string name, std::string desc, myfp fptr):BaseCmd(name, desc), m_fptr_(fptr) {}

    virtual bool checkArguments(const std::vector<BaseCmd::Variant>& params)
    {
        if(params.size() >= myIndex)
            return false;
        return checkArguments_priv(params, 0);
    }


    static bool checkArguments_priv(const std::vector<BaseCmd::Variant>& params, int offset = 0)
    {
        return true;
    }

    static std::string get_usage()
    {
        return ";";
    }

    const MyString getUsage() const
    {
        std::string str;
        std::string p = dconsole::BaseCmd::Type2String<Head>::name();
        str.append(getName());str.append(" ");
        str.append( p );
        return str;
    }

    virtual bool runCommand(const std::vector<BaseCmd::Variant>& params, Console& console)
    {
        executor<Head, myfp, myIndex>::run(m_fptr_, params);
        return true;
    }
};


// do we need this t all?
template <>
struct NumEl<NullType2> {
    enum { value = 0 };
};
// ?

template <class T>
struct NumEl<TypeList2<T, NullType2> > {
    enum { value  = 0 };
};

template <class Head, class Tail>
struct NumEl<TypeList2<Head, Tail> > {
private:
    enum { temp = NumEl<Tail>::value };
public:
    enum { value  = 1 + temp };
};

//=============================================================================
template <typename T>
struct TypeConv< TypeList2<T, NullType2> > {
    typedef T (*fp)(void);
};

template <typename T, typename Tp>
struct TypeConv1<  Tp, TypeList2<T, NullType2> > {
    typedef Tp (*fp)(T); 
};

template <typename T, typename Tp1, typename Tp2>
struct TypeConv2< Tp1, Tp2, TypeList2<T, NullType2> > {
    typedef Tp1 (*fp)(Tp2, T);
};

template <typename T, typename Tp1, typename Tp2, typename Tp3>
struct TypeConv3< Tp1, Tp2, Tp3, TypeList2<T, NullType2> > {
    typedef Tp1 (*fp)(Tp2, Tp3, T);
};

template <typename T, typename Tp1, typename Tp2, typename Tp3, typename Tp4>
struct TypeConv4< Tp1, Tp2, Tp3, Tp4, TypeList2<T, NullType2> > {
	typedef Tp1 (*fp)(Tp2, Tp3, Tp4, T);
};

template <typename T, typename Tp1, typename Tp2, typename Tp3, typename Tp4, typename Tp5>
struct TypeConv5< Tp1, Tp2, Tp3, Tp4, Tp5, TypeList2<T, NullType2> > {
	typedef Tp1 (*fp)(Tp2, Tp3, Tp4, Tp5, T);
};

template <class T, class U, typename Tp1, typename Tp2, typename Tp3, typename Tp4 >
struct TypeConv4<Tp1, Tp2, Tp3, Tp4, TypeList2<T, U> > {
	typedef typename    TypeConv5<
		Tp1,
		Tp2,
		Tp3,
		Tp4,
		T,
		TypeList2<typename U::Head, typename U::Tail>
	> ::fp  fp;
};


template <class T, class U, typename Tp1, typename Tp2, typename Tp3 >
struct TypeConv3<Tp1, Tp2, Tp3, TypeList2<T, U> > {
	typedef typename    TypeConv4<
		Tp1,
		Tp2,
		Tp3,
		T,
		TypeList2<typename U::Head, typename U::Tail>
	> ::fp  fp;
};

template <class T, class U, typename Tp1, typename Tp2 >
struct TypeConv2<Tp1, Tp2, TypeList2<T, U> > {
    typedef typename    TypeConv3<
        Tp1,
        Tp2,
        T,
        TypeList2<typename U::Head, typename U::Tail>
    > ::fp  fp;
};

template <class T, class U, typename Tp>
struct TypeConv1<Tp, TypeList2<T, U> > {
    typedef typename    TypeConv2<
        Tp,
        T,
        TypeList2<typename U::Head, typename U::Tail>
    > ::fp  fp;
};

template <class T, class U>
struct TypeConv<TypeList2<T, U> > {
    typedef typename    TypeConv1<  T,  
        TypeList2<typename U::Head, typename U::Tail>
    > ::fp  fp;    
};
//=============================================================================

template < typename T >  struct output_to_console;

template <>  struct output_to_console<MyString> {
    static void output(const MyString& v) {
        dconsole::Instance().addLogRecord(Console::COL_CMD_RESULT, v);
    }
};

template <>  struct output_to_console<char*> {
	static void output(const char* v) {
		dconsole::Instance().addLogRecord(Console::COL_CMD_RESULT, v);
	}
};
template <>  struct output_to_console<const char*> {
	static void output(const char* v) {
		dconsole::Instance().addLogRecord(Console::COL_CMD_RESULT, v);
	}
};

template < typename T > struct output_to_console {
        static void output(const T& v) {
        MyString str;
        std::stringstream inout;
        inout<<v;
        inout>>str;
        dconsole::Instance().addLogRecord(Console::COL_CMD_RESULT, str);
    }
};

template <typename R, typename T> struct executor<R, T, 0> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& ) {
        R retval = fp();
        output_to_console<R>::output(retval);
    }
};
template <typename T> struct executor<void, T, 0> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& ) {
        fp();
    }
};

template <typename R, typename T> struct executor<R, T, 1> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        output_to_console<R>::output( fp( vec[0] ) );
    }
};
template <typename T> struct executor<void, T, 1> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        fp( vec[0] );
    }
};

template <typename R, typename T> struct executor<R, T, 2> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        output_to_console<R>::output( fp( vec[0], vec[1] ) );
    }
};
template <typename T> struct executor<void, T, 2> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        fp( vec[0], vec[1] );
    }
};

template <typename R, typename T> struct executor<R, T, 3> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        output_to_console<R>::output( fp( vec[0], vec[1], vec[2]) );
    }
};
template <typename T> struct executor<void, T, 3> {
    static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
        fp( vec[0], vec[1], vec[2]);
    }
};

template <typename R, typename T> struct executor<R, T, 4> {
	static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
		output_to_console<R>::output( fp( vec[0], vec[1], vec[2], vec[3]) );
	}
};
template <typename T> struct executor<void, T, 4> {
	static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
		fp( vec[0], vec[1], vec[2], vec[3]);
	}
};

template <typename R, typename T> struct executor<R, T, 5> {
	static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
		output_to_console<R>::output( fp( vec[0], vec[1], vec[2], vec[3], vec[4]) );
	}
};
template <typename T> struct executor<void, T, 5> {
	static void run(T fp, const std::vector<BaseCmd::Variant>& vec) {
		fp( vec[0], vec[1], vec[2], vec[3], vec[4]);
	}
};

//=============================================================================

template <typename R> struct cmdCreator< R (*)(void) > {
    static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(void)) {
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
        return new TypeList2<R, NullType2> (name, desc, fffp);
    }
};

template <typename R, typename T> struct cmdCreator< R (*)(T) > {
    static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T)) {
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T>::isFundamental>;
        return new TypeList2< R, TypeList2<T, NullType2> > (name, desc, fffp);
    }
};

template <typename R, typename T1, typename T2> struct cmdCreator< R (*)(T1, T2) > {
    static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2)) {
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T1>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T2>::isFundamental>;
        return new TypeList2< R, TypeList2<T1, TypeList2<T2, NullType2> > > (name, desc, fffp);
    }
};

template <typename R, typename T1, typename T2, typename T3> struct cmdCreator< R (*)(T1, T2, T3) > {
    static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3)) {
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T1>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T2>::isFundamental>;
        You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T3>::isFundamental>;
        return new TypeList2< R, TypeList2<T1, TypeList2<T2, TypeList2<T3, NullType2> > > > (name, desc, fffp);
    }
};

template <typename R, typename T1, typename T2, typename T3, typename T4> struct cmdCreator< R (*)(T1, T2, T3, T4) > {
	static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3, T4)) {
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T1>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T2>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T3>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T4>::isFundamental>;
		return new TypeList2< R, TypeList2<T1, TypeList2<T2, TypeList2<T3, TypeList2<T4, NullType2> > > > > (name, desc, fffp);
	}
};

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5> struct cmdCreator< R (*)(T1, T2, T3, T4, T5) > {
	static BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3, T4, T5)) {
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<R>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T1>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T2>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T3>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T4>::isFundamental>;
		You_can_only_use_variable_commands_with_nonconst_fundamental_types<TypeTraits<T5>::isFundamental>;
		return new TypeList2< R, TypeList2<T1, TypeList2<T2, TypeList2<T3, TypeList2<T4, TypeList2<T5, NullType2> > > > > > (name, desc, fffp);
	}
};

template <typename R>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(void))
{
    typedef R (*fffp_t)();
    return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

template <typename R, typename T>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T))
{
    typedef R (*fffp_t)(T);
    return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

template <typename R, typename T1, typename T2>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2))
{
    typedef R (*fffp_t)(T1, T2);
    return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

template <typename R, typename T1, typename T2, typename T3>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3))
{
    typedef R (*fffp_t)(T1, T2, T3);
    return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

template <typename R, typename T1, typename T2, typename T3, typename T4>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3, T4))
{
	typedef R (*fffp_t)(T1, T2, T3, T4);
	return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
BaseCmd* createCmd(const std::string& name, const std::string& desc, R (*fffp)(T1, T2, T3, T4, T5))
{
	typedef R (*fffp_t)(T1, T2, T3, T4, T5);
	return cmdCreator<fffp_t>::createCmd(name, desc, fffp);
}

void test();

}

#endif //__IO_CONSOLE_COMMAND_H__
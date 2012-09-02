#include "dconsole/base_command.h"

namespace dconsole {

template<>
bool BaseCmd::Variant::canBeConvertedTo<int>() const 
{
    if(type == T_INT || type == T_FLOAT ) return true;
    return isOfTypeInt(m_str.c_str()) || isOfTypeFloat(m_str.c_str());
}
template<>
bool BaseCmd::Variant::canBeConvertedTo<float>() const 
{
    if(type == T_INT || type == T_FLOAT ) return true;
    return isOfTypeFloat(m_str.c_str()) || isOfTypeInt(m_str.c_str());
}
template<>
bool BaseCmd::Variant::canBeConvertedTo<bool>() const 
{
    if(type == T_INT || type == T_FLOAT ) return true;
    return isOfTypeBool(m_str.c_str());
}

template<> bool  BaseCmd::Variant::canBeConvertedTo<unsigned int>() const { return canBeConvertedTo<int>(); };  
template<> bool  BaseCmd::Variant::canBeConvertedTo<long>() const { return canBeConvertedTo<int>(); };  
template<> bool  BaseCmd::Variant::canBeConvertedTo<unsigned long>() const { return canBeConvertedTo<int>(); };  
// everything can be converted to string
template<> bool BaseCmd::Variant::canBeConvertedTo<MyString>() const { return true; /* :-) */ }
template<> bool BaseCmd::Variant::canBeConvertedTo<const char*>() const { return true; /* :-) */ }

}


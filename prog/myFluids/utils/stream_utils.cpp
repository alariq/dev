#include<string>

#include "stream.h"
#include "stream_utils.h"
#include <assert.h>
#include <limits.h>

namespace stream_utils {

size_t read_stdstring(const stream* pstream, std::string& str)
{
    size_t name_len = pstream->read_dword(); // 4 (*)
    if(name_len)
    {
        char* tmp = new char[name_len+1];
        pstream->read_cstr(tmp);
        tmp[name_len]='\0';
        str = tmp;
        delete[] tmp;
        return name_len + 4; // 4 - sizeof a dword, see (*)
    }
    return 0;
}

size_t write_stdstring(stream* pstream, const std::string& str)
{
    size_t counter = 0;
    size_t size = str.size();
    assert(size < UINT_MAX);
    counter += pstream->write_dword((unsigned int)size);
    if(size)
        counter += pstream->write_cstr(str.c_str());
    return counter;
}



} // namespace stream_utils

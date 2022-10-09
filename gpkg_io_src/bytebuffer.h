#pragma once

#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>

class ByteBuffer
{
public:
    template< typename T>
    ByteBuffer(const T&);

    ByteBuffer();
    ByteBuffer(const char*);
    ByteBuffer(const ByteBuffer&);

    void		clear()				{ buf_.clear(); }
    void		byte_swap(bool yn=false)	{ byteswap_ = yn; }
    std::string		toHex() const;
    void		fromHex(std::string);

    template<typename T>
    ByteBuffer&		add(const T&);

    ByteBuffer&		add(const ByteBuffer&);

    template<typename T>
    bool		get(T&, size_t&) const;

protected:
    std::vector<uint8_t>	buf_;
    bool			byteswap_ = false;
};

inline ByteBuffer::ByteBuffer()
{}

inline ByteBuffer::ByteBuffer(const char* str)
    : buf_(strlen(str))
{
    uint8_t* ptr = (uint8_t*) str;
    for (int i=0; i<buf_.size(); i++)
	buf_[i] = *ptr++;
}

template<typename T>
ByteBuffer::ByteBuffer(const T& t)
    : buf_(sizeof(T))
{
    uint8_t* ptr = (uint8_t*) &t;
    if (byteswap_)
	for (int i=buf_.size()-1; i>=0; i--)
	    buf_[i] = *ptr++;
    else
	for (int i=0; i<buf_.size(); i++)
	    buf_[i] = *ptr++;
}

inline ByteBuffer::ByteBuffer(const ByteBuffer& oth)
{
    buf_ = oth.buf_;
    byteswap_ = oth.byteswap_;
}

inline std::string ByteBuffer::toHex() const
{
    std::stringstream ss;
    ss << std::hex;
    for (int i=0; i<buf_.size(); i++)
	ss << std::setw(2) << std::setfill('0') << (int)buf_[i];

    auto str = ss.str();
    std::transform(str.begin(), str.end(), str.begin(),
		   [](unsigned char c){ return std::toupper(c);});
    return str;
}

inline void ByteBuffer::fromHex(std::string hexstr)
{
    buf_.resize(hexstr.size()/2);
    for (int i=0; i<hexstr.size(); i+=2)
	buf_[i/2] = (uint8_t) std::stoul(hexstr.substr(i,2), nullptr, 16);
}

template<typename T>
ByteBuffer& ByteBuffer::add(const T& t)
{
    return add(ByteBuffer(t));
}

inline ByteBuffer& ByteBuffer::add(const ByteBuffer& buf)
{
    buf_.insert(buf_.end(), buf.buf_.begin(), buf.buf_.end());
    return *this;
}

template<typename T>
bool ByteBuffer::get(T& t, size_t& pos) const
{
    if (pos+sizeof(T)>buf_.size())
	return false;
    T res;
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&t);
    if (byteswap_)
	for (int i=pos+sizeof(T)-1; i>=pos; i--)
	    *ptr++ = buf_[i];
    else
	for (int i=pos; i<pos+sizeof(T); i++)
	    *ptr++ = buf_[i];

    pos += sizeof(T);
    return true;
}

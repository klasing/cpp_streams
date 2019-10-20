// example 1, found at:
// https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
// example 2, found at:
// https://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/reference/streambuf.html
// example 3, found at:
// https://artofcode.wordpress.com/2010/12/12/deriving-from-stdstreambuf/
#include "pch.h"
#define _WIN32_WINNT 0x0601
#include <boost/asio/streambuf.hpp>
#include <boost/shared_array.hpp>
#include <iostream>
#include <streambuf>
#include <string>
#include <locale>
#include <cstdio>
#include <vector>

//****************************************************************************
//*                     stream_buf
//****************************************************************************
class stream_buf : public std::streambuf
{
	char* pBuf = new char[128];
	int iBuf = 0;
protected:
	// general output function, print characters in
	// uppercase mode
	virtual int_type overflow(int_type c)
	{
		if (c != EOF)
		{
			// convert lowercase to uppercase
			c = std::toupper(static_cast<char>(c), getloc());

			pBuf[iBuf++] = c;

			// and write the character to the standard output
			if (putchar(c) == EOF)
				return EOF;
		}
		return c;
	}
public:
	stream_buf()
	{
		clear_and_reset_buf();
	}
	void clear_and_reset_buf()
	{
		for (int i = 0; i < 128; ++i)
			pBuf[i] = '\0';
	}
	char* getBuf() const { return pBuf; }
};
//****************************************************************************
//*                     sock
//****************************************************************************
class sock
{
public:
	size_t send(boost::asio::streambuf::const_buffers_type data)
	{
		return data.size();
	}
	size_t receive(boost::asio::streambuf::mutable_buffers_type data)
	{
		return data.size();
	}
};
//****************************************************************************
//*                     char_array_buffer
//****************************************************************************
class char_array_buffer : public std::streambuf
{
	const char* const begin_;
	const char* const end_;
	const  char* current_;
public:
	char_array_buffer(const char* data
		, unsigned int len
	)
		: begin_(data)
		, end_(data + len)
		, current_(data)
	{}
private:
	char_array_buffer::int_type underflow()
	{
		if (current_ == end_)
			return traits_type::eof();
		return traits_type::to_int_type(*current_);
	}
	char_array_buffer::int_type uflow()
	{
		if (current_ == end_)
			return traits_type::eof();
		return traits_type::to_int_type(*current_++);

	}
	char_array_buffer::int_type pbackfail(int_type ch)
	{
		if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
			return traits_type::eof();
		return traits_type::to_int_type(*--current_);
	}
	std::streamsize showmanyc()
	{
		return end_ - current_;
	}
};
//****************************************************************************
//*                     main
//****************************************************************************
int main()
{
	std::cout << "EXAMPLE 1\n";
	// create special output buffer
	stream_buf sb;
	// initialize output stream with that output buffer
	std::ostream my_cout(&sb);

	my_cout << "31 hexadecimal: " << std::hex << 31 << std::endl;
	std::cout << sb.getBuf() << std::endl;
	sb.clear_and_reset_buf();
	my_cout << "the buffer now holds this string" << std::endl;
	std::cout << sb.getBuf() << std::endl;

	std::cout << "EXAMPLE 2 reading/writing to a simulated socket\n";
	size_t n = 0;
	sock sock_instance;
	boost::asio::streambuf b;
	std::cout << "A) reading\n";
	// reserve 512 bytes in output sequence
	boost::asio::streambuf::mutable_buffers_type bufs = b.prepare(512);
	n = sock_instance.receive(bufs);
	// received data is \"committed\" from output sequence to input sequence
	b.commit(n);
	std::istream is(&b);
	std::string s;
	is >> s;
	std::cout << s.c_str() << std::endl;

	std::cout << "B) writing\n";
	std::ostream os(&b);
	os << "Hello, World!\n";
	// try sending some data in input sequence
	n = sock_instance.send(b.data());
	// sent data is removed from input sequence
	b.consume(n);
	std::cout << n << " bytes written\n";

	// this stuff causes a crash:
	// Expression: is_block_type_valid(header->_block_use)
	//std::cout << "EXAMPLE 3 it's not yet clear what the usage is with this example\n";
	//std::string alphabet;
	//char szStr[]{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	//	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0' };
	//const unsigned int len = sizeof(szStr) / sizeof(szStr[0]);

	//boost::shared_array<char> data(szStr);
	//char_array_buffer buf(data.get(), len);
	//std::istream is(&buf);
	//is >> alphabet;
	//std::cout << alphabet.c_str() << std::endl;

	return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//*                     FilteringOutputStreamBuf
//****************************************************************************
//template <class Inserter>
//class FilteringOutputStreamBuf : public std::streambuf
//{
//private:
//	std::streambuf* myDest;
//	Inserter myInserter;
//	bool myDeleteWhenFinished;
//public:
//	FilteringOutputStreamBuf(std::streambuf* dest
//		, Inserter i
//		, bool deleteWhenFinished = false
//	)
//	{}
//	FilteringOutputStreamBuf(std::streambuf* dest
//		, bool deleteWhenFinished = false
//	)
//	{}
//	virtual ~FilteringOutputStreamBuf() {}
//	virtual int overflow(int ch) {}
//	virtual int underflow() {}
//	virtual int sync() {}
//	virtual std::streambuf* setbuf(char* p, int len) {}
//
//	inline Inserter& inserter();
//};


//////////////////////////////////////////////////////////////////////////////

//// ostream.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include "pch.h"
//#include <iostream>
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

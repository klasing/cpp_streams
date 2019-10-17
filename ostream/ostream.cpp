// example 1
// found at:
// https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html
#include "pch.h"
#include <iostream>
#include <streambuf>
#include <locale>
#include <cstdio>
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

	//std::cout << "EXAMPLE 2 A filtering streambuf for output\n";

	return EXIT_SUCCESS;
}


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

//main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// example inspired by:
// https://gcc.gnu.org/onlinedocs/libstdc++/manual/streambufs.html

#include "pch.h"
#include <iostream>
#include <string>

//****************************************************************************
//*                     output_buffer
//****************************************************************************
class output_buffer : public std::streambuf
{
	static const unsigned BUFFER_MAX = 128;
	char buffer[BUFFER_MAX] = { '\0' };
	unsigned i = 0;
public:
	char* get_buffer()
	{
		return buffer;
	}
protected:
	virtual int_type overflow(int_type c)
	{
		buffer[i++] = c;
		return c;
	}
};

//	static const unsigned BUFFER_MAX = 128;
//	char buffer[BUFFER_MAX] = { 't', 'h', 'i', 's', ' ',
//		's', 't', 'r', 'i', 'n', 'g', ' ', 'c', 'o', 'm', 'e', 's', ' ',
//		'f', 'r', 'o', 'm', ' ', 't', 'h', 'e', ' ',
//		'i', 'n', 'p', 'u', 't', ' ', 'b', 'u', 'f', 'f', 'e', 'r',
//		'\0'};
//	unsigned i = 0;

//****************************************************************************
//*                     input_buffer
//****************************************************************************
class input_buffer : public std::streambuf
{
	std::streambuf* source;
	char ch;
public:
	input_buffer() : source(std::cin.rdbuf())
	{
		setg(&ch, &ch + 1, &ch + 1);
	}
protected:
	int underflow()
	{
		int n = source->sbumpc();
		if (n != EOF)
		{
			ch = n;
			setg(&ch, &ch, &ch + 1);
		}
		return n;
	}
};

//****************************************************************************
//*                     input_stream
//****************************************************************************
class input_stream : public std::istream
{
	input_buffer ib;
public:
	input_stream() : std::istream(&ib) {}
};

//****************************************************************************
//*                     main
//****************************************************************************
int main()
{
    std::cout << "Hello World!\n";
	// create an output buffer
	output_buffer ob;
	// initialize an output stream with the output buffer
	std::ostream output_stream(&ob);
	// now everything send to the output_stream goes into the
	// output_buffer
	output_stream << "this string will end up in the output buffer" << std::endl;
	std::cout << "the output buffer now holds: " << ob.get_buffer() << std::endl;

	// check out:
	// http://www.cplusplus.com/forum/general/94912/
	// create an input stream
	input_stream is;
	char c;
	std::cout << "enter a char ";
	is >> c;
	std::cout << "c contains: " << c << std::endl;
	std::string s;
	std::cout << "enter a string ";
	is >> s;
	std::cout << "s contains: " << s.c_str() << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

// asio_tutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _WIN32_WINNT 0x0501
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>

//****************************************************************************
//*                     print
//****************************************************************************
void print(const boost::system::error_code& /*e*/)
{
	std::cout << "Hello World!\n";
}
//****************************************************************************
//*                     print_handler_with_arguments_bounded
//****************************************************************************
void print_handler_with_arguments_bounded(const boost::system::error_code& /*e*/
	, boost::asio::deadline_timer* t
	, int* count
)
{
	if (*count < 5)
	{
		std::cout << *count << std::endl;
		++(*count);
		t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
		// smells like recursion
		t->async_wait(boost::bind(print_handler_with_arguments_bounded
			, boost::asio::placeholders::error
			, t
			, count)
		);
	}
}
//****************************************************************************
//*                     printer
//****************************************************************************
class printer
{
	boost::asio::deadline_timer timer_;
	int count_;
public:
	printer(boost::asio::io_service& io)
		: timer_(io, boost::posix_time::seconds(1))
		, count_(0)
	{
		timer_.async_wait(boost::bind(&printer::print
			, this));
	}
	~printer()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}
	void print()
	{
		if (count_ < 5)
		{
			std::cout << count_ << std::endl;
			++count_;
			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
			// smells like recursion
			timer_.async_wait(boost::bind(&printer::print
				, this)
			);
		}
	}
};
//****************************************************************************
//*                     main
//****************************************************************************
int main()
{
	{
		std::cout << "TIMER.1 - Using a timer synchronously\n";
		std::cout << "waiting for 5 seconds before outputting...\n";
		boost::asio::io_service io;
		boost::asio::deadline_timer t(io
			, boost::posix_time::seconds(5)
		);
		t.wait();
		std::cout << "Hello World!\n";
	}

	{
		std::cout << "TIMER.2 - Using a timer asynchronously\n";
		std::cout << "waiting for 5 seconds before outputting...\n";
		boost::asio::io_service io;
		boost::asio::deadline_timer t(io
			, boost::posix_time::seconds(5)
		);
		t.async_wait(&print);
		io.run();
	}

	{
		std::cout << "TIMER.3 - Binding arguments to a handler\n";
		boost::asio::io_service io;
		int count = 0;
		boost::asio::deadline_timer t(io
			, boost::posix_time::seconds(1)
		);
		t.async_wait(boost::bind(print_handler_with_arguments_bounded
			, boost::asio::placeholders::error
			, &t
			, &count)
		);
		io.run();
		std::cout << "Final count is " << count << std::endl;

	}

	{
		std::cout << "TIMER.4 - Using a member function as a handler\n";
		boost::asio::io_service io;
		printer p(io);
		io.run();
	}
	return EXIT_SUCCESS;
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

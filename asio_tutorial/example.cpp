// asio_tutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
// error C4996: 'ctime': This function or variable may be unsafe.
// Consider using ctime_s instead.
#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_WINNT 0x0501
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;
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
//*                     printer_with_two_thread
//****************************************************************************
class printer_with_two_thread
{
	boost::asio::io_service::strand strand_;
	boost::asio::deadline_timer timer1_;
	boost::asio::deadline_timer timer2_;
	int count_;
public:
	printer_with_two_thread(boost::asio::io_service& io)
		: strand_(io)
		, timer1_(io, boost::posix_time::seconds(1))
		, timer2_(io, boost::posix_time::seconds(1))
		, count_(0)
	{
		timer1_.async_wait(strand_.wrap(boost::bind(
			&printer_with_two_thread::print1
			, this)));
		timer2_.async_wait(strand_.wrap(boost::bind(
			&printer_with_two_thread::print2
			, this)));
	}
	~printer_with_two_thread()
	{
		std::cout << "Final count is " << count_ << std::endl;
	}
	void print1()
	{
		if (count_ < 10)
		{
			std::cout << "Timer 1: " << count_ << std::endl;
			++count_;
			timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));
			// smells like recursion
			timer1_.async_wait(strand_.wrap(boost::bind(
				&printer_with_two_thread::print1
				, this)));
		}
	}
	void print2()
	{
		if (count_ < 10)
		{
			std::cout << "Timer 2: " << count_ << std::endl;
			++count_;
			timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));
			// smells like recursion
			timer2_.async_wait(strand_.wrap(boost::bind(
				&printer_with_two_thread::print2
				, this)));
		}
	}
};
//****************************************************************************
//*                     make_daytime_string
//****************************************************************************
std::string make_daytime_string()
{
	// for time_t, time and ctime
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}
//****************************************************************************
//*                     tcp_connection
//****************************************************************************
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
	tcp::socket socket_;
	std::string message_;
public:
	typedef boost::shared_ptr<tcp_connection> pointer;
	static pointer create(boost::asio::io_service& io_service)
	{
		return pointer(new tcp_connection(io_service));
	}
	tcp::socket& socket()
	{
		return socket_;
	}
	void start()
	{
		std::cout << "start()\n";
		message_ = make_daytime_string();
		boost::asio::async_write(socket_
			, boost::asio::buffer(message_)
			, boost::bind(&tcp_connection::handle_write
				, shared_from_this()
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred
			)
		);
	}
private:
	tcp_connection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{}
	void handle_write(const boost::system::error_code& /*error*/
		, size_t /*bytes_transferred*/
	)
	{}
};
//****************************************************************************
//*                     tcp_server
//****************************************************************************
class tcp_server
{
	tcp::acceptor acceptor_;
public:
	tcp_server(boost::asio::io_service& io_service)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), 13))
	{
		std::cout << "accept()\n";
		start_accept();
	}
private:
	void start_accept()
	{
		tcp_connection::pointer new_connection =
			tcp_connection::create(acceptor_.get_io_service());

		acceptor_.async_accept(new_connection->socket()
			, boost::bind(&tcp_server::handle_accept, this, new_connection
				, boost::asio::placeholders::error)
		);
	}
	void handle_accept(tcp_connection::pointer new_connection
		, const boost::system::error_code& error
	)
	{
		if (!error)
			new_connection->start();
		std::cout << "accept()\n";
		start_accept();
	}
};
//****************************************************************************
//*                     main
//****************************************************************************
int main()
{
	std::cout << "Boost version...: 1_69_0\n";
	std::cout << "Platform........: MSVC 2017 15.9.16\n";
	std::cout << "Compiler........: C++03\n";
	goto introduction_to_sockets;
	std::cout << "Basic Skills\n";
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

	{
		std::cout << "TIMER.5 - Synchronising handlers in multithreaded programs\n";
		boost::asio::io_service io;
		printer_with_two_thread p(io);
		// there is a typo in the example
		// it says: asio::thread t( etc.
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
		io.run();
		t.join();
	}
// label
introduction_to_sockets:
	std::cout << "Introduction to Sockets\n";
	{
		std::cout << "Daytime.1 - A synchronous TCP daytime client\n";
		try
		{
			//char host[] = "192.168.178.14";
			// NIST, Gaithersburg, Maryland (port is 13)
			// e.g. output is:
			// 58777 19-10-21 14:54:50 14 0 0 654.1 UTC(NIST) *
			char host[] = "129.6.15.28";
			boost::asio::io_service io_service;
			tcp::resolver resolver(io_service);
			tcp::resolver::query query(host, "daytime");
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::socket socket(io_service);
			boost::asio::connect(socket, endpoint_iterator);
			for (;;)
			{
				boost::array<char, 128> buf;
				boost::system::error_code error;
				size_t len = socket.read_some(boost::asio::buffer(buf), error);
				if (error == boost::asio::error::eof)
					// connection closed cleanly by peer
					break;
				else if (error)
					// some other error
					throw boost::system::system_error(error);
				std::cout.write(buf.data(), len);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	goto goto_daytime_4;
	{
		std::cout << "Daytime.2 - A synchronous TCP daytime server\n";
		try
		{
			boost::asio::io_service io_service;
			tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 13));
			for (;;)
			{
				tcp::socket socket(io_service);
				acceptor.accept(socket);
				std::string message = make_daytime_string();
				boost::system::error_code ignored_error;
				boost::asio::write(socket
					, boost::asio::buffer(message)
					, ignored_error
				);
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	// message received will be:
	// Mon Oct 21 11:14 : 16 2019
	//
	// De verbinding met de host is verbroken.

goto_daytime_3:
	{
		std::cout << "Daytime.3 - An asynchronous TCP daytime server\n";
		try
		{
			boost::asio::io_service io_service;
			tcp_server server(io_service);
			io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

goto_daytime_4:
	{
		std::cout << "Daytime.4 - A synchronous UDP daytime client\n";
		try
		{
			char host[] = "192.168.178.14";
			// daytime servers on the internet serve only with TCP
			boost::asio::io_service io_service;
			udp::resolver resolver(io_service);
			udp::resolver::query query(udp::v4(), host, "daytime");
			udp::endpoint receiver_endpoint = *resolver.resolve(query);
			udp::socket socket(io_service);
			socket.open(udp::v4());
			boost::array<char, 1> send_buf = { {0} };
			socket.send_to(boost::asio::buffer(send_buf)
				, receiver_endpoint
			);
			boost::array<char, 128> recv_buf;
			udp::endpoint sender_endpoint;
			size_t len = socket.receive_from(boost::asio::buffer(recv_buf)
				, sender_endpoint
			);
			std::cout.write(recv_buf.data(), len);
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
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

#include "headers.h"

//boost
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
//std
#include <thread>
#include <chrono>


using namespace boost::interprocess;
using namespace boost::posix_time;


void func( bool& data, bool& quit, interprocess_condition& cond )
{
	std::this_thread::sleep_for(std::chrono::milliseconds( 5500 )); //5.5x

	// scenario 1 : regular one : signal received
	//data = true;
	//cond.notify_one();

	// scenario 2 : force to quit
	quit = true;
}

//
// quit loop condition
//
struct predicate
{
	bool m_data;
	bool m_quit;

	predicate( bool data, bool quit ) : m_data( data ), m_quit( quit ){}

	bool operator()() const
	{ 
		std::cout << "predicate" << std::endl;
		return m_data || m_quit; 
	}
};


void Class_04::start()
{
	bool data = false;
	bool quit = false;

	interprocess_mutex mutex;
	interprocess_condition cond;

	//
	// pass references to the thread: it works Ok
	//
	auto t = std::thread( func, std::ref( data ), std::ref( quit ), std::ref( cond ) );
	
	scoped_lock<interprocess_mutex> lock( mutex );
	while( !data && !quit  )
	{
		bool res = cond.timed_wait( lock, second_clock::universal_time() + seconds( 1 ) );
		std::cout << "res: " << res << std::endl;
	}

	//
	// for our case of using timed_wait just to force quit, use predicate is not possible
	// it goes on executing the code by timeout and we want to avoid this
	// we want to go on only when predicate happens
	// we timed_wait to evaluate predicate continuously and not only one time
	//
	/*bool res1 = cond.timed_wait( 
		lock, 
		second_clock::universal_time() + seconds( 1 ), 
		predicate( data, quit ) );*/


	if( quit )
		std::cout << "end of wait : forced" << std::endl;
	else
		std::cout << "end of wait : signal" << std::endl;
	int a=0;

	t.join();
}
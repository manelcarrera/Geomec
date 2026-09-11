#include "headers.h"

//boost
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
//#include "doc_anonymous_condition_shared_data.hpp"
#include <libs/interprocess/example/doc_anonymous_condition_shared_data.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
//
#include <iostream>
#include <cstdio>

using namespace boost::interprocess;
using namespace boost::posix_time;

//own
#include "Printer.h"
//qt
#include <QString> 
//#include <QProcess> 
//#include <QStringList>
//std
#include <thread>
#include <chrono>

/////////////////////////////////////////////////
//
//			boost producer / consumer test
//
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//
//			1)original
//
/////////////////////////////////////////////////

namespace gm{
namespace boost{


int consumer()
{
   //Create a shared memory object.
   shared_memory_object shm
      (open_only                    //only create
      ,"MySharedMemory"              //name
      ,read_write                   //read-write mode
      );

   try{
      //Map the whole shared memory in this process
      mapped_region region
         (shm                       //What to map
         ,read_write //Map it as read-write
         );

      //Get the address of the mapped region
      void * addr       = region.get_address();

      //Obtain a pointer to the shared structure
      trace_queue * data = static_cast<trace_queue*>(addr);

      //Print messages until the other process marks the end
      bool end_loop = false;
      do{
         scoped_lock<interprocess_mutex> lock(data->mutex);
         if(!data->message_in){
      data->cond_empty.wait(lock);
         }
         if(std::strcmp(data->items, "last message") == 0){
      end_loop = true;
         }
         else{
      //Print the message
      std::cout << data->items << std::endl;
      //Notify the other process that the buffer is empty
      data->message_in = false;
      data->cond_full.notify_one();
         }
      }
      while(!end_loop);
   }
   catch(interprocess_exception &ex)
   {
      std::cout << ex.what() << std::endl;
      return 1;
   }

   return 0;
}

int producer()
{

   //Erase previous shared memory and schedule erasure on exit
   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("MySharedMemory"); }
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
   } remover;

   //Create a shared memory object.
   shared_memory_object shm
      (create_only               //only create
      ,"MySharedMemory"           //name
      ,read_write                //read-write mode
      );

   try
   {
      //Set size
      shm.truncate(sizeof(trace_queue));

      //Map the whole shared memory in this process
      mapped_region region
         (shm                       //What to map
         ,read_write //Map it as read-write
         );

      //Get the address of the mapped region
      void * addr       = region.get_address();

      //Construct the shared structure in memory
      trace_queue * data = new (addr) trace_queue;

      const int NumMsg = 100;

    //
    //
    //
      for(int i = 0; i < NumMsg; ++i)
    {
         scoped_lock<interprocess_mutex> lock(data->mutex);
         if(data->message_in)
      data->cond_full.wait(lock);

     if(i == (NumMsg-1))
      std::sprintf(data->items, "%s", "last message");
         else
      std::sprintf(data->items, "%s_%d", "my_trace", i);

         //Notify to the other process that there is a message
         data->cond_empty.notify_one();

         //Mark message buffer as full
         data->message_in = true;
      }
    //
    //
    //

   }
   catch(interprocess_exception &ex)
   {
      std::cout << ex.what() << std::endl;
      return 1;
   }

   return 0;
}

/////////////////////////////////////////////////
//
//			1)modifed (very little)
//
/////////////////////////////////////////////////


int consumer_red()
{
  shared_memory_object shm( open_only, "MySharedMemory", read_write );

  mapped_region region( shm, read_write );
  void* addr = region.get_address();
  trace_queue* data = static_cast< trace_queue* >( addr );

  bool end_loop = false;
  do
  {
    scoped_lock<interprocess_mutex> lock(data->mutex);

    if(!data->message_in)
      data->cond_empty.wait(lock);

    if(std::strcmp(data->items, "last message") == 0)
    {
      end_loop = true;
    }
    else
    {
      std::cout << data->items << std::endl;
      data->message_in = false;
      data->cond_full.notify_one();
    }
  }
  while( !end_loop );

   return 0;
}

int producer_red()
{
   //Erase previous shared memory and schedule erasure on exit
   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("MySharedMemory"); }
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
   } remover;

   shared_memory_object shm( create_only, "MySharedMemory", read_write );

  shm.truncate(sizeof(trace_queue));
  mapped_region region( shm, read_write );
  void* addr = region.get_address();
  trace_queue * data = new (addr) trace_queue;


  // 1)
  //
  // Launch child process
  // it waits forever
  //
  /*std::string s( argv[ 0 ] ); s += " child ";
  //if( 0 != std::system( s.c_str() ) )
  if( std::system( s.c_str() ) )
    return 1;*/

  // 2)
  //
  // this works Ok
  /*QProcess p1;
  p1.setProgram( argv[ 0 ] );
  p1.setArguments( QStringList() << "child" );
  p1.startDetached();*/

  const int NumMsg = 5;//100;
  for(int i = 0; i < NumMsg; ++i)
  {
    scoped_lock<interprocess_mutex> lock(data->mutex);
    if( data->message_in )
      data->cond_full.wait(lock);

    //
    std::this_thread::sleep_for(std::chrono::milliseconds( 5*1000 ));
    //

    if(i == (NumMsg-1))
    {
      std::cout << "-> last message" << std::endl;
      std::sprintf(data->items, "%s", "last message");
    }
    else
    {
      std::cout << "-> my_trace" << std::endl;
    }

    data->cond_empty.notify_one();

    data->message_in = true;
  }

  //
  // to keep shm alive for the consumer
  //
  std::this_thread::sleep_for(std::chrono::milliseconds( 1*1000 ));

  return 0;
}

}//namespace boost
}//namespace gm



//
// it works
//
void Class_03::start( eType type_ )
{
  if( type_ == Class_03::Producer )
    gm::boost::producer_red();
  else
    gm::boost::consumer_red();
}

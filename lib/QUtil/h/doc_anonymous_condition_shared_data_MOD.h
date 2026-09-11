#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

struct trace_queue
{
   enum { Length_ = 256 };

   trace_queue() : message_in( false ){}

   // synchro
   boost::interprocess::interprocess_mutex  mutex;				// Mutex to protect access to the queue
   boost::interprocess::interprocess_condition  cond_empty;		// Condition to wait when the queue is empty
   boost::interprocess::interprocess_condition  cond_full;		// Condition to wait when the queue is full

   // data
   char param[ Length_ ];		// param				
   int cmd_id;					// command id
   bool message_in;
};
#include "headers.h"

//std
#include <thread>
#include <chrono>

/////////////////////////////////////////////////
//
//		what does this project do?
//
//	- boost producer / consumer test
//	- kill a process / check out if a process is running
//	- Geomec emulator to test diana_app_new.exe (work-around when OIV online license is not available)
//  - timed_wait
//
/////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
  //Class_01 c;	// work-around OIV license
  //Class_02 c;	// kill a process
  //Class_03 c;	// boost producer / consumer test
  Class_04 c;	// timed_wait

  c.start();

  //
  // testing producer / consumer
  //
  /*Class_03 c1;
  Class_03 c2;

  c1.start( Class_03::Producer );

  // needed
  std::this_thread::sleep_for(std::chrono::milliseconds( 1*1000 )); // shm not ready: wait -> with this it works!

  c1.start( Class_03::Consumer );

  int a=0;
  for(;;){ if( a == 1 ) break; } //trick to avoid warning*/

  return 0;
}
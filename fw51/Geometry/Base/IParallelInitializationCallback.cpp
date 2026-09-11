#include "IParallelInitializationCallback.h"


namespace geo
{

IParallelInitializationCallback::IParallelInitializationCallback()
{
}

IParallelInitializationCallback::~IParallelInitializationCallback()
{
}


// static, to use where we explicitly abandon thread-safety (in order to find those instances)
IParallelInitializationCallback *IParallelInitializationCallback::Sequential = 0;

}
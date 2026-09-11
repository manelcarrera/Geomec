#include "stdafx.h"

#include <psapi.h>

#include "WindowsPlatform.h"

CWindowsPlatform::CWindowsPlatform()
: IPlatform()
{
}

void CWindowsPlatform::init()
{
  assert(m_singleton == 0);
  m_singleton = new CWindowsPlatform();
}

long CWindowsPlatform::processMemorySize() const
{
  long memorySize = 0;
  HANDLE process = GetCurrentProcess();
  PROCESS_MEMORY_COUNTERS processMemoryCounters;

  memset(&processMemoryCounters, 0, sizeof(PROCESS_MEMORY_COUNTERS));
  processMemoryCounters.cb = sizeof(PROCESS_MEMORY_COUNTERS);

  if (GetProcessMemoryInfo(process, &processMemoryCounters,
    processMemoryCounters.cb))
  {
    memorySize = processMemoryCounters.WorkingSetSize;
  }

  return memorySize;
}

void CWindowsPlatform::trace(const QString& message) const
{
  TRACE(message.toStdString().c_str());
}

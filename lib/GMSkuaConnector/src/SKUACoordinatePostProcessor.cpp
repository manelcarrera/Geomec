#include "SKUACoordinatePostProcessor.h"
#include "SKUAParseData.h"

#include <tbb/parallel_for_each.h>


namespace gm_skua
{

void SKUACoordinatePostProcessor::PostProcess()
{
  FixCoordinates();

  if (m_next)
    m_next->PostProcess();
}

// Converts the vertices to SI units, and adjusts the z-coordinate according to ZPOSITIVE
void SKUACoordinatePostProcessor::FixCoordinates()
{
  if (std::abs(m_data->coord_unit[0] - 1) > 1E-12 || std::abs(m_data->coord_unit[1] - 1) > 1E-12 || std::abs(m_data->coord_unit[2] - 1) > 1E-12)
    tbb::parallel_for_each(m_data->vertices.begin(), m_data->vertices.end(),
      [&](SKUAParseData::Vertex &vertex)
  {
    if (vertex.link < 0)
      for (int i = 0; i < 3; ++i)
        if (std::abs(m_data->coord_unit[i] - 1) > 1E-12)
          vertex.coordinate[i] *= m_data->coord_unit[i];
  }
  );
}


}

#include "RGInterface.h"

#include <fstream>

/// RGDepletionStage

RGDepletionStage::RGDepletionStage(const int& depletionStage, const int& year, const int& month)
: m_depletionStage(depletionStage),
  m_year(year),
  m_month(month)
{
}

int RGDepletionStage::getDepletionStage() const
{
  return m_depletionStage;
}

int RGDepletionStage::getYear() const
{
  return m_year;
}

int RGDepletionStage::getMonth() const
{
  return m_month;
}

/// RGNode

RGNode::RGNode(const double& x, const double& y, const double& z)
{
  m_cors[0] = x;
  m_cors[1] = y;
  m_cors[2] = z;
}

RGNode::RGNode()
{
}

const double& RGNode::operator [](size_t n) const
{
  return m_cors[n];
}

double& RGNode::operator [](size_t n)
{
  return m_cors[n];
}

/// RGElement

RGElement::RGElement(const RGElementType &type, const std::vector<RGNodeId> &nodes)
: m_type(type),
  m_nodes(nodes)
{
}

RGElementType RGElement::getType() const
{
  return m_type;
}

size_t RGElement::getNumNodes() const
{
  return m_nodes.size();
}

RGNodeId RGElement::getNodeN(const size_t &n) const
{
  return m_nodes[n];
}

/// RGFault

RGFault::RGFault(const std::string &name, const std::vector<RGElementId> &intfElms)
: m_name(name),
  m_intfElms(intfElms)
{
}

std::string RGFault::getName() const
{
  return m_name;
}

std::vector<RGElementId> RGFault::getIntfElements() const
{
  return m_intfElms;
}

/// RGProperty

RGProperty::RGProperty(const RGPropertyType &type)
: m_Type(type)
{
}

RGProperty::RGProperty()
: m_Type(propLast) // invalid
{
}

RGPropertyType RGProperty::getType() const
{
  return m_Type;
}

bool RGProperty::isValid() const
{
  return m_Type != propLast;
}

/// GMCommand

GMCommand::GMCommand(const GMCommandType &command)
: m_command(command)
{
}

GMCommand::GMCommand(const GMCommandType &command, const RGProperty &property)
: m_command(command),
  m_property(property)
{
}

GMCommandType GMCommand::getType() const
{
  return m_command;
}

RGProperty GMCommand::getProperty() const
{
  return m_property;
}

/// RGInterfaceImpl

class RGInterfaceImpl
{
public:
  RGInterfaceImpl(const std::string& fileName)
  : m_fileName(fileName)
  {
  // initialize from the file
  std::ifstream s(fileName.c_str());
  if(s)
  {
      s >> m_currentStage;
      s >> m_year;
      s >> m_numElements;

      if(!m_currentStage)
      {
    // commands for the initial stage
    m_vcCommands.push_back(GMCommand(typeCommandSaveGrid));
    m_vcCommands.push_back(GMCommand(typeCommandLoadProperty, RGProperty(propElementFluidPressure)));
    m_vcCommands.push_back(GMCommand(typeCommandLoadProperty, RGProperty(propElementTemperature)));
    m_vcCommands.push_back(GMCommand(typeCommandQuit));
      }
      else
      {
    // subsequent stages
    m_vcCommands.push_back(GMCommand(typeCommandLoadProperty, RGProperty(propElementFluidPressure)));
    m_vcCommands.push_back(GMCommand(typeCommandLoadProperty, RGProperty(propElementTemperature)));
    m_vcCommands.push_back(GMCommand(typeCommandCalculate));
    m_vcCommands.push_back(GMCommand(typeCommandQuit));
      }
  }
  }

  RGDepletionStage getCurrentDepletionStage() const
  {
  return RGDepletionStage(m_currentStage, m_year, 5);
  }

  void loadProperty(const RGProperty& property, std::vector<double>& values)
  {
  values.resize(m_numElements);
  int i;
  for(i = 0; i < m_numElements; ++i)
      values[i] = 10 * m_currentStage + 2;
  }

  size_t getNumCommands() const
  {
  return m_vcCommands.size();
  }

  GMCommand getCommandN(const size_t& n) const
  {
  return m_vcCommands[n];
  }

private:
  std::string m_fileName;
  int m_currentStage;
  int m_year;
  int m_numElements;
  std::vector<GMCommand> m_vcCommands;
};

/// RGInterface

RGInterface::RGInterface(const std::string &fileName)
: m_pimpl(new RGInterfaceImpl(fileName))
{
}

RGInterface::~RGInterface()
{
  delete m_pimpl;
}

RGDepletionStage RGInterface::getCurrentDepletionStage() const
{
  return m_pimpl->getCurrentDepletionStage();
}
/*
size_t RGInterface::getNumNodes() const
{
}

RGNode RGInterface::getNodeN(const size_t& n) const
{
}

size_t RGInterface::getNumElements() const
{
}

RGElement RGInterface::getElementN(const size_t& n) const
{
}
*/
void RGInterface::addNode(const RGNode& node)
{
}

void RGInterface::addElement(const RGElement& element)
{
}

void RGInterface::loadProperty(const RGProperty& property, std::vector<double>& values)
{
  m_pimpl->loadProperty(property, values);
}

void RGInterface::saveProperty(const RGProperty& property, const std::vector<double>& values)
{
}

void RGInterface::addCommand(const GMCommand& command)
{
}

size_t RGInterface::getNumCommands() const
{
  return m_pimpl->getNumCommands();
}

GMCommand RGInterface::getCommandN(const size_t& n) const
{
  return m_pimpl->getCommandN(n);
}

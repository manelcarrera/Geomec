#ifndef _WIX_MAKE_GUIDS_H__
#define _WIX_MAKE_GUIDS_H__

#pragma once


#include <string>
#include <list>
#include <iostream>


class Template
{
public:
  Template(std::string path, std::string templateFile);

  void write(std::string outputFile);

private:
  static std::string getGUID();

  struct TemplatePart
  {
    virtual void write(std::ostream &fs) = 0;
  };
  struct TemplatePartString : public TemplatePart
  {
    std::string m_Part;
    TemplatePartString(std::string part) : m_Part(part) {}

    virtual void write(std::ostream &fs) { fs << m_Part; }
  };
  struct TemplatePartGUID : public TemplatePart
  {
    TemplatePartGUID() {}

    virtual void write(std::ostream &fs) { fs << getGUID(); }
  };

  std::string m_Path;
  std::list<TemplatePart *> m_List;
};



#endif
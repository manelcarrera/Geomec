#include "WixMakeGUIDs.h"

void usage() {
  std::cout << "Usage: WixMakeGUIDs [options]" << std::endl;
  std::cout << std::endl;
  std::cout << "With options:" << std::endl;
  std::cout << "  --path=<path/to/WixInclude> (default is ../../wix/WixInclude/)" << std::endl;
  std::cout << "  --template=<template file name> (default is template_Guids_.wxi)" << std::endl;
  std::cout << "  --output=<template for output file> (default is Guids$.wxi)" << std::endl;
  std::cout << "  --list=<name_0>,<name_1>,etc" << std::endl;
  std::cout << std::endl;
  std::cout << "Reads the template file and replaces all instances of AUTOGUID" << std::endl;
  std::cout << "with a generated GUID." << std::endl;
  std::cout << "It does this for each name in list, and writes the results" << std::endl;
  std::cout << "to the output file, where the $ is replaced with the name." << std::endl;
  std::cout << std::endl;
  std::cout << "Example: WixMakeGUIDs --list=Test,AndAnother" << std::endl;
  std::cout << "creates two files: GuidsTest.wxi and GuidsAndAnother.wxi" << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {

  std::string path = "../../wix/WixInclude/";
  std::string templateFile = "template_Guids_.wxi";
  std::string outputTempl = "Guids$.wxi";

  std::list<std::string> params;

  for (int i = 1; i < argc; ++i) {
    if (!strncmp(argv[i], "--path=", 7)) {
      path = std::string(argv[i]).substr(7);
    } else if (!strncmp(argv[i], "--template=", 11)) {
      templateFile = std::string(argv[i]).substr(11);
    } else if (!strncmp(argv[i], "--output=", 9)) {
      outputTempl = std::string(argv[i]).substr(9);
    } else if (!strncmp(argv[i], "--list=", 7)) {
      std::string list = std::string(argv[i]).substr(7);
      std::size_t prev = 0, cur = 0, sz = list.size();

      while (cur < sz) {
        if (list[cur] == ',') {
          params.push_back(list.substr(prev, cur));
          prev = cur + 1;
        }

        ++cur;
      }
      if (prev < sz) {
        params.push_back(list.substr(prev, cur));
      }
    }
  }

  if (params.size() == 0) {
    usage();
    std::cin.ignore();
    return 1;
  }

  Template tmpl(path, templateFile);

  std::size_t pos = outputTempl.find('$');

  for (std::list<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
    std::string fileName = outputTempl;
    fileName.replace(pos, 1, *it);
    tmpl.write(path + fileName);
  }

  std::cin.ignore();

  return 0;
}
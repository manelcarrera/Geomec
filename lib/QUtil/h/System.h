#pragma once

#include <string>

class CSystem {
public:
  static const std::string ERROR_;
  static const std::string UNPACK_OK;

  bool unpack(const std::string &package, const std::string &dst_dir, const std::string &files = std::string());
  std::string exe(const std::string cmd);

  virtual std::string get_unzip();
};

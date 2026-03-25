#include <cstdlib>

#include "cmdr.h"
#include "processor.h"

using cmdr::option_builder;
using cmdr::option_id;
using cmdr::options;

option_builder cmdr::cmdr::option(char const *name) {
  size_t id = options.size();
  options.push_back(cmdr::cmdr::option_params{
      .name = name,
  });

  return option_builder(*this, (option_id)id);
}

option_builder &option_builder::abbrev(char ch) {
  cmdr.options[id].short_opt = ch;
  return *this;
}

option_builder &cmdr::option_builder::full(char const *full) {
  cmdr.options[id].long_opt = full;

  return *this;
}

option_id cmdr::option_builder::finalize() { return id; }

options cmdr::cmdr::parse(std::initializer_list<char const *> args) const {
  ::cmdr::processor proc{*this};

  for (const auto &arg : args) {
    proc.visit(arg);
  }
}

bool cmdr::options::exists(unsigned short) { return false; }

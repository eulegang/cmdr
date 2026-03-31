#include "cmdr.h"

using cmdr::option_builder;
using cmdr::option_id;

option_builder cmdr::cmdr::option(char const *name) {
  size_t id = _options.size();
  _options.push_back(cmdr::cmdr::option_params{
      .name = name,
  });

  return option_builder(*this, (option_id)id);
}

option_builder &option_builder::abbrev(char ch) {
  _cmdr._options[_id].short_opt = ch;
  return *this;
}

option_builder &option_builder::full(char const *full) {
  _cmdr._options[_id].long_opt = full;

  return *this;
}

option_builder &option_builder::required() {
  _cmdr._options[_id].flags |= cmdr::option_params::FLAGS_required;

  return *this;
}

option_builder &option_builder::boolean() {
  _cmdr._options[_id].flags |= cmdr::option_params::FLAGS_boolean;

  return *this;
}

option_builder &option_builder::position(size_t position) {
  _cmdr._options[_id].position = position;

  return *this;
}

option_builder &option_builder::env(const char *var) {
  _cmdr._options[_id].env = var;

  return *this;
}

option_builder &option_builder::help(const char *help) {
  _cmdr._options[_id].help = help;

  return *this;
}

option_id cmdr::option_builder::finalize() { return _id; }

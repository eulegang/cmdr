#include "cmdr.h"
#include "processor.h"
#include <cassert>
#include <cstring>

using cmdr::option_id;
using cmdr::options;
using cmdr::processor;

options cmdr::cmdr::parse(std::initializer_list<char const *> args) const {
  options opts{*this};
  processor proc{*this, opts};

  for (const auto &arg : args) {
    proc.visit(arg);
  }

  proc.finalize();

  return opts;
}

const cmdr::cmdr::option_params *cmdr::cmdr::lookup_short(char ch,
                                                          option_id *id) const {
  option_id cur = 0;
  for (const auto &opt : _options) {
    if (opt.short_opt == ch) {
      *id = cur;
      return &opt;
    }
    cur++;
  }

  return NULL;
}

const cmdr::cmdr::option_params *cmdr::cmdr::lookup_long(const char *arg,
                                                         option_id *id) const {
  option_id cur = 0;
  for (const auto &opt : _options) {
    if (!strcmp(opt.long_opt, arg)) {
      *id = cur;
      return &opt;
    }
    cur++;
  }

  return NULL;
}

bool cmdr::options::exists(option_id id) {
  assert(id < _slots.size());

  return _slots[id].check.kind != slot_kind::unset;
}

options::options(const cmdr &cmdr) : _slots{} {
  _slots.reserve(cmdr._options.size());

  for (size_t i{}; i < cmdr._options.size(); i++) {
    _slots.push_back(slot{.unset_value = {.kind = slot_kind::unset}});
  }
}

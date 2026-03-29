#include "cmdr.h"
#include <cassert>

using cmdr::option_id;
using cmdr::options;

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

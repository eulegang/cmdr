#include "cmdr.h"
#include <cassert>

using cmdr::option_id;
using cmdr::options;

bool cmdr::options::exists(option_id id) {
  assert(id < _slots.size());

  return _slots[id].check.kind != slot_kind::unset;
}

options::~options() {
  for (size_t i{}; i < _slots.size(); ++i) {
    const auto &slot = _slots[i];
    const auto &params = _cmdr._options[i];

    if (slot.parsed_value.kind == options::slot_kind::parsed) {
      params.deleteFn(slot.parsed_value.value);
    }
  }
}

options::options(const cmdr &cmdr) : _cmdr{cmdr}, _slots{} {
  _slots.reserve(cmdr._options.size());

  for (size_t i{}; i < cmdr._options.size(); i++) {
    _slots.push_back(slot{.unset_value = {.kind = slot_kind::unset}});
  }
}

template <> const char *options::get<const char *>(option_id id) {
  auto &slot = _slots[id];

  if (slot.bool_value.kind == options::slot_kind::unset)
    return NULL;

  if (slot.str_value.kind != options::slot_kind::str) {
    throw invalid_type_error(slot_kind_repr(options::slot_kind::str),
                             slot_kind_repr(slot.check.kind));
  }
  return slot.str_value.value;
}

template <> bool options::get<bool>(option_id id) {
  auto &slot = _slots[id];

  if (slot.bool_value.kind == options::slot_kind::unset)
    return false;

  if (slot.str_value.kind != options::slot_kind::boolean) {
    throw invalid_type_error(slot_kind_repr(options::slot_kind::boolean),
                             slot_kind_repr(slot.check.kind));
  }
  return slot.bool_value.value;
}

const char *options::slot_kind_repr(options::slot_kind kind) {
  switch (kind) {
  case slot_kind::unset:
    return "unset";
  case slot_kind::boolean:
    return "bool";
  case slot_kind::str:
    return "string";
  case slot_kind::parsed:
    return "parsed";
  }

  assert(false && "should not get here");
}

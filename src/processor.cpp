#include "processor.h"
#include "cmdr.h"
#include <cassert>

using cmdr::processor;

namespace cmdr {
bool is_short(const char *);
bool is_long(const char *);
} // namespace cmdr

void processor::visit(const char *arg) {

  switch (_state) {
  case state::cmd:
    _state = state::blank;
    break;

  case state::blank:
    if (is_short(arg)) {
      process_short(arg);
    } else if (is_long(arg)) {
      process_long(arg);
    } else {
      process_bare(arg);
    }
    break;

  case state::flag:
    process_flag(arg);
    break;
  }
}

void processor::finalize() {
  for (size_t i{}; i < _opts._slots.size(); ++i) {
    const auto &slot = _opts._slots[i];
    const auto &params = _cmdr._options[i];

    if (params.is_required() && slot.check.kind == options::slot_kind::unset) {
      throw missing_required_error(params.name);
    }
  }
}

bool cmdr::is_short(const char *opt) {
  assert(opt);

  char a = *opt, b = *(opt + 1);

  return a == '-' && a != 0 && b != '-';
}

bool cmdr::is_long(const char *opt) {
  assert(opt);
  char a = *opt, b = *(opt + 1);

  return a == '-' && b == '-';
}

void processor::process_short(const char *arg) {
  bool opt_required = false;
  arg++;

  while (*arg) {
    option_id id;
    auto opt = _cmdr.lookup_short(*arg, &id);

    if (!opt) {
      throw invalid_flag_error(arg);
    }

    if (opt->is_boolean()) {
      auto &slot = _opts._slots[id];

      if (slot.check.kind != options::slot_kind::unset) {
        throw multiset_error(arg);
      }

      slot = {.bool_value{
          .kind = options::slot_kind::boolean,
          .value = true,
      }};
    } else {
      if (opt_required) {
        throw multivalue_error();
      }
      opt_required = true;
      _state = state::flag;
      _flag = id;
    }

    arg++;
  }
}

void processor::process_long(const char *arg) {
  arg += 2;

  option_id id;
  auto opt = _cmdr.lookup_long(arg, &id);
  if (!opt) {
    throw invalid_flag_error(arg);
  }

  if (opt->is_boolean()) {
    auto &slot = _opts._slots[id];

    if (slot.check.kind != options::slot_kind::unset) {
      throw multiset_error(arg);
    }

    slot = {.bool_value{
        .kind = options::slot_kind::boolean,
        .value = true,
    }};
  } else {
    _state = state::flag;
    _flag = id;
  }
}

void processor::process_bare(const char *arg) {
  option_id id;
  auto opt = _cmdr.lookup_pos(_position++, &id);
  if (!opt) {
    throw invalid_flag_error(arg);
  }

  assert(!opt->is_boolean());

  auto &slot = _opts._slots[id];
  if (slot.check.kind != options::slot_kind::unset) {
    throw multiset_error(arg);
  }

  if (opt->parse) {
    void *output = opt->parse(arg);
    slot = {.parsed_value = {
                .kind = options::slot_kind::parsed,
                .value = output,
            }};
  } else {

    slot = {.str_value = {
                .kind = options::slot_kind::str,
                .value = arg,
            }};
  }
}

void processor::process_flag(const char *arg) {
  auto &params = _cmdr._options[_flag];
  auto &slot = _opts._slots[_flag];

  if (slot.check.kind != options::slot_kind::unset) {
    throw multiset_error(arg);
  }

  assert(!params.is_boolean());

  if (params.parse) {
    void *output = params.parse(arg);
    slot = {.parsed_value = {
                .kind = options::slot_kind::parsed,
                .value = output,
            }};
  } else {
    slot = {.str_value = {
                .kind = options::slot_kind::str,
                .value = arg,
            }};
  }
}

#include "processor.h"
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

void processor::finalize() {}

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
    assert(opt && "should throw exception on missing flag");

    if (opt->is_boolean()) {
      auto &slot = _opts._slots[id];

      assert(slot.check.kind == options::slot_kind::unset &&
             "should throw exception on multi set");

      slot = {.bool_value{
          .kind = options::slot_kind::boolean,
          .value = true,
      }};
    } else {
      assert(!opt_required && "should throw on ambigous flag values");
      opt_required = true;
      _state = state::flag;
      _flag = id;
    }

    arg++;
  }
}

void processor::process_long(const char *arg) {
  bool opt_required = false;
  arg += 2;

  option_id id;
  auto opt = _cmdr.lookup_long(arg, &id);
  assert(opt && "should throw exception on missing flag");

  if (opt->is_boolean()) {
    auto &slot = _opts._slots[id];

    assert(slot.check.kind == options::slot_kind::unset &&
           "should throw exception on multi set");

    slot = {.bool_value{
        .kind = options::slot_kind::boolean,
        .value = true,
    }};
  } else {
    assert(!opt_required && "should throw on ambigous flag values");
    opt_required = true;
    _state = state::flag;
    _flag = id;
  }
}

void processor::process_bare(const char *arg) {
  option_id id;
  auto opt = _cmdr.lookup_pos(_position++, &id);
  assert(opt && "should throw exception on missing flag");
  assert(!opt->is_boolean());

  auto &slot = _opts._slots[id];
  assert(slot.str_value.kind == options::slot_kind::unset);

  slot = {.str_value = {
              .kind = options::slot_kind::str,
              .value = arg,
          }};
}

void processor::process_flag(const char *arg) {
  auto &params = _cmdr._options[_flag];
  auto &slot = _opts._slots[_flag];

  assert(slot.check.kind == options::slot_kind::unset &&
         "should throw on multi set");

  assert(!params.is_boolean());

  slot = {.str_value = {
              .kind = options::slot_kind::str,
              .value = arg,
          }};
}

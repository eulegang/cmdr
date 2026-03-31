#include "cmdr.h"
#include "processor.h"
#include <cassert>
#include <cstring>
#include <iostream>

using cmdr::option_id;
using cmdr::options;
using cmdr::processor;

options cmdr::cmdr::parse(std::initializer_list<const char *> args) const {
  try {
    options opts{*this};
    processor proc{*this, opts};

    for (const auto &arg : args) {
      proc.visit(arg);
    }

    proc.finalize();

    return opts;
  } catch (const parse_error &err) {
    if (rethrow) {
      throw;
    } else {
      std::cerr << err.what() << std::endl;
      exit(1);
    }
  }
}

options cmdr::cmdr::parse(int argc, const char **argv) const {
  try {
    options opts{*this};
    processor proc{*this, opts};

    while (argc) {
      proc.visit(*argv);

      argv++;
      argc--;
    }

    proc.finalize();

    return opts;
  } catch (const parse_error &err) {
    if (rethrow) {
      throw;
    } else {
      std::cerr << err.what() << std::endl;
      exit(1);
    }
  }
}

const cmdr::cmdr::option_params *cmdr::cmdr::lookup_pos(size_t pos,
                                                        option_id *id) const {
  option_id cur = 0;
  for (const auto &opt : _options) {
    if (opt.position == pos) {
      if (id)
        *id = cur;
      return &opt;
    }
    cur++;
  }

  return NULL;
}

const cmdr::cmdr::option_params *cmdr::cmdr::lookup_short(char ch,
                                                          option_id *id) const {
  option_id cur = 0;
  for (const auto &opt : _options) {
    if (opt.short_opt == ch) {
      if (id)
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
      if (id)
        *id = cur;
      return &opt;
    }
    cur++;
  }

  return NULL;
}

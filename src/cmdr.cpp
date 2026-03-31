#include "cmdr.h"
#include "processor.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

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

size_t cmdr::cmdr::option_size(const cmdr::cmdr::option_params &opt) {
  // 2 spaces before line
  size_t len = 2;

  if (opt.short_opt != 0)
    len += 2;

  // comma
  if (opt.short_opt != 0 && opt.long_opt != NULL)
    len += 2;

  if (opt.long_opt != NULL)
    len += 2 + strlen(opt.long_opt);

  if (!opt.is_boolean()) {
    len += 3 + strlen(opt.name);
  }

  return len;
}

std::string cmdr::cmdr::help() const {

  std::stringstream sout;

  sout << _name;
  if (!_version.empty()) {
    sout << "(" << _version << ")";
  }

  sout << ": " << _description << std::endl;

  size_t base_len = 0;
  for (const auto &opt : _options) {
    base_len = std::max(base_len, option_size(opt));
  }

  for (const auto &opt : _options) {
    sout << "  ";

    if (opt.short_opt != 0) {
      sout << "-" << opt.short_opt;
    }

    if (opt.short_opt != 0 && opt.long_opt != NULL) {
      sout << ", ";
    }

    if (opt.long_opt != NULL) {
      sout << "--" << opt.long_opt;
    }

    if (!opt.is_boolean()) {
      if (opt.short_opt != 0 && opt.long_opt != NULL)
        sout << ' ';
      sout << (opt.is_required() ? '[' : '<');
      sout << opt.name;
      sout << (opt.is_required() ? ']' : '>');
    }

    if (opt.help) {
      size_t len = option_size(opt);
      size_t buffer = (opt.short_opt != 0 && opt.long_opt != NULL) ? 2 : 3;

      sout << std::string((base_len + buffer) - len, ' ');
      sout << opt.help;
    }

    sout << std::endl;
  }

  return sout.str();
}

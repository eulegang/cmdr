#ifndef _CMDR_H
#define _CMDR_H

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace cmdr {
using option_id = uint16_t;

class cmdr;

class option_builder {
  ::cmdr::cmdr &cmdr;
  option_id id;

  option_builder(::cmdr::cmdr &cmdr, option_id id) : cmdr{cmdr}, id{id} {}

  friend class cmdr;

public:
  option_builder &abbrev(char s);
  option_builder &full(const char *l);
  option_id finalize();
};

class options {

public:
  bool exists(option_id id);
};

class cmdr {
  struct option_params {
    char short_opt = 0;
    const char *long_opt = NULL;
    const char *name = NULL;
  };

  std::vector<option_params> options;

  friend class option_builder;

public:
  cmdr() : options{} {}

  option_builder option(const char *);

  class options parse(int argc, const char **argv) const;
  class options parse(std::initializer_list<const char *>) const;
};

} // namespace cmdr

#endif

#ifndef _CMDR_H
#define _CMDR_H

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace cmdr {
using option_id = uint16_t;

class cmdr;

class option_builder final {
  ::cmdr::cmdr &_cmdr;
  option_id _id;

  option_builder(::cmdr::cmdr &cmdr, option_id id) : _cmdr{cmdr}, _id{id} {}

  friend class cmdr;

public:
  option_builder &abbrev(char s);
  option_builder &full(const char *l);
  option_builder &required();
  option_builder &boolean();
  option_id finalize();
};

class options final {
  enum class slot_kind {
    unset,
    boolean,
    str,
    parsed,
  };

  union slot {
    struct unset_flag_value {
      slot_kind kind;
    } unset_value;

    struct bool_flag_value {
      slot_kind kind;
      bool value;
    } bool_value;

    struct str_flag_value {
      slot_kind kind;
      const char *value;
    } str_value;

    struct parsed_flag_value {
      slot_kind kind;
      void *value;
    } parsed_value;

    struct check {
      slot_kind kind;
    } check;
  };

  std::vector<slot> _slots;

  friend class cmdr;
  friend class processor;

  options(const cmdr &cmdr);

public:
  bool exists(option_id id);
};

class cmdr final {
  struct option_params {
    static const char FLAGS_required = 1 << 0;
    static const char FLAGS_boolean = 1 << 1;

    const char *long_opt = NULL;
    const char *name = NULL;
    char short_opt = 0;
    char flags = 0;

    bool is_boolean() const { return (flags & FLAGS_boolean) != 0; }
    bool is_required() const { return (flags & FLAGS_required) != 0; }
  };

  std::vector<option_params> _options;

  friend class option_builder;
  friend class options;
  friend class processor;

  const option_params *lookup_short(char, option_id *n = NULL) const;
  const option_params *lookup_long(const char *, option_id *n = NULL) const;

public:
  cmdr() : _options{} {}

  option_builder option(const char *);

  options parse(int argc, const char **argv) const;
  options parse(std::initializer_list<const char *>) const;
};

} // namespace cmdr

#endif

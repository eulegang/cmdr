#ifndef _CMDR_H
#define _CMDR_H

#include <cassert>
#include <cstdint>
#include <format>
#include <functional>
#include <initializer_list>
#include <vector>

namespace cmdr {
using option_id = uint16_t;

class cmdr;

class parse_error : public std::exception {
  std::string msg;

public:
  parse_error(std::string msg) : msg{msg} {}
  const char *what() const noexcept override { return msg.c_str(); }
};

class invalid_flag_error : public parse_error {
public:
  invalid_flag_error(const char *name)
      : parse_error(std::format("invalid flag: ", name)) {}
};

class multiset_error : public parse_error {
public:
  multiset_error(const char *name)
      : parse_error(std::format("multiple flags: ", name)) {}
};

class multivalue_error : public parse_error {
public:
  multivalue_error()
      : parse_error(
            "multiple short flags for values specified in same cluster") {}
};

class invalid_type_error : public parse_error {
public:
  invalid_type_error(const char *expected, const char *actual)
      : parse_error(std::format("invalid value expected {} but found {}",
                                expected, actual)) {}
};

class missing_required_error : public parse_error {
public:
  missing_required_error(const char *name)
      : parse_error(std::format("value never provided for {}", name)) {}
};

class options final {
  enum class slot_kind {
    unset,
    boolean,
    str,
    parsed,
  };

  const char *slot_kind_repr(slot_kind);

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

  const cmdr &_cmdr;
  std::vector<slot> _slots;

  friend class cmdr;
  friend class processor;

  options(const cmdr &cmdr);

public:
  ~options();
  bool exists(option_id id);

  template <typename T> T get(option_id id) {
    auto &slot = _slots[id];
    if (slot.check.kind == options::slot_kind::unset)
      return nullptr;

    if (slot.check.kind != options::slot_kind::parsed) {
      throw invalid_type_error(slot_kind_repr(options::slot_kind::parsed),
                               slot_kind_repr(slot.check.kind));
    }

    return (T)slot.parsed_value.value;
  }
};

template <> bool options::get<bool>(option_id id);
template <> const char *options::get<const char *>(option_id id);

class cmdr final {
  struct option_params {
    static const char FLAGS_required = 1 << 0;
    static const char FLAGS_boolean = 1 << 1;

    const char *long_opt = NULL;
    const char *name = NULL;
    const char *env = NULL;
    const char *help = NULL;
    size_t position = -1;
    char short_opt = 0;
    char flags = 0;
    std::function<void *(const char *)> parse = nullptr;
    std::function<void(void *)> deleteFn = nullptr;

    bool is_boolean() const { return (flags & FLAGS_boolean) != 0; }
    bool is_required() const { return (flags & FLAGS_required) != 0; }
  };

  std::vector<option_params> _options;
  std::string _name = "";
  std::string _version = "";
  std::string _description = "";

  friend class option_builder;
  friend class options;
  friend class processor;

  const option_params *lookup_pos(size_t, option_id *n = NULL) const;
  const option_params *lookup_short(char, option_id *n = NULL) const;
  const option_params *lookup_long(const char *, option_id *n = NULL) const;
  static size_t option_size(const option_params &);

public:
  bool rethrow;
  cmdr(std::string name, std::string version, std::string description)
      : _options{}, _name{name}, _version{version}, _description{description},
        rethrow{false} {}
  cmdr() : _options{}, rethrow{false} {}

  class option_builder option(const char *);

  options parse(int argc, const char **argv) const;
  options parse(std::initializer_list<const char *>) const;

  std::string help() const;
};

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
  option_builder &position(size_t pos);
  option_builder &env(const char *var);
  option_builder &help(const char *var);
  template <typename T>
  option_builder &parse(
      std::function<T *(const char *)> parse,
      std::function<void(void *)> deleteFn = [](void *t) { delete (T *)t; }) {

    _cmdr._options[_id].parse = parse;
    _cmdr._options[_id].deleteFn = deleteFn;

    return *this;
  }
  option_id finalize();
};

} // namespace cmdr

#endif

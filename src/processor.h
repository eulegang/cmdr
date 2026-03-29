#ifndef _CMDR_PROC_H
#define _CMDR_PROC_H

#include "cmdr.h"

namespace cmdr {
class processor {
  enum class state {
    cmd,
    blank,
    flag,
  };

  const cmdr &_cmdr;
  options &_opts;
  size_t _position;
  state _state;
  option_id _flag;

  void process_short(const char *);
  void process_long(const char *);
  void process_bare(const char *);
  void process_flag(const char *);

public:
  processor(const cmdr &cmdr, options &opts)
      : _cmdr{cmdr}, _opts{opts}, _position{}, _state{processor::state::cmd} {}
  void visit(const char *arg);
  void finalize();
};
} // namespace cmdr

#endif

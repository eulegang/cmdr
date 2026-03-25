#ifndef _CMDR_PROC_H
#define _CMDR_PROC_H

#include "cmdr.h"

namespace cmdr {
class processor {
  const ::cmdr::cmdr &cmdr;

public:
  processor(const ::cmdr::cmdr &cmdr) : cmdr{cmdr} {}

  void visit(const char *arg);
};
} // namespace cmdr

#endif

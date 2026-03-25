#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, verbose) {
  cmdr::cmdr cmdr{};

  cmdr::option_id verbose =
      cmdr.option("verbose").abbrev('v').full("verbose").finalize();

  cmdr::options opts = cmdr.parse({"-v"});

  EXPECT_TRUE(opts.exists(verbose));
}

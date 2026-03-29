#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, bool_flag) {
  cmdr::cmdr cmdr{};

  cmdr::option_id verbose =
      cmdr.option("verbose").abbrev('v').full("verbose").boolean().finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-v"});
    EXPECT_TRUE(opts.exists(verbose));
  }

  {
    cmdr::options opts = cmdr.parse({"cmd", "--verbose"});
    EXPECT_TRUE(opts.exists(verbose));
  }

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_FALSE(opts.exists(verbose));
  }
}

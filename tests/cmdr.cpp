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

TEST(cmdr, str_flag) {
  cmdr::cmdr cmdr{};

  cmdr::option_id output =
      cmdr.option("environment").abbrev('e').full("env").finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-e", "dev"});
    EXPECT_TRUE(opts.exists(output));
    EXPECT_STREQ(opts.get<const char *>(output), "dev");
  }

  {
    cmdr::options opts = cmdr.parse({"cmd", "--env", "qa"});
    EXPECT_TRUE(opts.exists(output));
    EXPECT_STREQ(opts.get<const char *>(output), "qa");
  }

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_FALSE(opts.exists(output));
    EXPECT_EQ(opts.get<const char *>(output), nullptr);
  }
}

#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, bool_flag) {
  cmdr::cmdr cmdr{};

  cmdr::option_id verbose =
      cmdr.option("verbose").abbrev('v').full("verbose").boolean().finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-v"});
    EXPECT_TRUE(opts.exists(verbose));
    EXPECT_TRUE(opts.get<bool>(verbose));
  }

  {
    cmdr::options opts = cmdr.parse({"cmd", "--verbose"});
    EXPECT_TRUE(opts.exists(verbose));
    EXPECT_TRUE(opts.get<bool>(verbose));
  }

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_FALSE(opts.exists(verbose));
    EXPECT_FALSE(opts.get<bool>(verbose));
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

TEST(cmdr, position_flag) {
  cmdr::cmdr cmdr{};

  cmdr::option_id cmd = cmdr.option("cmd").position(0).finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "ls"});
    EXPECT_TRUE(opts.exists(cmd));
    EXPECT_STREQ(opts.get<const char *>(cmd), "ls");
  }

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_FALSE(opts.exists(cmd));
    EXPECT_EQ(opts.get<const char *>(cmd), nullptr);
  }
}

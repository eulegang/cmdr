#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, argv_parse) {
  cmdr::cmdr cmdr{};
  cmdr::option_id verbose =
      cmdr.option("verbose").abbrev('v').full("verbose").boolean().finalize();

  int argc = 2;
  const char *argv[] = {"cmdr", "-v"};

  auto opts = cmdr.parse(argc, argv);

  EXPECT_TRUE(opts.get<bool>(verbose));
}

TEST(cmdr, argv_parse_fail) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;
  cmdr.option("verbose").abbrev('v').full("verbose").boolean().finalize();

  int argc = 2;
  const char *argv[] = {"cmdr", "-vo"};

  EXPECT_ANY_THROW({ cmdr.parse(argc, argv); });
}

#include <fcntl.h>
#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, multiset_flag) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  cmdr.option("input").abbrev('i').full("input").finalize();

  EXPECT_ANY_THROW(
      { cmdr::options opts = cmdr.parse({"cmd", "-i", "-", "-i", "xyz"}); });
}

TEST(cmdr, required_flag) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  cmdr::option_id input =
      cmdr.option("input").abbrev('i').full("input").required().finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-i", "-"});
    EXPECT_TRUE(opts.exists(input));
    EXPECT_STREQ(opts.get<const char *>(input), "-");
  }

  {
    EXPECT_ANY_THROW({ cmdr::options opts = cmdr.parse({"cmd"}); });
  }
}

TEST(cmdr, invalid_flag) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  cmdr.option("input").abbrev('i').full("input").finalize();

  EXPECT_ANY_THROW({ cmdr::options opts = cmdr.parse({"cmd", "-o", "-"}); });
  EXPECT_ANY_THROW(
      { cmdr::options opts = cmdr.parse({"cmd", "--output", "-"}); });
  EXPECT_ANY_THROW({ cmdr::options opts = cmdr.parse({"cmd", "xyz.out"}); });
}

TEST(cmdr, multiset_bool_flag) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  cmdr.option("test").abbrev('t').full("test").boolean().finalize();

  EXPECT_ANY_THROW({ cmdr::options opts = cmdr.parse({"cmd", "-t", "-t"}); });
  EXPECT_ANY_THROW(
      { cmdr::options opts = cmdr.parse({"cmd", "--test", "--test"}); });
}

TEST(cmdr, multi_value_flag) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  cmdr.option("output").abbrev('o').full("output").finalize();
  cmdr.option("input").abbrev('i').full("input").finalize();

  EXPECT_ANY_THROW(
      { cmdr::options opts = cmdr.parse({"cmd", "-io", "xyz.txt"}); });
}

TEST(cmdr, invalid_access) {
  cmdr::cmdr cmdr{};
  cmdr.rethrow = true;

  auto output = cmdr.option("output").abbrev('o').full("output").finalize();
  auto test = cmdr.option("test").abbrev('t').full("test").boolean().finalize();

  cmdr::options opts = cmdr.parse({"cmd", "-to", "xyz.txt"});

  EXPECT_ANY_THROW({ opts.get<bool>(output); });
  EXPECT_ANY_THROW({ opts.get<const char *>(test); });
  EXPECT_ANY_THROW({ opts.get<int *>(output); });
}

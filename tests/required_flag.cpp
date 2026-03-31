#include <fcntl.h>
#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, required_flag) {
  cmdr::cmdr cmdr{};

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

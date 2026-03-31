#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdexcept>

#include "cmdr.h"

class Input {
public:
  int fd;

  ~Input() {
    if (fd > 2) {
      close(fd);
    }
  }

  static Input *parse(const char *input) {
    Input *out = new Input{};
    if (!strcmp("-", input)) {
      out->fd = 1;
    } else {
      int fd = open(input, O_RDONLY);

      if (fd == -1) {
        throw std::runtime_error("failed to open");
      }

      out->fd = fd;
    }
    return out;
  }
};

TEST(cmdr, parse_flag) {
  cmdr::cmdr cmdr{};

  std::function<Input *(const char *)> fn = Input::parse;

  cmdr::option_id input =
      cmdr.option("input")
          .abbrev('i')
          .full("input")
          .parse((std::function<Input *(const char *)>)Input::parse)
          .finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-i", "-"});
    EXPECT_TRUE(opts.exists(input));

    Input *out = opts.get<Input *>(input);

    ASSERT_TRUE(out);
    EXPECT_EQ(out->fd, 1);
  }

  {
    cmdr::options opts = cmdr.parse({"cmd", "-i", "../tests/" __FILE_NAME__});
    EXPECT_TRUE(opts.exists(input));

    Input *out = opts.get<Input *>(input);

    ASSERT_TRUE(out);
    EXPECT_TRUE(out->fd > 2);
  }

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_FALSE(opts.exists(input));
    EXPECT_EQ(opts.get<Input *>(input), nullptr);
  }
}

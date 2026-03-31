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

TEST(cmdr, parse_position) {
  cmdr::cmdr cmdr{};

  cmdr::option_id input =
      cmdr.option("input")
          .position(0)
          .parse((std::function<Input *(const char *)>)Input::parse)
          .finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd", "-"});
    EXPECT_TRUE(opts.exists(input));

    Input *out = opts.get<Input *>(input);

    ASSERT_TRUE(out);
    EXPECT_EQ(out->fd, 1);
  }
}

class Paths {
public:
  std::vector<std::string_view> _parts;

  Paths() : _parts{} {}

  static Paths *parse(const char *path) {
    Paths *p = new Paths;

    const char *base = path;
    while (*path) {
      if (*path == ':') {
        p->_parts.emplace_back(base, path - base);
        base = path + 1;
      }

      path++;
    }
    if (base != path)
      p->_parts.emplace_back(base, path - base);

    return p;
  }
};

TEST(cmdr, env_parse_flag) {
  cmdr::cmdr cmdr{};

  cmdr::option_id input =
      cmdr.option("path")
          .abbrev('p')
          .full("path")
          .parse((std::function<Paths *(const char *)>)Paths::parse)
          .env("PATH")
          .finalize();

  {
    cmdr::options opts = cmdr.parse({"cmd"});
    EXPECT_TRUE(opts.exists(input));

    Paths *out = opts.get<Paths *>(input);
    EXPECT_TRUE(out->_parts.size() != 0);
  }

  {
    cmdr::options opts = cmdr.parse({"cmd", "-p", "/usr/bin:/usr/local/bin"});
    EXPECT_TRUE(opts.exists(input));

    Paths *out = opts.get<Paths *>(input);
    EXPECT_TRUE(out->_parts.size() == 2);
  }
}

#include "gtest/gtest.h"
#include <gtest/gtest.h>

#include "cmdr.h"

TEST(cmdr, help_string) {
  cmdr::cmdr cmdr{"deploy", "0.1.2", "deploy code"};
  cmdr.option("verbose")
      .abbrev('v')
      .full("verbose")
      .boolean()
      .help("show which commands are run")
      .finalize();

  cmdr.option("revision")
      .abbrev('r')
      .full("revision")
      .help("commitish of artifact to deploy")
      .finalize();

  cmdr.option("environment")
      .abbrev('e')
      .full("env")
      .help("which env to deploy to")
      .finalize();

  cmdr.option("dry-run")
      .abbrev('d')
      .full("dry-run")
      .boolean()
      .help("do not actually run deployment")
      .finalize();

  cmdr.option("codebase")
      .position(0)
      .required()
      .help("which code base to deploy")
      .finalize();

  auto repr = cmdr.help();

  EXPECT_EQ(repr,
            "deploy(0.1.2): deploy code\n"
            "  -h, --help                 display this help message\n"
            "  -V, --version              display version information\n"
            "  -v, --verbose              show which commands are run\n"
            "  -r, --revision <revision>  commitish of artifact to deploy\n"
            "  -e, --env <environment>    which env to deploy to\n"
            "  -d, --dry-run              do not actually run deployment\n"
            "  [codebase]                 which code base to deploy\n");
}

TEST(cmdr, help_process) {
  cmdr::cmdr cmdr{"deploy", "0.1.2", "deploy code"};
  cmdr.option("verbose")
      .abbrev('v')
      .full("verbose")
      .boolean()
      .help("show which commands are run")
      .finalize();

  cmdr.option("revision")
      .abbrev('r')
      .full("revision")
      .help("commitish of artifact to deploy")
      .finalize();

  cmdr.option("environment")
      .abbrev('e')
      .full("env")
      .help("which env to deploy to")
      .finalize();

  cmdr.option("dry-run")
      .abbrev('d')
      .full("dry-run")
      .boolean()
      .help("do not actually run deployment")
      .finalize();

  cmdr.option("codebase")
      .position(0)
      .required()
      .help("which code base to deploy")
      .finalize();

  EXPECT_EXIT(
      { cmdr.parse({"deploy", "-h"}); }, testing::ExitedWithCode(0), "");
}

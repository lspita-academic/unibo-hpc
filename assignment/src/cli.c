#include "cli.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "safety.h"

char* check_arg(char* arg, char* arg_name) {
  safe_assert(
      strlen(arg) > 0, "Invalid argument %s: value is empty\n", arg_name
  );
  return arg;
}

CliArgs parse_cli_args(int argc, char* argv[]) {
  safe_assert(argc == 4, "Usage: %s K input_file output_file\n", argv[0]);

  char* k_str = check_arg(argv[1], "K");
  char* input_file_path = check_arg(argv[2], "input_file");
  char* output_file_path = check_arg(argv[3], "output_file");

  /*
   * cast argument to unsigned long, without crashing on failure.
   * https://man7.org/linux/man-pages/man3/strtoul.3.html
   * In particular, if *nptr is not '\0' but **endptr is '\0' on return, the
   * entire string is valid.
   */
  char* endptr;
  int64_t k = strtol(k_str, &endptr, 10);
  safe_assert(
      *endptr == '\0' && k >= 0,
      "Invalid argument K: value must be a non-negative number\n"
  );

  return (CliArgs){
      .k = k,
      .input_file_path = input_file_path,
      .output_file_path = output_file_path,
  };
}

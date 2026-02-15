// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#include <stdio.h>
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <stdlib.h>

#include "cli.h"

#define MAX_ITER 100
#define TOL 1e-5

int main(int argc, char* argv[]) {
  CliArgs args = parse_cli_args(argc, argv);

  printf("K: %lu\n", args.k);
  printf("Input file: %s\n", args.input_file_path);
  printf("Output file: %s\n", args.output_file_path);

  return EXIT_SUCCESS;
}

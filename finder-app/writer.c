#include <stdio.h>
#include <syslog.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    syslog(LOG_ERR, "Wrong number of arguments");
    fprintf(stderr, "Wrong number of arguments\n");
    fprintf(stderr, "Usage: ./writer <file_name> <text_to_write>\n");
    return 1;
  }

  openlog(argv[0], LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);

  const char *filename = argv[1];
  const char *textToWrite = argv[2];

  FILE *fp = fopen(filename, "w");

  if (fp == NULL) {
    syslog(LOG_ERR, "Error opening %s", filename);
    fprintf(stderr, "Error opening %s\n", filename);
    return 1;
  }

  syslog(LOG_DEBUG, "Writing %s to %s\n", textToWrite, filename);

  fprintf(fp, "%s", textToWrite);
  fclose(fp);

  return 0;
}
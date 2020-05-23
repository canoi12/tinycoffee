#ifndef TC_FILESYSTEM_H
#define TC_FILESYSTEM_H

#include "tinycoffee.h"

// #include "external/zip/src/miniz.h"
#include "external/zip/src/zip.h"

#ifndef TCDEF
  #define TCDEF extern
#endif

#include <sys/stat.h>

typedef enum {
  TC_FILE = 0,
  TC_DIRECTORY,
  TC_SYMLINK
} FILE_TYPE;

typedef enum {
  TC_WRITE = 0,
  TC_APPEND,
  TC_BINARY
} TC_WRITE_MODE;

typedef struct {
  FILE_TYPE type;
  size_t size;
  FILE *fp;
} tc_file;

TCDEF char *tc_fs_read_file(const char *filename, size_t *size, tc_bool readBinary);
TCDEF char *tc_fs_read_file_from_zip(const char *zipName, const char *filename, size_t *size);
TCDEF void tc_fs_write_file(const char *filename, const char *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_fs_write_file_to_zip(const char *zipName, const char *filename, const char *text, size_t size, TC_WRITE_MODE mode);

TCDEF void tc_fs_delete_file(const char *filename);
TCDEF void tc_fs_delete_file_from_zip(const char *zipName, const char *filename);
TCDEF tc_bool tc_fs_file_exists(const char *filename);
TCDEF tc_bool tc_fs_file_exists_in_zip(const char *zipName, const char *filename);

TCDEF void tc_fs_mkdir(const char *path);
TCDEF void tc_fs_mkdir_in_zip(const char *zipName, const char *path);
TCDEF void tc_fs_rmdir(const char *path);
TCDEF void tc_fs_rmdir_in_zip(const char *zipName, const char *path);
TCDEF tc_bool tc_fs_directory_exists(const char *path);
TCDEF tc_bool tc_fs_directory_exists_in_zip(const char *zipName, const char *filename);

TCDEF tc_file tc_fs_get_info(const char *path);

TCDEF tc_file tc_fs_open_file(const char *filename);
TCDEF void tc_fs_close_file(tc_file *file);

#endif

#if defined(TC_FILESYSTEM_IMPLEMENTATION)

TCDEF char * tc_fs_read_file(const char *filename, size_t *outSize, tc_bool readBinary) {
  char read[2] = "r";
  if (readBinary) read[1] = 'b';
  FILE *fp;
  fp = fopen(filename, read);
  if (!fp) {
    ERROR("FS", "File not found '%s'\n", filename);
    return NULL;
  }
  size_t size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (outSize) *outSize = size;

  char *buffer = malloc(size + 1);
  int _ = fread(buffer, 1, size, fp); (void)_;
  buffer[size] = '\0';
  fclose(fp);

  return buffer;
}

TCDEF char * tc_fs_read_file_from_zip(const char *zipName, const char *filename, size_t *outSize) {
  void *buffer;
  size_t size;
  struct zip_t *zip = zip_open(zipName, 0, 'r');
  ASSERT(zip != NULL);
  tc_bool failed = zip_entry_open(zip, filename);
  if (failed < 0) {
    ERROR("FS", "Failed to open file '%s'\n", filename);
    zip_close(zip);
    return NULL;
  }
  zip_entry_read(zip, &buffer, &size);
  if (outSize) *outSize = size;
  zip_entry_close(zip);
  zip_close(zip);

  return (char*)buffer;
}

TCDEF void tc_fs_write_file(const char *filename, const char *text, size_t size, TC_WRITE_MODE mode) {
  char writeMode[2] = "w\0";
  if (mode & TC_APPEND) writeMode[0] = 'a';
  if (mode & TC_BINARY) writeMode[1] = 'r';

  size_t bufSize = strlen(text);
  if (size > 0) bufSize = size;
  FILE *fp;
  fp = fopen(filename, writeMode);
  if (!fp) {
    ERROR("FS", "Cannot create file '%s'\n", filename);
    return;
  }
  // int write = fwrite(text, 1, bufSize, fp);
  for (int i = 0; i < bufSize; i++) {
    int write = fputc(text[i], fp);
    if (write == EOF) {
      ERROR("FS", "Failed to write file '%s'\n", filename);
      fclose(fp);
      return;
    }
  }
  // if (write != bufSize) {
  //   ERROR("FS", "Failed to write file '%s'\n", filename);
  //   fclose(fp);
  //   return;
  // }

  fclose(fp);
}

TCDEF void tc_fs_write_file_to_zip(const char *zipName, const char *filename, const char *text, size_t size, TC_WRITE_MODE mode) {
  char writeMode = 'w';
  if (mode & TC_APPEND) writeMode = 'a';
  struct zip_t *zip = zip_open(zipName, 0, writeMode);
  ASSERT(zip != NULL);
  tc_bool failed = zip_entry_open(zip, filename);
  if (failed < 0) {
    ERROR("FS", "Failed to open file '%s' from zip '%s'", filename, zipName);
    zip_close(zip);
    return;
  }
  zip_entry_write(zip, text, size);
  zip_entry_close(zip);
  zip_close(zip);
}

TCDEF void tc_fs_delete_file(const char *filename) {
  if (remove(filename)) {
    ERROR("FS", "Cannot remove file '%s'\n", filename);
  }
}

TCDEF tc_bool tc_fs_file_exists(const char *filename) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (!fp) {
    return TC_FALSE;
  }
  fclose(fp);
  return TC_TRUE;
}

TCDEF tc_bool tc_fs_file_exists_in_zip(const char* zipName, const char *filename) {
  struct zip_t *zip = zip_open(zipName, 0, 'r');
  tc_bool failed = zip_entry_open(zip, filename);
  if (failed < 0) {
    zip_close(zip);
    return TC_FALSE;
  }
  zip_entry_close(zip);
  zip_close(zip);
  return TC_TRUE;
}

TCDEF void tc_fs_mkdir(const char *path) {
#if defined(__unix__)
	int check = mkdir(path, 0775);
#else
	int check = mkdir(path);
#endif
	if (check)
	{
		ERROR("FS", "Failed to create folder '%s'\n", path);
	}
}

TCDEF void tc_fs_mkdir_in_zip(const char *zipName, const char *path) {

}

TCDEF void tc_fs_rmdir(const char *path) {
  tc_bool check = rmdir(path);
  if (check) {
    ERROR("FS", "Failed to remove directory '%s'\n", path);
  }
}

TCDEF tc_bool tc_fs_directory_exists(const char *path) {
  struct stat sb;
	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) return 1;

	return 0;
}

TCDEF tc_bool tc_fs_directory_exists_in_zip(const char *zipName, const char *path) {
  struct zip_t *zip = zip_open(zipName, 0, 'r');
  ASSERT(zip != NULL);
  tc_bool result = zip_entry_open(zip, path);
  if (result < 0) {
    zip_close(zip);
    return TC_FALSE;
  }
  zip_entry_close(zip);
  zip_close(zip);
  return TC_TRUE;
}

TCDEF tc_file tc_fs_get_info(const char *path) {
  tc_file file = {0};
  if (tc_fs_file_exists(path)) {
    file.type = TC_FILE;
    FILE *fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    file.size = ftell(fp);
    fclose(fp);
  }

  return file;
}

#endif

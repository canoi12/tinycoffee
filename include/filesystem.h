#ifndef TICO_FILESYSTEM_H
#define TICO_FILESYSTEM_H

#include "tico.h"

typedef enum {
  TIC_FILE = 0,
  TIC_DIRECTORY,
  TIC_SYMLINK
} TIC_FILE_TYPE_;

typedef enum {
  TIC_WRITE = 0,
  TIC_APPEND,
  TIC_BINARY
} TIC_WRITE_MODE_;

typedef struct tc_Filesystem tc_Filesystem;

typedef unsigned char*(*ReadFunction)(tc_Filesystem*, const char*, size_t*);
typedef void(*WriteFunction)(tc_Filesystem*, const char*, const char*,  size_t, TIC_WRITE_MODE_);
typedef tc_bool(*FileExistsFunction)(tc_Filesystem*, const char*);

struct tc_Filesystem {
  char projectPath[256];
  char exePath[256];
  ReadFunction readFile;
  WriteFunction writeFile;
  FileExistsFunction fileExists;
  // unsigned char* (*readFile)(const unsigned char*, size_t*);
  // void (*writeFile)(const unsigned char*, const unsigned char*, size_t, TIC_WRITE_MODE_);
  // tc_bool (*fileExists)(const unsigned char*);
  tc_bool packed;
};

TIC_API int tico_filesystem_init(tc_Filesystem *fs);
TIC_API INTERNAL_API(void tico_filesystem_set_packed, tc_Filesystem *fs, tc_bool packed);
TIC_API INTERNAL_API(tc_bool tico_filesystem_is_packed, tc_Filesystem *fs);

TIC_API INTERNAL_API(void tico_filesystem_set_path, tc_Filesystem *fs, const char *path);
TIC_API INTERNAL_API(unsigned char* tico_filesystem_get_path, tc_Filesystem *fs);
TIC_API INTERNAL_API(void tico_filesystem_resolve_path, tc_Filesystem *fs, char *outName, const char *filename);
TIC_API INTERNAL_API(unsigned char* tico_filesystem_get_exe_path, tc_Filesystem *fs);

TIC_API INTERNAL_API(unsigned char *tico_filesystem_read_internal_file, tc_Filesystem *fs, const char *filename, size_t *outSize);
TIC_API INTERNAL_API(unsigned char *tico_filesystem_read_external_file, tc_Filesystem *fs, const char *filename, size_t *outSize);

TIC_API INTERNAL_API(void tico_filesystem_write_internal_file, tc_Filesystem *fs, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode);
TIC_API INTERNAL_API(void tico_filesystem_write_external_file, tc_Filesystem *fs, const char *filename, const char* text, size_t size, TIC_WRITE_MODE_ mode);

TIC_API INTERNAL_API(tc_bool tico_filesystem_internal_file_exists, tc_Filesystem *fs, const char *filename);
TIC_API INTERNAL_API(tc_bool tico_filesystem_external_file_exists, tc_Filesystem *fs, const char *filename);

TIC_API INTERNAL_API(unsigned char *tico_filesystem_read_file, tc_Filesystem *fs, const char *filename, size_t *outSize);
TIC_API INTERNAL_API(void tico_filesystem_write_file, tc_Filesystem *fs, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode);
TIC_API INTERNAL_API(void tico_filesystem_delete_file, tc_Filesystem *fs, const char *filename);
TIC_API INTERNAL_API(tc_bool tico_filesystem_file_exists, tc_Filesystem *fs, const char *filename);
TIC_API INTERNAL_API(void tico_filesystem_mkdir, tc_Filesystem *fs, const char *path);
TIC_API INTERNAL_API(void tico_filesystem_rmdir, tc_Filesystem *fs, const char *path);

#endif // TICO_FILESYSTEM_H
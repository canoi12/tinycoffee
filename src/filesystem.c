#include "tico.h"

#include "zip/src/zip.h"
#include <sys/stat.h>

#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
#endif

static int getPathOnly(char *out, const char *filepath, int size) {
  if (!strchr(filepath, '/') && !strchr(filepath, '\\')) return 0;
  int i = 0;
  for (i = size; i > 0; i--) {
    char c = *(filepath + i);
    if (c == '\\' || c == '/') break;
  }
  memcpy(out, filepath, i);
  out[i] = '\0';
  return i;
}

static tc_bool isAbsolute(const char *path) {
  if (path[0] == '/') return tc_true;
  if (path[1] == ':' && path[2] == '\\') return tc_true;
  return tc_false;
}

int tico_filesystem_init(tc_Filesystem *fs) {
	strcpy(fs->projectPath, ".");
  fs->packed = tc_false;
  fs->readFile = &tico_filesystem_read_external_file_internal;
  fs->writeFile = &tico_filesystem_write_external_file_internal;
  fs->fileExists = &tico_filesystem_external_file_exists_internal;
  // tico_filesystem_set_packed_internal(fs, tc_false);

  // TRACELOG("%p", fs->fileExists);
#if _WIN32
  int len = GetModuleFileName(NULL, fs->exePath, sizeof(fs->exePath)-1);
  fs->exePath[len] = '\0';
#elif __linux__
  int len = readlink("/proc/self/exe", fs->exePath, sizeof(fs->exePath)-1);
  fs->exePath[len] = '\0';
#endif
  getPathOnly(fs->exePath, fs->exePath, strlen(fs->exePath));

  return 1;
}

void INTERNAL_API(tico_filesystem_set_packed, tc_Filesystem *fs, tc_bool packed) {
	fs->packed = packed;
  if (packed) {
    fs->readFile = &tico_filesystem_read_internal_file_internal;
    fs->writeFile = &tico_filesystem_write_internal_file_internal;
    fs->fileExists = &tico_filesystem_internal_file_exists_internal;
    return;
  }
  fs->readFile = &tico_filesystem_read_external_file_internal;
  fs->writeFile = &tico_filesystem_write_external_file_internal;
  fs->fileExists = &tico_filesystem_external_file_exists_internal;
}
tc_bool INTERNAL_API(tico_filesystem_is_packed, tc_Filesystem *fs) {
	return fs->packed;
}

void INTERNAL_API(tico_filesystem_set_path, tc_Filesystem *fs, const char *path) {
	if (!path) return;
  strcpy(fs->projectPath, path);
  
  int len = strlen(path) - 1;
  if (path[len] == '/') fs->projectPath[len] = '\0';
}
unsigned char* INTERNAL_API(tico_filesystem_get_path, tc_Filesystem *fs) {
	return (unsigned char*)fs->projectPath;
}

void INTERNAL_API(tico_filesystem_resolve_path, tc_Filesystem *fs, char *outName, const char *filename) {
	if (!isAbsolute(filename))  sprintf(outName, "%s/%s", fs->projectPath, filename);
  else strcpy(outName, filename);
}

unsigned char* INTERNAL_API(tico_filesystem_get_exe_path, tc_Filesystem *fs) {
	return fs->exePath;
}

unsigned char* INTERNAL_API(tico_filesystem_read_internal_file, tc_Filesystem *fs, const char *filename, size_t *outSize) {
	void *buffer;
  size_t size;
  char packName[1024];
  tico_filesystem_resolve_path_internal(fs, packName, PACK_NAME);
  struct zip_t *zip = zip_open(packName, 0, 'r');
  ASSERT(zip != NULL);
  tc_bool failed = zip_entry_open(zip, filename);
  if (failed < 0) {
    TRACEERR("Failed to open file '%s'", filename);
    zip_close(zip);
    return NULL;
  }
  zip_entry_read(zip, &buffer, &size);
  if (outSize) *outSize = size;
  zip_entry_close(zip);
  zip_close(zip);

  return (unsigned char*)buffer;
}
unsigned char* INTERNAL_API(tico_filesystem_read_external_file, tc_Filesystem *fs, const char *filename, size_t *outSize) {
	FILE *fp;
  char nFilename[1024];
  tico_filesystem_resolve_path_internal(fs, nFilename, filename);
  fp = fopen(nFilename, "rb");
  if (!fp) {
    TRACEERR("File not found '%s'", nFilename);
    return NULL;
  }
  size_t size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (outSize) *outSize = size;

  unsigned char *buffer = TIC_MALLOC(size + 1);
  int _ = fread(buffer, 1, size, fp); (void)_;
  buffer[size] = '\0';
  fclose(fp);

  return buffer;
}

void INTERNAL_API(tico_filesystem_write_internal_file, tc_Filesystem *fs, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode) {
	char writeMode = 'w';
  if (mode & TIC_APPEND) writeMode = 'a';
  char packName[1024];
  tico_filesystem_resolve_path_internal(fs, packName, PACK_NAME);
  struct zip_t *zip = zip_open(packName, 0, writeMode);
  ASSERT(zip != NULL);
  tc_bool failed = zip_entry_open(zip, filename);
  if (failed < 0) {
    TRACEERR("Failed to open file '%s'", filename);
    zip_close(zip);
    return;
  }
  zip_entry_write(zip, text, size);
  zip_entry_close(zip);
  zip_close(zip);
}
void INTERNAL_API(tico_filesystem_write_external_file, tc_Filesystem *fs, const char *filename, const char* text, size_t size, TIC_WRITE_MODE_ mode) {
	char writeMode[2] = "w\0";
  if (mode & TIC_APPEND) writeMode[0] = 'a';
  if (mode & TIC_BINARY) writeMode[1] = 'b';

  size_t bufSize = strlen(text);
  if (size > 0) bufSize = size;

  FILE *fp;
  char nFilename[1024];
  tico_filesystem_resolve_path_internal(fs, nFilename, filename);

  fp = fopen(nFilename, writeMode);
  if (!fp) {
    TRACEERR("Cannot create file '%s'", filename);
    return;
  }
  int write = fwrite(text, 1, bufSize, fp);
  if (!write) {
    TRACEERR("Failed to write file '%s'", filename);
  }

  fclose(fp);
}

tc_bool INTERNAL_API(tico_filesystem_internal_file_exists, tc_Filesystem *fs, const char *filename) {
	char packName[1024];
  tico_filesystem_resolve_path_internal(fs, packName, PACK_NAME);

  struct zip_t *zip = zip_open(packName, 0, 'r');
  ASSERT(zip != NULL);
  tc_bool result = zip_entry_open(zip, filename);
  if (result < 0) {
    zip_close(zip);
    return tc_false;
  }
  zip_entry_close(zip);
  zip_close(zip);
  return tc_true;
}
tc_bool INTERNAL_API(tico_filesystem_external_file_exists, tc_Filesystem *fs, const char *filename) {
	char nFilename[1024];
  tico_filesystem_resolve_path_internal(fs, nFilename, filename);

  FILE *fp;
  fp = fopen(nFilename, "r");
  if (!fp) {
    return tc_false;
  }
  fclose(fp);
  return tc_true;
}

unsigned char* INTERNAL_API(tico_filesystem_read_file, tc_Filesystem *fs, const char *filename, size_t *outSize) {
  ASSERT(fs->readFile != NULL);
	return fs->readFile(fs, filename, outSize);
}
void INTERNAL_API(tico_filesystem_write_file, tc_Filesystem *fs, const char *filename, const char *text, size_t size, TIC_WRITE_MODE_ mode) {
	// tico_filesystem_write_file_inter
  ASSERT(fs->writeFile != NULL);
	fs->writeFile(fs, filename, text, size, mode);
}
tc_bool INTERNAL_API(tico_filesystem_file_exists, tc_Filesystem *fs, const char *filename) {
  // if (!fs->fileExists) return 0;
  ASSERT(fs->fileExists != NULL);
	return fs->fileExists(fs, filename);
}
void INTERNAL_API(tico_filesystem_delete_file, tc_Filesystem *fs, const char *filename) {

}
void INTERNAL_API(tico_filesystem_mkdir, tc_Filesystem *fs, const char *path) {
	char nPath[1024];
  tico_filesystem_resolve_path_internal(fs, nPath, path);
#if defined(__unix__)
  int check = mkdir(nPath, 0775);
#else
  int check = mkdir(nPath);
#endif
  if (check)
  {
    TRACEERR("Failed to create folder '%s'", path);
  }
}
void INTERNAL_API(tico_filesystem_rmdir, tc_Filesystem *fs, const char *path) {
	char nPath[1024];
  tico_filesystem_resolve_path_internal(fs, nPath, path);
  tc_bool check = rmdir(nPath);
  if (check) {
    TRACEERR("Failed to remove directory '%s'", path);
  }
}
#include "tico.h"

#include "external/zip/src/zip.h"
#include <sys/stat.h>

#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
#endif

#define PACK_NAME "data.pack"

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

int tic_filesystem_init(tc_Filesystem *fs) {
  strcpy(fs->projectPath, ".");
  fs->packed = tc_false;
  fs->readFile = &tic_filesystem_read_external_file;
  fs->writeFile = &tic_filesystem_write_external_file;
  fs->fileExists = &tic_filesystem_external_file_exists;
#if _WIN32
  int len = GetModuleFileName(NULL, fs->exePath, sizeof(fs->exePath)-1);
  fs->exePath[len] = '\0';
#elif __linux__
  int len = readlink("/proc/self/exe", fs->exePath, sizeof(fs->exePath)-1);
  fs->exePath[len] = '\0';
#endif
  getPathOnly(fs->exePath, fs->exePath, strlen(fs->exePath));
}

void tic_filesystem_resolve_path(char *outName, const tc_uint8 *filename) {
  if (!isAbsolute(filename))  sprintf(outName, "%s/%s", Core.fs.projectPath, filename);
  else strcpy(outName, filename);
}

void tic_filesystem_set_path(const char *path) {
  if (!path) return;
  strcpy(Core.fs.projectPath, path);
  
  int len = strlen(path) - 1;
  if (path[len] == '/') Core.fs.projectPath[len] = '\0';
}

tc_uint8* tic_filesystem_get_path() {
  return Core.fs.projectPath;
}

void tic_filesystem_set_packed(tc_bool packed) {
  Core.fs.packed = packed;
  if (packed) {
    Core.fs.readFile = &tic_filesystem_read_internal_file;
    Core.fs.writeFile = &tic_filesystem_write_internal_file;
    Core.fs.fileExists = &tic_filesystem_internal_file_exists;
    return;
  }
  Core.fs.readFile = &tic_filesystem_read_external_file;
  Core.fs.writeFile = &tic_filesystem_write_external_file;
  Core.fs.fileExists = &tic_filesystem_external_file_exists;
}

tc_uint8* tic_filesystem_get_exe_path() {
  return Core.fs.exePath;
}

tc_uint8* tic_filesystem_read_internal_file(const tc_uint8 *filename, size_t *outSize) {
  void *buffer;
  size_t size;
  char packName[1024];
  tic_filesystem_resolve_path(packName, PACK_NAME);
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

  return (tc_uint8*)buffer;
}

tc_uint8* tic_filesystem_read_external_file(const tc_uint8 *filename, size_t *outSize) {
  FILE *fp;
  char nFilename[1024];
  tic_filesystem_resolve_path(nFilename, filename);
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

  tc_uint8 *buffer = TIC_MALLOC(size + 1);
  int _ = fread(buffer, 1, size, fp); (void)_;
  buffer[size] = '\0';
  fclose(fp);

  return buffer;
}

void tic_filesystem_write_internal_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TIC_WRITE_MODE mode) {
  tc_uint8 writeMode = 'w';
  if (mode & TIC_APPEND) writeMode = 'a';
  char packName[1024];
  tic_filesystem_resolve_path(packName, PACK_NAME);
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

void tic_filesystem_write_external_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TIC_WRITE_MODE mode) {
  tc_uint8 writeMode[2] = "w\0";
  if (mode & TIC_APPEND) writeMode[0] = 'a';
  if (mode & TIC_BINARY) writeMode[1] = 'b';

  size_t bufSize = strlen(text);
  if (size > 0) bufSize = size;

  FILE *fp;
  char nFilename[1024];
  tic_filesystem_resolve_path(nFilename, filename);

  fp = fopen(nFilename, writeMode);
  if (!fp) {
    TRACEERR("Cannot create file '%s'", filename);
    return;
  }
  int write = fwrite(text, 1, bufSize, fp);
  if (!write) {
    TRACEERR("Failed to write file '%s'", filename);
  }
  // for (int i = 0; i < bufSize; i++) {
  //   int write = fputc(text[i], fp);
  //   if (write == EOF) {
  //     ERROR("Failed to write file '%s'", filename);
  //     fclose(fp);
  //     return;
  //   }
  // }

  fclose(fp);
}

tc_bool tic_filesystem_internal_file_exists(const tc_uint8 *filename) {
  char packName[1024];
  tic_filesystem_resolve_path(packName, PACK_NAME);

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
tc_bool tic_filesystem_external_file_exists(const tc_uint8 *filename) {
  char nFilename[1024];
  tic_filesystem_resolve_path(nFilename, filename);

  FILE *fp;
  fp = fopen(nFilename, "r");
  if (!fp) {
    return tc_false;
  }
  fclose(fp);
  return tc_true;
}

unsigned char *tic_filesystem_read_file(const tc_uint8 *filename, size_t *outSize) {
//   if (Core.packed) {
//     return tc_read_internal_file(filename, outSize);
//   } else {
//     return tc_read_external_file(filename, outSize);
//   }
  return Core.fs.readFile(filename, outSize);
//   return NULL;
}

void tic_filesystem_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TIC_WRITE_MODE mode) {
//   if (Core.packed) {
//     tc_write_internal_file(filename, text, size, mode);
//   } else {
//     tc_write_external_file(filename, text, size, mode);
//   }
  Core.fs.writeFile(filename, text, size, mode);
}

tc_bool tic_filesystem_file_exists(const tc_uint8 *filename) {
//   if (Core.packed) {
//     return tc_internal_file_exists(filename);
//   } else {
//     return tc_external_file_exists(filename);
//   }
  return Core.fs.fileExists(filename);
}
void tic_filesystem_mkdir(const tc_uint8 *path) {
  char nPath[1024];
  tic_filesystem_resolve_path(nPath, path);
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

void tic_filesystem_rmdir(const tc_uint8 *path) {
  char nPath[1024];
  tic_filesystem_resolve_path(nPath, path);
  tc_bool check = rmdir(nPath);
  if (check) {
    TRACEERR("Failed to remove directory '%s'", path);
  }
}

/***********************
 *  JSON
 ***********************/

cJSON *tic_json_open(const char *filename) {
  char *content = tic_filesystem_read_file(filename, NULL);
  cJSON *parsed = cJSON_Parse(content);
  free(content);
  if (!parsed) {
    const char *err = cJSON_GetErrorPtr();
    if (err != NULL) {
      TRACEERR("Failed to load json '%s': error before %s", filename, err);
    }
  }
  return parsed;
}
cJSON *tic_json_parse(const char *jsonStr) {
  cJSON *parsed = cJSON_Parse(jsonStr);
  if (!parsed) {
    const char *err = cJSON_GetErrorPtr();
    if (err != NULL) {
      TRACEERR("Failed to parse json: error before %s", err);
    }
  }
  return parsed;
}
void tic_json_save(const char *filename, cJSON* const json) {
  char *string = cJSON_Print(json);
  size_t size = strlen(string);
  if (!string) {
    TRACEERR("Failed to save json: '%s'", filename);
    return;
  }
  tic_filesystem_write_file(filename, string, size, 0);
}

char *tic_json_print(cJSON* const json) {
  return cJSON_Print(json);
}

cJSON* tic_json_create() {
  return cJSON_CreateObject();
}

cJSON* tic_json_create_array() {
  return cJSON_CreateArray();
}

void tic_json_delete(cJSON* json) {
  cJSON_Delete(json);
}

/***********
 * String
 ***********/

tc_bool tic_json_is_string(cJSON* const json, char *name) {
  cJSON *jsonName = cJSON_GetObjectItem(json, name);
  if (jsonName) {
    return cJSON_IsString(jsonName);
  }
  return tc_false;
}

char *tic_json_get_string(cJSON* const json, char *name) {
  return tic_json_get_opt_string(json, name, NULL);
}

char *tic_json_get_opt_string(cJSON* const json, char *name, char *optVal) {
  cJSON *jsonName = cJSON_GetObjectItem(json, name);
  if (cJSON_IsString(jsonName) && (jsonName->valuestring != NULL)) {
    return jsonName->valuestring;
  }
  return optVal;
}
cJSON *tic_json_set_string(cJSON* const json, char *name, char *value) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    cJSON_SetValuestring(val, value);
    return val;
  }
  cJSON *strVal = cJSON_AddStringToObject(json, name, value);
  if (strVal == NULL) TRACEERR("failed to add string '%s' to json ['%s']", value, name);
  return strVal;
}

/**********
 * Number
 **********/

tc_bool tic_json_is_number(cJSON* const json, char *name) {
  cJSON *value = cJSON_GetObjectItem(json, name);
  if (value) return cJSON_IsNumber(value);
  return tc_false;
}

float tic_json_get_number(cJSON* const json, char *name) {
  return tic_json_get_opt_number(json, name, -1);
}
float tic_json_get_opt_number(cJSON* const json, char *name, float optVal) {
  cJSON *value = cJSON_GetObjectItem(json, name);
  if (cJSON_IsNumber(value)) {
    return value->valuedouble;
  }
  return optVal;
}
cJSON *tic_json_set_number(cJSON* const json, char *name, float value) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    val->valuedouble = value;
    return val;
  }
  cJSON *nVal = cJSON_AddNumberToObject(json, name, value);
  if (nVal == NULL) TRACEERR("Failed to add number '%f' to json ['%s']", value, name);
  return nVal;
}

/***********
 * Boolean
 ***********/

tc_bool tic_json_is_boolean(cJSON* const json, char *name) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) return cJSON_IsBool(val);
  return tc_false;
}

int tic_json_get_boolean(cJSON* const json, char *name) {
  return tic_json_get_opt_boolean(json, name, tc_false);
}
int tic_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (cJSON_IsBool(val)) {
    return val->valueint;
  }
  return optVal;
}
cJSON* tic_json_set_boolean(cJSON* const json, char *name, tc_bool value) {
  cJSON *boolVal = cJSON_AddBoolToObject(json, name, value);
  if (boolVal == NULL) TRACEERR("Failed to add boolean '%s' to json ['%s']", value == 0 ? "false" : "true", name);
  return boolVal;
}

/***********
 * Array
 ***********/

tc_bool tic_json_is_array(cJSON* const json, char *name) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (array) return cJSON_IsArray(array);
  return tc_false;
}

cJSON* tic_json_get_array(cJSON* const json, char *name) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (cJSON_IsArray(array)) {
    return array;
  }
  return NULL;
}

cJSON* tic_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (val) {
    // TRACELOG("testando");
    tic_json_delete(val);
  }
  if (jsonArray) {
    cJSON_AddItemToObject(json, name, jsonArray);
  } else {
    return cJSON_AddArrayToObject(json, name);
  }
}

int tic_json_get_array_size(const cJSON *json) {
  return cJSON_GetArraySize(json);
}

/*************
 * Object
 *************/

tc_bool tic_json_is_object(cJSON* const json, char *name) {
  cJSON *obj = cJSON_GetObjectItem(json, name);
  if (obj) return cJSON_IsObject(obj);
  return tc_false;
}

cJSON* tic_json_get_object(cJSON* const json, char *name) {
  cJSON *obj = cJSON_GetObjectItem(json, name);
  if (cJSON_IsObject(obj)) return obj;
  return NULL;
}
cJSON* tic_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj) {
    if (jsonObj) {
      cJSON_AddItemToObject(json, name, jsonObj);
      return jsonObj;
    } else {
      return cJSON_AddObjectToObject(json, name);
    }
}
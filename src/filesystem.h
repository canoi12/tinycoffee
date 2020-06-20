#ifndef TICO_FILESYSTEM_H
#define TICO_FILESYSTEM_H

#include "tinycoffee.h"

#include "external/cjson/cJSON.h"
#include "external/zip/src/zip.h"
#include <sys/stat.h>

#if defined(__unix__) || defined(__linux__)
#include <unistd.h>
#endif

#define PACK_NAME "data.pack"

typedef enum {
  TC_FILE = 0,
  TC_DIRECTORY,
  TC_SYMLINK
} TC_FILE_TYPE;

typedef enum {
  TC_WRITE = 0,
  TC_APPEND,
  TC_BINARY
} TC_WRITE_MODE;

typedef struct {
  FILE *fp;
  tc_uint8 *name;

} tc_File;

TCDEF void tc_resolve_path(char *outName, const tc_uint8 *filename);

TCDEF tc_uint8 *tc_read_internal_file(const tc_uint8 *filename, size_t *outSize);
TCDEF tc_uint8 *tc_read_external_file(const tc_uint8 *filename, size_t *outSize);

TCDEF void tc_write_internal_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_write_external_file(const tc_uint8 *filename, const tc_uint8* text, size_t size, TC_WRITE_MODE mode);

TCDEF tc_bool tc_internal_file_exists(const tc_uint8 *filename);
TCDEF tc_bool tc_external_file_exists(const tc_uint8 *filename);

TCDEF tc_uint8 *tc_read_file(const tc_uint8 *filename, size_t *outSize);
TCDEF void tc_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode);
TCDEF void tc_delete_file(const tc_uint8 *filename);
TCDEF tc_bool tc_file_exists(const tc_uint8 *filename);
TCDEF void tc_mkdir(const tc_uint8 *path);
TCDEF void tc_rmdir(const tc_uint8 *path);

TCDEF void tc_filesystem_set_path(const char *path);
TCDEF tc_uint8* tc_filesystem_get_path();

/**********************
 * JSON
 **********************/

TCDEF cJSON* tc_json_open(const char *filename);
TCDEF cJSON* tc_json_parse(const char *jsonStr);
TCDEF void tc_json_save(const char *filename, cJSON* const json);
TCDEF char* tc_json_print(cJSON* const json);

TCDEF cJSON* tc_json_create();
TCDEF void tc_json_delete(cJSON* const json);

TCDEF char* tc_json_get_string(cJSON* const json, char *name);
TCDEF char* tc_json_get_opt_string(cJSON* const json, char *name, char *optVal);
TCDEF cJSON* tc_json_set_string(cJSON* const json, char *name, char *value);

TCDEF float tc_json_get_number(cJSON* const json, char *name);
TCDEF float tc_json_get_opt_number(cJSON* const json, char *name, float optVal);
TCDEF cJSON* tc_json_set_number(cJSON* const json, char *name, float value);

TCDEF int tc_json_get_boolean(cJSON* const json, char *name);
TCDEF int tc_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal);
TCDEF cJSON* tc_json_set_boolean(cJSON* const json, char *name, tc_bool value);

TCDEF cJSON* tc_json_get_array(cJSON* const json, char *name);
TCDEF cJSON* tc_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray);
TCDEF int tc_json_get_array_size(const cJSON* json);

TCDEF cJSON* tc_json_get_object(cJSON* const json, char *name);
TCDEF cJSON* tc_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj);

#endif /* TICO_FILESYSTEM_H */

#if defined(TICO_FILESYSTEM_IMPLEMENTATION)

static char projectPath[256];

TCDEF void tc_resolve_path(char *outName, const tc_uint8 *filename) {
  sprintf(outName, "%s/%s", projectPath, filename);
}

TCDEF tc_uint8* tc_read_internal_file(const tc_uint8 *filename, size_t *outSize) {
  void *buffer;
  size_t size;
  char packName[1024];
  tc_resolve_path(packName, PACK_NAME);
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

TCDEF tc_uint8* tc_read_external_file(const tc_uint8 *filename, size_t *outSize) {
  FILE *fp;
  char nFilename[1024];
  tc_resolve_path(nFilename, filename);
  fp = fopen(nFilename, "rb");
  if (!fp) {
    TRACEERR("File not found '%s'", filename);
    return NULL;
  }
  size_t size;
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (outSize) *outSize = size;

  tc_uint8 *buffer = malloc(size + 1);
  int _ = fread(buffer, 1, size, fp); (void)_;
  buffer[size] = '\0';
  fclose(fp);

  return buffer;
}

TCDEF void tc_write_internal_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode) {
  tc_uint8 writeMode = 'w';
  if (mode & TC_APPEND) writeMode = 'a';
  char packName[1024];
  tc_resolve_path(packName, PACK_NAME);
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

TCDEF void tc_write_external_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode) {
  tc_uint8 writeMode[2] = "w\0";
  if (mode & TC_APPEND) writeMode[0] = 'a';
  if (mode & TC_BINARY) writeMode[1] = 'b';

  size_t bufSize = strlen(text);
  if (size > 0) bufSize = size;

  FILE *fp;
  char nFilename[1024];
  tc_resolve_path(nFilename, filename);

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

TCDEF tc_bool tc_internal_file_exists(const tc_uint8 *filename) {
  char packName[1024];
  tc_resolve_path(packName, PACK_NAME);

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
TCDEF tc_bool tc_external_file_exists(const tc_uint8 *filename) {
  char nFilename[1024];
  tc_resolve_path(nFilename, filename);

  FILE *fp;
  fp = fopen(nFilename, "r");
  if (!fp) {
    return tc_false;
  }
  fclose(fp);
  return tc_true;
}

TCDEF unsigned char *tc_read_file(const tc_uint8 *filename, size_t *outSize) {
  if (Core.packed) {
    return tc_read_internal_file(filename, outSize);
  } else {
    return tc_read_external_file(filename, outSize);
  }
  return NULL;
}

TCDEF void tc_write_file(const tc_uint8 *filename, const tc_uint8 *text, size_t size, TC_WRITE_MODE mode) {
  if (Core.packed) {
    tc_write_internal_file(filename, text, size, mode);
  } else {
    tc_write_external_file(filename, text, size, mode);
  }
}

TCDEF tc_bool tc_file_exists(const tc_uint8 *filename) {
  if (Core.packed) {
    return tc_internal_file_exists(filename);
  } else {
    return tc_external_file_exists(filename);
  }
}
TCDEF void tc_mkdir(const tc_uint8 *path) {
  char nPath[1024];
  tc_resolve_path(nPath, path);
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

TCDEF void tc_rmdir(const tc_uint8 *path) {
  char nPath[1024];
  tc_resolve_path(nPath, path);
  tc_bool check = rmdir(nPath);
  if (check) {
    TRACEERR("Failed to remove directory '%s'", path);
  }
}

TCDEF void tc_filesystem_set_path(const char *path) {
  if (!path) return;
  strcpy(projectPath, path);
}

TCDEF tc_uint8 *tc_filesystem_get_path() {
  return Core.config.path;
}

/***********************
 *  JSON
 ***********************/

TCDEF cJSON *tc_json_open(const char *filename) {
  char *content = tc_read_file(filename, NULL);
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
TCDEF cJSON *tc_json_parse(const char *jsonStr) {
  return cJSON_Parse(jsonStr);
}
TCDEF void tc_json_save(const char *filename, cJSON* const json) {
  char *string = cJSON_Print(json);
  size_t size = strlen(string);
  if (!string) {
    TRACEERR("Failed to save json: '%s'", filename);
    return;
  }
  tc_write_file(filename, string, size, 0);
}

TCDEF char *tc_json_print(cJSON* const json) {
  return cJSON_Print(json);
}

TCDEF cJSON* tc_json_create() {
  return cJSON_CreateObject();
}
TCDEF void tc_json_delete(cJSON* const json) {
  cJSON_Delete(json);
}

TCDEF char *tc_json_get_string(cJSON* const json, char *name) {
  return tc_json_get_opt_string(json, name, NULL);
}

TCDEF char *tc_json_get_opt_string(cJSON* const json, char *name, char *optVal) {
  cJSON *jsonName = cJSON_GetObjectItem(json, name);
  if (cJSON_IsString(jsonName) && (jsonName->valuestring != NULL)) {
    return jsonName->valuestring;
  }
  return optVal;
}
TCDEF cJSON *tc_json_set_string(cJSON* const json, char *name, char *value) {
  cJSON *strVal = cJSON_AddStringToObject(json, name, value);
  if (strVal == NULL) TRACEERR("failed to add string '%s' to json ['%s']", value, name);
  return strVal;
}

TCDEF float tc_json_get_number(cJSON* const json, char *name) {
  return tc_json_get_opt_number(json, name, -1);
}
TCDEF float tc_json_get_opt_number(cJSON* const json, char *name, float optVal) {
  cJSON *value = cJSON_GetObjectItem(json, name);
  if (cJSON_IsNumber(value)) {
    return value->valuedouble;
  }
  return optVal;
}
TCDEF cJSON *tc_json_set_number(cJSON* const json, char *name, float value) {
  cJSON *nVal = cJSON_AddNumberToObject(json, name, value);
  if (nVal == NULL) TRACEERR("Failed to add number '%f' to json ['%s']", value, name);
  return nVal;
}

TCDEF int tc_json_get_boolean(cJSON* const json, char *name) {
  return tc_json_get_opt_boolean(json, name, tc_false);
}
TCDEF int tc_json_get_opt_boolean(cJSON* const json, char *name, tc_bool optVal) {
  cJSON *val = cJSON_GetObjectItem(json, name);
  if (cJSON_IsBool(val)) {
    return val->valueint;
  }
  return optVal;
}
TCDEF cJSON* tc_json_set_boolean(cJSON* const json, char *name, tc_bool value) {
  cJSON *boolVal = cJSON_AddBoolToObject(json, name, value);
  if (boolVal == NULL) TRACEERR("Failed to add boolean '%s' to json ['%s']", value == 0 ? "false" : "true", name);
  return boolVal;
}

TCDEF cJSON* tc_json_get_array(cJSON* const json, char *name) {
  cJSON *array = cJSON_GetObjectItem(json, name);
  if (cJSON_IsArray(array)) {
    return array;
  }
  return NULL;
}
TCDEF cJSON* tc_json_set_array(cJSON* const json, char *name, cJSON* const jsonArray) {
  if (jsonArray) {
    cJSON_AddItemToObject(json, name, jsonArray);
  } else {
    return cJSON_AddArrayToObject(json, name);
  }
}
TCDEF int tc_json_get_array_size(const cJSON *json) {
  return cJSON_GetArraySize(json);
}

TCDEF cJSON* tc_json_get_object(cJSON* const json, char *name) {
  cJSON *obj = cJSON_GetObjectItem(json, name);
  if (cJSON_IsObject(obj)) return obj;
  return NULL;
}
TCDEF cJSON* tc_json_set_object(cJSON* const json, char *name, cJSON* const jsonObj) {
    if (jsonObj) {
      cJSON_AddItemToObject(json, name, jsonObj);
      return jsonObj;
    } else {
      return cJSON_AddObjectToObject(json, name);
    }
}

#endif/* TICO_FILESYSTEM_IMPLEMENTATION */

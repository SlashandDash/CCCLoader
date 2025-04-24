#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zip.h>
#include <libgen.h>








// --- for unzip function ---



// Recursively create directories
int mkdir_recursive(const char *dir) {
    char tmp[512];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }

    return mkdir(tmp, 0755);
}

int unzip_file(const char *zip_path) {
    // Get directory of zip file
    char zip_path_copy[PATH_MAX];
    realpath(zip_path, zip_path_copy); // Resolve full path

    char zip_dir[PATH_MAX];
    snprintf(zip_dir, sizeof(zip_dir), "%s", zip_path_copy);
    dirname(zip_dir);  // This modifies zip_dir in-place

    // Open ZIP archive
    int err = 0;
    zip_t *za = zip_open(zip_path, 0, &err);
    if (!za) {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        fprintf(stderr, "Failed to open zip: %s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        return -1;
    }

    zip_int64_t num_entries = zip_get_num_entries(za, 0);
    printf("Extracting %lld entries to: %s\n", (long long)num_entries, zip_dir);

    for (zip_int64_t i = 0; i < num_entries; i++) {
        struct zip_stat st;
        if (zip_stat_index(za, i, 0, &st) != 0) continue;

        size_t name_len = strlen(st.name);
        if (name_len == 0) continue;

        // Compose full path: <zip_dir>/<st.name>
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", zip_dir, st.name);

        if (st.name[name_len - 1] == '/') {
            mkdir_recursive(full_path);
            continue;
        }

        zip_file_t *zf = zip_fopen_index(za, i, 0);
        if (!zf) continue;

        // Ensure parent dirs exist
        char dir_path[PATH_MAX];
        snprintf(dir_path, sizeof(dir_path), "%s", full_path);
        mkdir_recursive(dirname(dir_path));

        FILE *out = fopen(full_path, "wb");
        if (!out) {
            fprintf(stderr, "Failed to create file: %s\n", full_path);
            zip_fclose(zf);
            continue;
        }

        char buffer[4096];
        zip_int64_t bytes;
        while ((bytes = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytes, out);
        }

        fclose(out);
        zip_fclose(zf);
    }

    zip_close(za);
    printf("Done extracting to: %s\n", zip_dir);
    return 0;
}




// --- end unzip function ---


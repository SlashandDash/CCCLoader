#include "../include/raygui.h"
#include <raylib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <zip.h>
#include <sys/types.h>
#include <libgen.h>

#define WINDOWSIZE_X 1280
#define WINDOWSIZE_Y 720

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
    printf("Extracting %lld entries to: %s\n", num_entries, zip_dir);

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

//for curl function

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}


int main(){

    char *bepinex_url = "https://builds.bepinex.dev/projects/bepinex_be/577/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip";

    //initializing curl
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl){
        fprintf(stderr, "init failed\n");
        return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, bepinex_url);
    //curl init end


    //get username in linux

    char *username = getlogin();
    if (username != NULL) {
        printf("Username: %s\n", username);
    } else {
        perror("getlogin");
    }

    char outfilename[256] = "/home/USER/.local/share/Steam/steamapps/common/Crab Game/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip";
    snprintf(outfilename, sizeof(outfilename), "/home/%s/.local/share/Steam/steamapps/common/Crab Game/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip", username);

    //end username

    InitWindow(WINDOWSIZE_X, WINDOWSIZE_Y, "CCCLoader");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    bool buttonEnabled = true;



    while (!WindowShouldClose()){

        ClearBackground(WHITE);

        BeginDrawing();


        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        if (buttonEnabled){

            if (GuiButton((Rectangle){(WINDOWSIZE_X - 125)/2.0, WINDOWSIZE_Y-220, 125, 30}, "Install")){
                buttonEnabled = false;

                fp = fopen(outfilename, "wb");
                curl_easy_setopt(curl, CURLOPT_URL, bepinex_url);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                res = curl_easy_perform(curl);


                //curl clean up
                
                fclose(fp);
                curl_easy_cleanup(curl);

                unzip_file(outfilename);

            }
        }

        EndDrawing();
    }

    CloseWindow();



    return 0;
}


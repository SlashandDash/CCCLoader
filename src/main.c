#include "../include/raygui.h"
#include <raylib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <unistd.h>
#include <zip.h>


#define WINDOWSIZE_X 1280
#define WINDOWSIZE_Y 720



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

    char outfilename[256] = "/home/USER/Downloads/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip";

    snprintf(outfilename, sizeof(outfilename), "/home/%s/Downloads/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip", username);

    //end username




    InitWindow(WINDOWSIZE_X, WINDOWSIZE_Y, "Crab Game Installer");

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





            }
        }


        EndDrawing();
    }

    CloseWindow();

    //curl clean up

    curl_easy_cleanup(curl);
    fclose(fp);

    return 0;
}

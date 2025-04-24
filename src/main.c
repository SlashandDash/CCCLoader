#include "../include/raygui.h"
#include <raylib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../include/unzip_file.h"

#define WINDOWSIZE_X 1280
#define WINDOWSIZE_Y 720


//for curl function

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}


int main(){

    char outfilename[600];
    char crabGamePath[265];

    // detect os

    #ifdef __linux__
        const int os_number = 0;
    #elif defined(_WIN32)
        const int os_number = 1;
    #elif defined(__APPLE__) && defined(__MACH__)
        const int os_number = 2;
    #else
        const int os_number = 3;
    #endif

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


    //linux

    if (os_number == 0){


        char *username = getlogin();
        if (username != NULL) {
            printf("Username: %s\n", username);
        } else {
            perror("getlogin");
        }

        snprintf(outfilename, sizeof(outfilename), "/home/%s/.local/share/Steam/steamapps/common/Crab Game/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip", username);

        snprintf(crabGamePath, sizeof(crabGamePath), "/home/%s/.local/share/Steam/steamapps/common/Crab Game", username);

    }
    
    // windows

    else if (os_number == 1){

        // temp
        
        
        strcpy(outfilename, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Crab Game\\BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip");
        strcpy(crabGamePath, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Crab Game");

    }

    InitWindow(WINDOWSIZE_X, WINDOWSIZE_Y, "CCCLoader");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    bool buttonEnabled = true;

    while (!WindowShouldClose()){

        ClearBackground(WHITE);



        BeginDrawing();


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

                buttonEnabled = false;

            }
        }



        GuiTextBox((Rectangle){(WINDOWSIZE_X)/2.0 - 500,  WINDOWSIZE_Y-300, 1000, 30}, crabGamePath, 100, true);

        snprintf(outfilename, sizeof(crabGamePath), "%s/BepInEx_UnityIL2CPP_x64_ec79ad0_6.0.0-be.577.zip", crabGamePath);


        //printf("%s\n", outfilename);


        EndDrawing();
    }

    CloseWindow();



    return 0;
}


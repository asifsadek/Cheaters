#include "main.h"
#include "log.h"
#include "log.c"

#include <stdio.h>
#include <math.h>
#include <windows.h>

// Log examples:
// log_trace(const char *fmt, ...);
// log_debug(const char *fmt, ...);
// log_info(const char *fmt, ...);
// log_warn(const char *fmt, ...);
// log_error(const char *fmt, ...);
// log_fatal(const char *fmt, ...);

// Game Type Examples:
// First line of Eight-Ball Game:
// AE 52 1D FB 71 6C 09 08 CC AD 63 08 [05] 00 00 00 // First time to launch
// 06 35 91 8F 6F C9 09 08 CC AD 6A 08 [05] 00 00 00 // Second time to launch
// First line of Snook-Ball Game:
// 48 96 BB 49 51 8A 09 08 CC AD 63 08 [10] 00 00 00
// C8 00 DB F2 C9 3F 09 08 CC AD 69 08 [10] 00 00 00
// First line of Nine-Ball Game:
// 8C DE D9 8A 79 91 09 08 CC AD 63 08 [04] 00 00 00
// 72 53 56 5C 53 BB 09 08 CC AD 58 04 [04] 00 00 00

// Ball
struct ball{
    // Coordinate
    float coord_x;
    float coord_y;
    // Maybe these bytes represents rotation or other useful stuffs
    unsigned char unknown_0[12];
    // Speed
    float speed_x;
    float speed_y;
    // Whether this ball is kicked into the hole
    unsigned char on_desktop;
    unsigned char unknown_1[3];
}__attribute__((packed)); // __attribute__ ((packed)) to disable memory align

unsigned char type_choices[] = {
    0x00, 0x00, 0x00, 0x00,
    // Nine-Ball (0x04)
    0x0A, // 1 + 1 + 2 + 3 + 2 + 1 (Left -> Right)
    // Eight-Ball (0x05)
    0x10, // 1 + 1 + 2 + 3 + 4 + 5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // Snook (0x10)
    0x16, // 1 + 3 + 1 + 1 + 1 + 2 + 3 + 4 + 5 + 1
};

// TODO
int weight = 1000;

unsigned char * pocketclient_dll_address = (unsigned char*)(*((int *)(0x400000 + 0x50B68)));
unsigned char * balls_array_address = (unsigned char*)(*((int *)(pocketclient_dll_address + 0x9AD04)) + 8);
unsigned char game_type = *(balls_array_address - 4);

HHOOK mouseHook;

struct ball * obtain_ball_information(unsigned char index){
    return (struct ball *)(balls_array_address + sizeof(struct ball) * index);
}

double get_distance(double this_x, double this_y, double that_x, double that_y){
    return sqrt(pow(that_x - this_x, 2) + pow(that_y - this_y, 2));
}

int get_pixel(float n){
    return (int)(n * weight);
}

LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode >= 0) {
        switch(wParam) {
            case WM_RBUTTONDOWN:
                char message[0x100] = {0};

                // Get mouse position
                PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT) lParam;
                // Screen resolution
                float screen_cx = GetSystemMetrics(SM_CXSCREEN);
                float screen_cy = GetSystemMetrics(SM_CYSCREEN);
                // Centre of screen
                float screen_centre_x = screen_cx / 2.0f;
                float screen_centre_y = screen_cy / 2.0f;
                // Calculate distance away from centre of screen
                float mouse_x = p->pt.x - screen_centre_x;
                float mouse_y = p->pt.y - screen_centre_y;

                memset(message, 0, sizeof(message));
                sprintf(message, "Right Click: (%f, %f)", mouse_x, mouse_x);
                MessageBoxA(NULL, message, "Information", MB_OK);

                // Get max number of balls by game type
                unsigned char max_ball_number = type_choices[game_type];
                struct ball * balls_ptr[max_ball_number] = {0};

                // Get ball list
                int i;
                for(i = 0; i < max_ball_number; i++){
                    balls_ptr[i] = obtain_ball_information(i);
                }

                // Scan for nearest
                double min_distance = get_distance(
                    mouse_x, mouse_y,
                    get_pixel(balls_ptr[0]->coord_x), get_pixel(balls_ptr[0]->coord_y)
                );

                double distance_cache = 0.0;
                unsigned char min_distance_index = 0;
                for(i = 1; i < max_ball_number; i++){
                    distance_cache = get_distance(
                        mouse_x, mouse_y,
                        get_pixel(balls_ptr[i]->coord_x), get_pixel(balls_ptr[i]->coord_y)
                    );
                    if (min_distance > distance_cache){
                        min_distance = distance_cache;
                        min_distance_index = i;
                    }
                }


                sprintf(message, "The min index: %d; min distance: %f", min_distance_index, min_distance);
                MessageBoxA(NULL, message, "Information", MB_OK);

                // Caculate the new mouse position
                // TODO
                // float mouse_x_new = 0;
                // float mouse_y_new = 0;
                // struct ball * min_distance_ball = balls_ptr[min_distance_index];


                // Move the mouse
                // TODO: Send input
            break;
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

int cdasdasdsad(){

/*
    int timespan = 1000;

    float ball_coord_x;
    float ball_coord_y;
    float ball_speed_x;
    float ball_speed_y;
    char ball_on_desktop;

    float * float_ptr = (float *)(balls_array_address);

    int i = 0;

    static char buffer[0x400] = {0};

    char game_type = *(balls_array_address - 4);

    char ball_number = 0;


    // Log to file
    log_info(
         "%d: (%f, %f) (%f, %f) [%d]",
         i,
         ball_coord_x, ball_coord_y,
         ball_speed_x, ball_speed_y,
         ball_on_desktop
    );
    */


    /*
    // 2.2 Loop to obtain the balls' information
    for(
        // Init condition
        i = 0,
        float_ptr = (float *)(balls_array_address),
        ball_coord_x = 0.0,
        ball_coord_y = 0.0,
        ball_speed_x = 0.0,
        ball_speed_y = 0.0,
        ball_on_desktop = 1;

        // Judge condition
        i < ball_number;

        // After Every loop
        i++,
        float_ptr += 8
    ){
        // Get all information
        ball_coord_x = *((float *)(float_ptr));
        ball_coord_y = *((float *)(float_ptr + 1));
        ball_speed_x = *((float *)(float_ptr + 5));
        ball_speed_y = *((float *)(float_ptr + 6));
        ball_on_desktop = *((char *)(float_ptr + 7));

        // Log to file
        log_info(
             "%d: (%f, %f) (%f, %f) [%d]",
             i,
             ball_coord_x, ball_coord_y,
             ball_speed_x, ball_speed_y,
             ball_on_desktop
        );
    }
    */

    return 0;
}




extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /*
        FILE* log_file;
        // Set log file
        log_file = fopen("log.txt", "a+");
        // Declare buffer
        char buffer[0x100] = {0};
        // Set full buffer mode
        setvbuf(log_file, buffer, _IOFBF, sizeof(buffer));
        // Set log file handler
        log_set_fp(log_file);
        log_info("PocketClient.dll Address: 0x%08p", pocketclient_dll_address);
        log_info("Balls Array Address: 0x%08p", balls_array_address);
        fclose(log_file);
    */



    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Hook Mouse Event
            mouseHook = SetWindowsHookEx(
                WH_MOUSE_LL,
                mouseHookProc,
                hinstDLL,
                NULL
            );
            if(mouseHook == NULL){
                MessageBoxA(NULL, "Hook Mouse Events Failed!", "Error", MB_OK);
                return FALSE;
            }
            MessageBoxA(NULL, "Hook Mouse Events Successfully!", "Succeed", MB_OK);

            MSG Msg;
            while(GetMessage(&Msg, NULL, 0, 0) > 0)
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
            // UnHook
            // UnhookWindowsHookEx(mouseHook);
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }

    return TRUE; // succesful
}

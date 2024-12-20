#include "HCPPBox.h"
#include "hbox.h"
#include "hrc.h"
#include "stdint.h"
#include <thread>
#include <chrono>
#include <SDL.h>
#if WASM_BUILD
#include <assert.h>
#include <emscripten.h>
#endif


void loop()
{
    hgui_scene1_app_update(&g_hgui_scene1_app,NULL);
}


static void sub_init();

int main()
{


    sub_init();

    hgui_scene1_app_init(&g_hgui_scene1_app,NULL);

    {
#if WASM_BUILD
        emscripten_set_main_loop(loop,0,0);
#else
        while(true)
        {
            loop();
        }
#endif
    }

    return 0;
}


#define main submain

#include "../../3rdparty/HCppBox/test/monochromescreen/main.cpp"

static void sub_init()
{
    //修正背景色
    backcolor=0xFF00F2FA;
    //修正前景色
    frontcolor=0xFF000000;
}

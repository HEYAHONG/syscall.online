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

static void init();
static void loop();

int main()
{

    init();

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

#include "../../3rdparty/HCppBox/test/monochromescreen_simplegui/main.cpp"


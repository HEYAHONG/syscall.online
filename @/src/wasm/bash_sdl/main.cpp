#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_keyboard.h>
#include <thread>
#include <chrono>
#if WASM_BUILD
#include <assert.h>
#include <emscripten.h>
#endif

static bool is_running=true;
//存储当前的宽与高
static int screen_width=800;
static int screen_height=600;
const char * const title="base_sdl";
const char * const title_icon="sdl";
static SDL_Surface *screen=NULL;

//绘制背景
static void DrawBackGround(SDL_Surface *screen)
{
    //填充纯色背景
    SDL_Rect fullscreen= {0,0,(Uint16)screen_width,(Uint16)screen_height};
    SDL_FillRect(screen, &fullscreen, 0xFF664422);
}

//秒定时器回调
static bool second_timer_reached=false;
Uint32 SDLCALL second_timer_callback(Uint32 interval, void *param)
{
    second_timer_reached=true;
    SDL_Surface *screen=(SDL_Surface *)param;
    static Uint32 second=0;
    second++;
    printf("second timer callback!second=%d\n",(int)second);
    return interval;
}

void loop()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_VIDEORESIZE:
        {
            SDL_ResizeEvent *r = (SDL_ResizeEvent*)&event;
            printf("resize event %d:%d\n", r->w, r->h);
            screen_width=r->w;
            screen_height=r->h;
        }
        break;
        case SDL_QUIT:
        {
            is_running=false;
        }
        break;
        case SDL_KEYDOWN:
        {
            SDL_KeyboardEvent *key = (SDL_KeyboardEvent*)&event;
            if(key->keysym.sym == SDLK_F11 && (key->keysym.mod & KMOD_ALT)!=0)
            {
                //Alt+F11按键,默认全屏,桌面端有效
                SDL_WM_ToggleFullScreen(screen);
            }
        }
        break;
        default:
        {
        }
        break;
        }
    }
    //秒定时器到达
    if(second_timer_reached)
    {
        second_timer_reached=false;
#if WASM_BUILD
        SDL_AddTimer(1000,second_timer_callback,screen);
#endif // WASM_BUILD
        //开始渲染
        DrawBackGround(screen);
        //TODO:在此处渲染其它元素

        SDL_Flip(screen);
    }
}

static int main_thread_entry()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        printf("sdl init  error!\n");
        return -1;
    }
    screen = SDL_SetVideoMode(screen_width, screen_height, 32, SDL_HWSURFACE);
    if(screen==NULL)
    {
        return -1;
    }

    //设置标题(非WASM情况下有效)
    SDL_WM_SetCaption(title,title_icon);

    //设置秒定时器
    SDL_AddTimer(1000,second_timer_callback,screen);


#if WASM_BUILD
    emscripten_set_canvas_size(screen_width, screen_height);
    emscripten_set_main_loop(loop,0,0);
#else
    while(is_running)
    {
        loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SDL_Quit();
#endif
    return 0;
}

int main()
{
    return main_thread_entry();
}

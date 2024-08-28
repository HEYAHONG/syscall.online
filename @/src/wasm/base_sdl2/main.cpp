#include <stdlib.h>
#include <stdio.h>
#include <random>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include "SDL.h"
#include "SDL_ttf.h"
#include "fsloader.h"
#include "font.h"

int done=0;
static int screen_width=800;
static int screen_height=600;
static SDL_Window *window=NULL;
static SDL_Renderer *renderer=NULL;
static void DrawBackGround()
{
    SDL_SetRenderDrawColor(renderer,0x66,0x44,0x22,0xFF);
    SDL_Rect screen= {0,0,screen_width,screen_height};
    SDL_RenderFillRect(renderer,&screen);
}

//绘制随机方块(50X50)
static void DrawRandomRect()
{
    static std::mt19937 generator;
    Uint16 x=screen_width/50;
    Uint16 y=screen_width/50;
    for(Uint16 i=0; i<x; i++)
    {
        for(Uint16 j=0; j<y; j++)
        {
            SDL_Rect fill= {(int16_t)(50*i),(int16_t)(50*j),50,50};
            uint32_t color=generator();
            SDL_SetRenderDrawColor(renderer,color>>16,color>>8,color,0xFF);
            SDL_RenderFillRect(renderer,&fill);
        }
    }
}




static void quit(int rc)
{
    TTF_Quit();
    SDL_Quit();
    exit(rc);
}

void loop(void)
{
    int i;
    SDL_Event event;
    /* Check for events */
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_WINDOWEVENT:
        {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                if (window)
                {
                    SDL_Log("Window %" SDL_PRIu32 " resized to %" SDL_PRIs32 "x%" SDL_PRIs32 "\n",
                            event.window.windowID,
                            event.window.data1,
                            event.window.data2);
                    screen_width=event.window.data1;
                    screen_height=event.window.data1;
                }
            }
            if (event.window.event == SDL_WINDOWEVENT_MOVED)
            {
                SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
                if (window)
                {
                    SDL_Log("Window %" SDL_PRIu32 " moved to %" SDL_PRIs32 ",%" SDL_PRIs32 " (display %s)\n",
                            event.window.windowID,
                            event.window.data1,
                            event.window.data2,
                            SDL_GetDisplayName(SDL_GetWindowDisplayIndex(window)));
                }
            }
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
            {

            }
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
            {

            }
        }
        break;
        case SDL_KEYDOWN:
        {
            SDL_KeyboardEvent &key=*(SDL_KeyboardEvent *)&event;
            if(key.keysym.sym == SDLK_F11 && (key.keysym.mod & KMOD_ALT)!=0)
            {
                //Alt+F11按键,默认全屏,桌面端有效
                static bool isfullscreen=false;
                SDL_SetWindowFullscreen(window,isfullscreen?0:SDL_WINDOW_FULLSCREEN_DESKTOP);
                isfullscreen=!isfullscreen;
            }

        }
        break;
        case SDL_MOUSEBUTTONUP:
        {

        }
        break;
        case SDL_QUIT :
        {
            done=1;
        }
        break;
        default:
        {
        }
        break;
        }
    }

    {
        //采用tick实现按秒动作
        static Uint32 last_tick=0;
        Uint32 tick= SDL_GetTicks();
        if(tick-last_tick > 1000)
        {
            if(last_tick==0)
            {
                last_tick=tick;
                //第一次进入
                DrawBackGround();
                {
                    TTF_Font *font=TTF_OpenFont((std::string(font_get_root())+"/"+font_get_default_font()).c_str(),32);
                    if(font!=NULL)
                    {
                        const char *text="BaseSDL2启动中！";
                        SDL_Color font_color= {0xff,0xff,0xff};
                        SDL_Surface *surface=TTF_RenderUTF8_Blended(font,text,font_color);
                        if(surface!=NULL)
                        {
                            SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer,surface);
                            if(texture!=NULL)
                            {
                                int w=0,h=0;
                                TTF_SizeUTF8(font,text,&w,&h);
                                SDL_Rect dest_rect= {0,0,w,h};
                                SDL_RenderCopy(renderer,texture,NULL,&dest_rect);
                                SDL_DestroyTexture(texture);
                            }
                            SDL_FreeSurface(surface);
                        }
                        TTF_CloseFont(font);
                    }
                }
                SDL_RenderPresent(renderer);
            }
            else
            {
                last_tick=tick;
                {
                    static Uint32 second=0;
                    second++;
                    printf("second reached!second=%d\n",(int)second);
                }
                //开始渲染
                SDL_RenderClear(renderer);
                DrawBackGround();
                //TODO:在此处渲染其他元素
                DrawRandomRect();

                SDL_RenderPresent(renderer);
            }
        }
    }


#ifdef __EMSCRIPTEN__
    if (done)
    {
        emscripten_cancel_main_loop();
    }
#endif
}

int main(int argc, char *argv[])
{
    //加载文件系统(主要进行一些用户操作)
    fsloader_init();
    {
        printf("font root:%s,default font:%s\n",font_get_root(),font_get_default_font().c_str());
    }
    {
        SDL_version version= {0};
        SDL_GetVersion(&version);
        printf("SDL Version:%d,%d,%d\n",version.major,version.minor,version.patch);
    }

    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    /* Initialize SDL */

    uint32_t sdl_initflag=SDL_INIT_EVERYTHING;
#ifdef __EMSCRIPTEN__
    {
        //去除不支持的子系统
        sdl_initflag&=~(SDL_INIT_HAPTIC);
    }
#endif
    if (SDL_Init(sdl_initflag) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    TTF_Init();

    /* Set 800x600 video mode */
    window = SDL_CreateWindow("base_Sdl2",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,screen_width, screen_height, 0);
    if (!window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create 800x600 window: %s\n", SDL_GetError());
        quit(2);
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s\n",SDL_GetError());
        quit(2);
    }


    /* Main render loop */
    done = 0;
#ifdef __EMSCRIPTEN__
    emscripten_set_canvas_size(screen_width, screen_height);
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!done)
    {
        loop();
    }
#endif

    quit(0);
    /* keep the compiler happy ... */
    return 0;
}


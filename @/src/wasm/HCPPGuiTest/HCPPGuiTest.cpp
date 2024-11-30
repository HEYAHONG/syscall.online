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
static const size_t w=320;
static const size_t h=240;
static uint32_t VRAM[w][h]= {0};
static hgui_pixel_t pixel;

bool _loop();
void loop()
{
    _loop();
}

bool _loop()
{
    static size_t i=0;
    if(hgui_driver_update(NULL))
    {
        SDL_Delay(1);
        i++;
        if(i==100)
        {
            ssize_t w=::w,h=::h;
            {
                hgui_driver_resize(NULL,&w,&h);
            }
            hgui_driver_fill_rectangle(NULL,0,0,w,h,pixel);
        }
        if(i==200)
        {
            auto draw_pixel=[](const hgui_gui_dotfont_t * dotfont,size_t x,size_t y,bool point,void *usr)->bool
            {
                (void)dotfont;
                (void)usr;
                if((x<w) && (y<h)&&point)
                {
                    VRAM[x][y]=0xFF000000;
                }
                return true;
            };
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_acs2_0806,"Booting",0,0,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_acs2_1206,"Booting",0,8,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_acs2_1608,"Booting",0,8+12,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_acs2_2416,"Booting",0,8+12+16,w,draw_pixel,NULL);
            hgui_driver_fill_rectangle(NULL,0,0,w,h,pixel);
        }
        if (i == 300)
        {
            auto draw_pixel = [](const hgui_gui_dotfont_t* dotfont, size_t x, size_t y, bool point, void* usr)->bool
            {
                (void)dotfont;
                (void)usr;
                if ((x < w) && (y < h) && point)
                {
                    VRAM[x][y] = 0xFF000000;
                }
                return true;
            };
            const char* banner = (const char *)RCGetHandle("banner");
            if (banner != NULL)
            {
                for (size_t i = 0; i < w; i++)
                {
                    for (size_t j = 0; j < h; j++)
                    {
                        VRAM[i][j] = 0xFFFF0000;
                    }
                }
                hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_acs2_1206, banner, 0, 0, w, draw_pixel, NULL);
                hgui_driver_fill_rectangle(NULL, 0, 0, w, h, pixel);
            }
        }

        if(i==500)
        {
            hgui_gui_xpm_header_t header=hgui_gui_xpm_header_get(hgui_gui_xpm_xpm_xpm);
            printf("xpm:width=%d,height=%d,ncolors=%d,cpp=%d,x_hotspot=%d,y_hotspot=%d,XPMEXT=%s\r\n",(int)header.width,(int)header.height,(int)header.ncolors,(int)header.cpp,(int)header.x_hotspot,(int)header.y_hotspot,header.XPMEXT?"true":"false");
            for (size_t i = 0; i < w; i++)
            {
                for (size_t j = 0; j < h; j++)
                {
                    VRAM[i][j] = 0xFF00FF00;
                }
            }
            auto draw_pixel = [](size_t x,size_t y,uint32_t color,void *usr)->bool
            {
                (void)usr;
                if ((x < w) && (y < h))
                {
                    VRAM[x][y] = (0xFF000000 | color);
                }
                return true;
            };
            hgui_gui_xpm_draw_color(hgui_gui_xpm_xpm_xpm,(w-header.width)/2,(h-header.height)/2,draw_pixel,NULL);
            hgui_driver_fill_rectangle(NULL, 0, 0, w, h, pixel);
        }
        return true;
    }

    return false;
}

int main()
{
    HCPPGuiInit();

    {
        //初始化屏幕颜色
        for(size_t i=0; i<w; i++)
        {
            for(size_t j=0; j<h; j++)
            {
                VRAM[i][j]=0xFF0000FF;
            }
        }
        //初始化像素回调
        pixel.mode=HGUI_PIXEL_MODE_CALLBACK;
        pixel.pixel=[](ssize_t x,ssize_t y) -> hgui_pixel_t
        {
            hgui_pixel_t ret={0};
            ret.mode=HGUI_PIXEL_MODE_32_BITS;
            if(x<w && y <h)
            {
                ret.pixel_32_bits=VRAM[x][y];
            }
            return ret;
        };
    }

    if(hgui_driver_reset(NULL))
    {
#if WASM_BUILD
        emscripten_set_main_loop(loop,0,0);
#else
        while(_loop());
#endif
    }

    return 0;
}


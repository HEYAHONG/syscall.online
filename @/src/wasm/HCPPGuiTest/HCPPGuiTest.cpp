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
extern "C"
{
    extern const hgui_gui_rawimage_t hrawimage_input;
    extern const hgui_gui_rawimage_t hrawimage_input_gray;
}
static const size_t w=320;
static const size_t h=240;
static uint32_t VRAM[w][h]= {0};
static hgui_pixel_t pixel;

static bool hgui_event_process(uint8_t type,void *eventparam,size_t eventparam_length,void *usr)
{
    {
        hgui_gui_event_key_t key;
        if(hgui_gui_event_key_get(&key,type,eventparam,eventparam_length,usr)!=NULL)
        {
            printf("event key:%c(%02X) %s,mode=%04X\r\n",(char)key.key_value,(int)key.key_value,key.key_press_or_release?"press":"release",(int)key.key_mode);
        }
    }
    return true;
}

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
            for (size_t i = 0; i < w; i++)
            {
                for (size_t j = 0; j < h; j++)
                {
                    VRAM[i][j] = 0xFF0000FF;
                }
            }
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
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_ascii_0806,"Booting",0,0,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_ascii_1206,"Booting",0,8,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_ascii_1608,"Booting",0,8+12,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_ascii_2416,"Booting",0,8+12+16,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_unicode_string(&hgui_gui_dotfont_unicode_dummy_1212,L"启动中",0,8+12+16+24,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_unicode_string(&hgui_gui_dotfont_unicode_dummy_1616,L"启动中",0,8+12+16+24+12,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_unicode_string(&hgui_gui_dotfont_unicode_dummy_2424,L"启动中",0,8+12+16+24+12+16,w,draw_pixel,NULL);
            hgui_gui_dotfont_show_unicode_string(&hgui_gui_dotfont_unicode_dummy_3232,L"启动中",0,8+12+16+24+12+16+24,w,draw_pixel,NULL);
            {
                const hgui_gui_dotfont_hdotfont_t font=hgui_gui_dotfont_hdotfont(hdotfont_char_set_24,hdotfont_char_set_24_size,24);
                hgui_gui_dotfont_show_unicode_string((const hgui_gui_dotfont_t *)&font,L"启动中",0,8+12+16+24+12+16+24+32,w,draw_pixel,NULL);
            }
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
                hgui_gui_dotfont_show_ascii_string(&hgui_gui_dotfont_ascii_1206, banner, 0, 0, w, draw_pixel, NULL);
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
        if(i==1000)
        {

            printf("rawimage:width=%d,height=%d,cpp=%d\r\n",(int)hrawimage_input.width,(int)hrawimage_input.height,(int)hrawimage_input.cpp);
            for (size_t i = 0; i < w; i++)
            {
                for (size_t j = 0; j < h; j++)
                {
                    VRAM[i][j] = 0xFF000000;
                }
            }
            auto draw_pixel=[](const hgui_gui_rawimage_t *rawimage,size_t x,size_t y,const uint8_t *color,void *usr)
            {
                if(rawimage==NULL)
                {
                    return false;
                }
                if(rawimage->cpp==3)
                {
                    VRAM[x][y]=(0xFF000000 | (color[0] * 0x10000 + color[1] * 0x100 +color[2] * 0x01) );
                }
                else
                {
                    VRAM[x][y]=(0xFF000000 | (color[0] * 0x10000 + color[0] * 0x100 +color[0] * 0x1) );
                }

                return true;
            };

            hgui_gui_rawimage_draw_color(&hrawimage_input,(w-hrawimage_input.width)/2,(h-hrawimage_input.height)/2,draw_pixel,NULL);
            hgui_driver_fill_rectangle(NULL, 0, 0, w, h, pixel);
        }
        if(i==1500)
        {
            printf("rawimage_gray:width=%d,height=%d,cpp=%d\r\n",(int)hrawimage_input_gray.width,(int)hrawimage_input_gray.height,(int)hrawimage_input_gray.cpp);
            for (size_t i = 0; i < w; i++)
            {
                for (size_t j = 0; j < h; j++)
                {
                    VRAM[i][j] = 0xFF000000;
                }
            }
            auto draw_pixel=[](const hgui_gui_rawimage_t *rawimage,size_t x,size_t y,const uint8_t *color,void *usr)
            {
                if(rawimage==NULL)
                {
                    return false;
                }
                if(rawimage->cpp==3)
                {
                    VRAM[x][y]=(0xFF000000 | (color[0] * 0x10000 + color[1] * 0x100 +color[2] * 0x1) );
                }
                else
                {
                    VRAM[x][y]=(0xFF000000 | (color[0] * 0x10000 + color[0] * 0x100 +color[0] * 0x1) );
                }

                return true;
            };

            hgui_gui_rawimage_draw_color(&hrawimage_input_gray,(w-hrawimage_input_gray.width)/2,(h-hrawimage_input_gray.height)/2,draw_pixel,NULL);
            hgui_driver_fill_rectangle(NULL, 0, 0, w, h, pixel);
        }
        if(i== 2000)
        {
            i=150;
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
                //修复颜色显示，使其与桌面端表现一致
                uint32_t pixel_bits=VRAM[x][y];
                uint32_t new_pixel_bits=(pixel_bits&0xFF000000);
#if WASM_BUILD
                new_pixel_bits|=((pixel_bits>>16)&0xFF);
                new_pixel_bits|=(pixel_bits&0xFF00);
                new_pixel_bits|=((pixel_bits&0xFF)<<16);
#else
                new_pixel_bits=pixel_bits;
#endif
                ret.pixel_32_bits=new_pixel_bits;
            }
            return ret;
        };

        //初始化事件处理
        hgui_driver_event_callback_set(NULL,hgui_event_process);

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


#include "../../../3rdparty/HCppBox/test/HCPPGuiTest/hdotfont.c"
#include "../../../3rdparty/HCppBox/test/HCPPGuiTest/input_hrawimage.c"

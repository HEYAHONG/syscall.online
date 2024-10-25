#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include "HCPPBox.h"
#include "hbox.h"
#include <thread>
#include <chrono>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__
static void banner();
static void execute_line(char *line);
static WINDOW *win=NULL;
static class memory_clean
{
public:
    memory_clean()
    {

    }
    ~memory_clean()
    {
        if(win!=NULL)
        {
            delwin(win);
        }
        endwin();
    }
} g_memory_clean;
static int win_putchar(char ch)
{
    putchar(ch);
    if(win!=NULL&& ch != '\r')
    {
        char str[2]= {ch,0};
        waddstr(win,str);
        wrefresh(win);
    }
    return ch;
}
static bool IoIsConnected();
static char buff[4096]= {0};
static size_t buff_index=0;
static int last_x=0;
static int last_y=0;
static void main_loop()
{
    {
        last_x=getcurx(win);
        last_y=getcury(win);
        chtype  c=wgetch(win);
        if(c=='\b')
        {
            if(buff_index>0)
            {
                buff_index--;
                wdelch(win);
            }
            else
            {
                wmove(win,last_y,last_x);
                wrefresh(win);
            }
            return;
        }
        if(((c & 0xFF)>=0x20) && ((c & 0xFF)<0x80))
        {
            if(buff_index<(sizeof(buff)-1))
            {
                buff[buff_index++]=(c&0xff);
            }
        }
        if(c==KEY_ENTER || c == '\n' || (buff_index==((sizeof(buff)-1))))
        {
            buff[buff_index]='\0';
            if(strcmp(buff,"exit")==0)
            {
                hprintf("exist is not support!\r\n");
            }
            execute_line(buff);
            waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
            buff[0]='\0';
            buff_index=0;
        }
        wrefresh(win);
    }
}
int pthread_main()
{
#ifdef XCURSES
    Xinitscr(argc, argv);
#else
    initscr();
#endif
#ifdef A_COLOR
    if (has_colors())
        start_color();
#endif

    win = newwin(0, 0, 0, 0);

    if (win == NULL)
    {
        endwin();
        return -1;
    }


#ifdef A_COLOR
    if (has_colors())
    {
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        wbkgd(win, COLOR_PAIR(1));
    }
    else
#endif
        wbkgd(win, A_REVERSE);

    echo();
    curs_set(1);
    scrollok(win,true);
    {
        //初始化套接字
        HCPPSocketInit();

        hprintf_set_callback([](char c)
        {
            win_putchar(c);
        });
        {
            //打印信息
            banner();
        }

    }

    waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
    {
        while(true)
        {
            main_loop();
        }
    }
}

int main()
{
    std::thread main_thread(pthread_main);
    main_thread.detach();
#ifndef __EMSCRIPTEN__
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#endif // __EMSCRIPTEN__
    return 0;
}

#define main submain
#define readline subreadline
//导入测试代码
#include "../../../3rdparty/HCppBox/test/ModbusTCPClient/main.cpp"

static bool IoIsConnected()
{
    return Io.IsConnected();
}



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
#include <emscripten/websocket.h>
#include <emscripten/threading.h>
#include <emscripten/posix_socket.h>
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
static void main_loop()
{
    {
        int ret=wgetnstr(win,buff,sizeof(buff)-1);
        if(ret!=ERR && ret >= 0)
        {
            buff[ret]='\0';
            if(strcmp(buff,"exit")==0)
            {
                hprintf("exist is not support!\r\n");
            }
            execute_line(buff);
            waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
        }
        wrefresh(win);
    }
}
static void socket_init()
{
#ifdef __EMSCRIPTEN__
    static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
    bridgeSocket = emscripten_init_websocket_to_posix_socket_bridge("ws://localhost:58080");
    // Synchronously wait until connection has been established.
    uint16_t readyState = 0;
    do
    {
        emscripten_websocket_get_ready_state(bridgeSocket, &readyState);
        emscripten_thread_sleep(1000);
        printf("wait for socket ready!\r\n");
    }
    while (readyState == 0);
#else
    HCPPSocketInit();
#endif // __EMSCRIPTEN__

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
        socket_init();

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

#include "ModbusSocketIo.h"

#define main submain
#define readline subreadline
//导入测试代码
#include "../../../3rdparty/HCppBox/test/ModbusTCPClient/main.cpp"

static bool IoIsConnected()
{
    return Io.IsConnected();
}



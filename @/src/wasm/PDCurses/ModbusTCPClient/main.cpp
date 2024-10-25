#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include "HCPPBox.h"
#include "hbox.h"
static void banner();
static void execute_line(char *line);
static WINDOW *win=NULL;
static int win_putchar(char ch)
{
    putchar(ch);
    if(win!=NULL&& ch != '\r')
    {
        char str[2]={ch,0};
        waddstr(win,str);
        wrefresh(win);
    }
    return ch;
}
static bool IoIsConnected();
int main()
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
    char buff[4096]={0};
    size_t buff_index=0;
    waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
    while(true)
    {
        chtype  c=wgetch(win);
        if(((c & 0xFF)>=0x20) && ((c & 0xFF)<0x80))
        {
            if(buff_index<(sizeof(buff)-1))
            {
                buff[buff_index++]=(c&0xff);
            }
            else
            {
                buff[sizeof(buff)-1]='\0';
                execute_line(buff);
                waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
                buff[0]='\0';
                buff_index=0;
            }
        }
        if(c==KEY_ENTER || c == '\n')
        {
            buff[buff_index]='\0';
            execute_line(buff);
            waddstr(win,IoIsConnected()?"modbus>":"modbus(not connected)>");
            buff[0]='\0';
            buff_index=0;
        }
        wrefresh(win);
    }

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



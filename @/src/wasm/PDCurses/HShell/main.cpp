#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <curspriv.h>
#include "HCPPBox.h"
#include "hbox.h"
#include "time.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__
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

static void main_init();
static bool  main_loop();
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
    {
        wbkgd(win, A_REVERSE);
    }
    noecho();
    curs_set(1);
    scrollok(win,true);
    keypad(win,true);
    nodelay(win,false);
    main_init();
    while(main_loop())
    {

    }
    return 0;
}


#define main submain

//导入测试代码
#include "../../../3rdparty/HCppBox/test/HShell/main.cpp"

static const  struct
{
    int key;
    const char *esc_seq;
} key_map_array[]=
{
    { KEY_UP,"[A" },
    { KEY_DOWN,"[B"},
    { KEY_RIGHT,"[C"},
    { KEY_LEFT,"[D"},
    { KEY_IC,"[2~"},
    { KEY_DC,"[3~"},
};

static const char *current_esc_seq=NULL;

static int key_map(int old_ch)
{
    //对一些字符进行转换(主要转换为终端格式)
    int ret=old_ch;
    switch(old_ch)
    {
    case '\b':
    {
        ret=HSHELL_CTLSEQ_CONTROL_SET_DEL;
    }
    break;
    default:
    {
        for(size_t i=0; i<(sizeof(key_map_array)/sizeof(key_map_array[0])); i++)
        {
            if(key_map_array[i].key==old_ch)
            {
                current_esc_seq=key_map_array[i].esc_seq;
                ret=HSHELL_CTLSEQ_CONTROL_SET_C0_ESC;
                break;
            }
        }
    }
    break;
    }
    if(ret >= 0x80)
    {
        //特殊按键默认不处理
        ret=0;
    }
    return ret;
}

static int main_getchar(void)
{
    if(current_esc_seq!=NULL)
    {
        if(current_esc_seq[0]=='\0')
        {
            current_esc_seq=NULL;
        }
        else
        {
            return *(current_esc_seq++);
        }
    }
    if(win!=NULL)
    {
        int ret=0;
        while(ret==0)
        {
            ret=(((ret=key_map(wgetch(win)))!=ERR)?(ret&0xFFFF):EOF);
        }
        return ret;
    }
    return 0;
}

static int main_putchar(int ch)
{
    if(ch!='\r')
    {
        putchar(ch);
    }
    if(win!=NULL && ch!='\r')
    {
        if(ch=='\b')
        {
            //当位置为行首时，回到上一行末尾
            int x=getcurx(win),y=getcury(win);
            if(x==0 && y>1)
            {
                y--;
                x=(getmaxx(win));
                if(x>0 && y > 0)
                {
                    win->_curx = x;
                    win->_cury = y;
                }
                wrefresh(win);
                wsyncup(win);
            }
        }
        waddch(win,(ch&0xFFFF));
        wrefresh(win);
    }
    return ch;
}

static void main_init()
{
    hshell_command_array_set(NULL,commands,sizeof(commands)/sizeof(commands[0]));
    {
        hshell_context_external_api_t api=hshell_context_default_external_api();
        api.getchar=main_getchar;
        api.putchar=main_putchar;
        hshell_external_api_set(NULL,api);
    }
}

static bool  main_loop()
{
    //此hshell不用退出
    hshell_loop(NULL);
    return true;
}



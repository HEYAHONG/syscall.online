#include <thread>
#include <chrono>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "dbus_interface.h"
#include "DeamonLog.h"

static bool isrunning=true;
extern "C" void main_stop_running()
{
    isrunning=false;
}
static void signalHandler(int signum)
{
    if(!isrunning)
    {
        return;
    }
    isrunning=false;
}

static void init_signal()
{
    signal(SIGINT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
}


int main()
{
    //初始化信号
    init_signal();

    DeamonLog_Init();

    dbus_interface_init();

    while(isrunning)
    {
        //默认systemd作为init系统，因此不采用传统守护进程的启动方式(fork()->setsid()->fork()),直接不退出程序
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        dbus_interface_process();
    }

    dbus_interface_deinit();

    DeamonLog_Deinit();

    return 0;
}

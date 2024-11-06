#include <thread>
#include <chrono>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "dbus_interface.h"
#include "sysloginfo.h"
#include "globalvariable.h"
#include "websockets.h"

static void signalHandler(int signum)
{
    gv_set_running(false);
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

    sysloginfo_init();

    gv_init();

    dbus_interface_init();

    websockets_init();

    while(gv_is_running())
    {
        //默认systemd作为init系统，因此不采用传统守护进程的启动方式(fork()->setsid()->fork()),直接不退出程序
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        dbus_interface_process();
    }

    websockets_deinit();

    dbus_interface_deinit();

    gv_deinit();

    sysloginfo_deinit();

    return 0;
}

#include "DeamonLog.h"
#include "syslog.h"

void DeamonLog_Init()
{
    //初始化 syslog
    openlog("syscalld", LOG_NDELAY|LOG_NOWAIT|LOG_PID, LOG_DAEMON);
    LOGI("DeamonLog Init!");
}

void DeamonLog_Deinit()
{
    LOGI("DeamonLog Deinit!");
    //关闭syslog
    closelog();
}

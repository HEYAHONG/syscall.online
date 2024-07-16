#include "sysloginfo.h"
#include "syslog.h"

void sysloginfo_init()
{
    //初始化 syslog
    openlog("syscalld", LOG_NDELAY|LOG_NOWAIT|LOG_PID, LOG_DAEMON);
    LOGI("sysloginfo Init!");
}

void sysloginfo_deinit()
{
    LOGI("sysloginfo Deinit!");
    //关闭syslog
    closelog();
}

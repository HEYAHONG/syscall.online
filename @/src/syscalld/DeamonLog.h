#ifndef DEAMONLOG_H_INCLUDED
#define DEAMONLOG_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifndef LOGI
#include "syslog.h"
#define LOGINFO_REAL(...)  {syslog(LOG_DAEMON|LOG_INFO,__VA_ARGS__);}
#define LOGI(fmt,...) LOGINFO_REAL(fmt,##__VA_ARGS__)
#endif // LOGI


/** \brief DeamonLog初始化
 *
 *
 */
void DeamonLog_Init();

/** \brief DeamonLog反初始化
 *
 *
 */
void DeamonLog_Deinit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEAMONLOG_H_INCLUDED

#ifndef __GLOBALVARIABLE_H__
#define __GLOBALVARIABLE_H__
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <stdint.h>

/** \brief 初始化
 *
 *
 */
void gv_init();


/** \brief 反初始化
 *
 *
 */
void gv_deinit();

using boot_timepoint_t=std::chrono::system_clock::time_point;
/** \brief 获取守护进程启动时间戳
 *
 * \return boot_timepoint_t 守护进程启动时间戳
 *
 */
boot_timepoint_t gv_get_boot_time_point();

/** \brief 获取守护进程启动时间戳
 *
 * \return uint64_t 守护进程启动时间戳（秒数）
 *
 */
uint64_t gv_get_boot_time_point_seconds();

using uuid_t=boost::uuids::uuid;
/** \brief 获取全局uuid
 *
 * \return uuid_t uuid，用于区分不同的实例,在整个进程的生命周期中保持不变。
 *
 */
uuid_t gv_get_global_uuid();

/** \brief 获取新uuid
 *
 * \return uuid_t uuid
 *
 */
uuid_t gv_get_new_uuid();

/** \brief UUID转字符串
 *
 * \param uuid uuit_t UUID
 * \return std::string UUID字符串
 *
 */
std::string gv_uuid_to_string(uuid_t uuid=gv_get_global_uuid());

/** \brief 获取是否运行
 *
 * \return bool 是否运行
 *
 */
bool gv_is_running();

/** \brief 设置是否运行
 *
 * \param _is_running bool 是否运行
 *
 */
void gv_set_running(bool _is_running=false);


/** \brief
 *
 * \param func 初始函数
 *
 */
#define INIT_EXPORT(func)       extern "C" void __gv_add_init_func__(int(*_init)());\
                                static void init_export()  __attribute__((constructor));\
                                static void init_export()\
                                {\
                                    __gv_add_init_func__(func);\
                                }

#endif // __GLOBALVARIABLE_H__

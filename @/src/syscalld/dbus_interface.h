#ifndef __DBUS_INTERFACE_H__
#define __DBUS_INTERFACE_H__
#include "dbus/dbus.h"
#include <functional>

#ifdef __cplusplus
extern "C"
{
#endif

void dbus_interface_init();

void dbus_interface_process();

void dbus_interface_deinit();



#ifdef __cplusplus
}
#endif


/** \brief DBus消息回调
 *
 * \param conn DBusConnection* DBus连接指针
 * \param msg DBusMessage* DBus消息指针
 * \return bool 是否成功处理，false继续交给下一个回调处理
 *
 */
typedef bool _dbus_interface_message_callback_t(DBusConnection *conn,DBusMessage *msg);
using dbus_interface_message_callback_t=std::function<_dbus_interface_message_callback_t>;


/** \brief 注册DBus消息回调
 *
 * \param cb dbus_interface_message_callback_t DBus消息回调
 *
 */
void dbus_interface_register_message_callback(dbus_interface_message_callback_t cb);


#endif // __DBUS_INTERFACE_H__

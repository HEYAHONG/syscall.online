#ifndef __DBUS_INTERFACE_H__
#define __DBUS_INTERFACE_H__

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

#endif // __DBUS_INTERFACE_H__

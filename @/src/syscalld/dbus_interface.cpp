#include "dbus_interface.h"
#include "dbus/dbus.h"
#include "DeamonLog.h"

#define DBUS_SERVICE_NAME "online.syscall"
extern "C" void main_stop_running();
static DBusConnection *connection = NULL;
void dbus_interface_init()
{
    DBusError error;
    dbus_error_init(&error);
    connection=dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if(dbus_error_is_set(&error))
    {
        LOGI("Connect Dbus failed: %s\n", error.message);
        dbus_error_free(&error);
        main_stop_running();
        return;
    }
    if(dbus_bus_name_has_owner(connection,DBUS_SERVICE_NAME,&error))
    {
        LOGI("Dbus exists:");
        dbus_error_free(&error);
        main_stop_running();
        return;
    }
    int ret=dbus_bus_request_name(connection, DBUS_SERVICE_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if(dbus_error_is_set(&error))
    {
        LOGI("Register Dbus Service failed: %s\n", error.message);
        dbus_error_free(&error);
        main_stop_running();
        return;
    }

    dbus_error_free(&error);
}

void dbus_interface_process()
{
    if(connection==NULL)
    {
        LOGI("Dbus not exists:");
        main_stop_running();
        return;
    }
    DBusError error;
    dbus_error_init(&error);

    dbus_connection_read_write_dispatch(connection,10);

    DBusMessage *message = dbus_connection_pop_message(connection);

    if(message!=NULL)
    {
        dbus_message_unref(message);
    }

    dbus_error_free(&error);
}

void dbus_interface_deinit()
{
    if(connection!=NULL)
    {
        dbus_connection_unref(connection);
        connection=NULL;
    }
}

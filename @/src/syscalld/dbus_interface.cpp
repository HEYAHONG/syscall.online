#include "dbus_interface.h"
#include "dbus/dbus.h"
#include "sysloginfo.h"
#include "globalvariable.h"
#include <vector>
#include <mutex>

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
        gv_set_running(false);
        return;
    }
    if(dbus_bus_name_has_owner(connection,DBUS_SERVICE_NAME,&error))
    {
        LOGI("Dbus exists:");
        dbus_error_free(&error);
        gv_set_running(false);
        return;
    }
    int ret=dbus_bus_request_name(connection, DBUS_SERVICE_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if(dbus_error_is_set(&error))
    {
        LOGI("Register Dbus Service failed: %s\n", error.message);
        dbus_error_free(&error);
        gv_set_running(false);
        return;
    }

    dbus_error_free(&error);
}

static std::recursive_mutex m_msg_callback_lock;
static std::vector<dbus_interface_message_callback_t> m_msg_callback;
void dbus_interface_process()
{
    if(connection==NULL)
    {
        LOGI("Dbus not exists:");
        gv_set_running(false);
        return;
    }
    DBusError error;
    dbus_error_init(&error);

    dbus_connection_read_write_dispatch(connection,1);
    while(true)
    {
        DBusMessage *message = dbus_connection_pop_message(connection);
        if(message!=NULL)
        {
            std::lock_guard<std::recursive_mutex> lock(m_msg_callback_lock);
            for(auto cb:m_msg_callback)
            {
                if(cb!=NULL)
                {
                    if(cb(connection,message))
                    {
                        break;
                    }
                }
            }
            dbus_message_unref(message);
        }
        else
        {
            break;
        }
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

void dbus_interface_register_message_callback(dbus_interface_message_callback_t cb)
{
    std::lock_guard<std::recursive_mutex> lock(m_msg_callback_lock);
    m_msg_callback.push_back(cb);
}

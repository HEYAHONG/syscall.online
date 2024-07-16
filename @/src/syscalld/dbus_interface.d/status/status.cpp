#include "dbus_interface.h"
#include "globalvariable.h"
#include "sysloginfo.h"
#include "time.h"
#include <json/value.h>
#include <json/writer.h>
#define DBUS_INTERFACE_NAME "online.syscall"
#define DBUS_METHOD_NAME         "status"
//为简化实现，此方法返回字符串（内容为JSON）
static bool dbus_interface_message_callback(DBusConnection *conn,DBusMessage *msg)
{
    if(conn==NULL || msg == NULL)
    {
        return false;
    }
    if(dbus_message_is_method_call(msg,DBUS_INTERFACE_NAME,DBUS_METHOD_NAME))
    {
        DBusMessage *reply=dbus_message_new_method_return(msg);
        if(reply!=NULL)
        {
            std::string ret;
            {
                Json::Value root(Json::objectValue);
                Json::StyledWriter writer;
                root["boottime"]=(Json::UInt64)gv_get_boot_time_point_seconds();
                root["path"]=dbus_message_get_path(msg);
                root["timestamp"]=(Json::UInt64)time(NULL);
                ret=writer.write(root);
            }
            const char *retstr=ret.c_str();
            if(!ret.empty())
            {
                dbus_message_append_args (reply,DBUS_TYPE_STRING, &retstr,DBUS_TYPE_INVALID);
            }
            dbus_connection_send(conn, reply, NULL);
            dbus_connection_flush(conn);
            dbus_message_unref(reply);
        }
        return true;
    }
    return false;
}

static int init()
{
    dbus_interface_register_message_callback(dbus_interface_message_callback);
    return 0;
}

INIT_EXPORT(init);

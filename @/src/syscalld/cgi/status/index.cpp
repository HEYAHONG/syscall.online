#include <cstdlib>
#include <new>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPRedirectHeader.h>
#include <cgicc/HTMLClasses.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <dbus/dbus.h>

//重定向到static下的说明目录
std::string get_redirect_url(std::string script_name)
{
    std::string script_dir;
    for(size_t i=0; i<script_name.length(); i++)
    {
        if(script_name.c_str()[script_name.length()-i]=='/')
        {
            script_dir=script_name.substr(0,script_name.length()-i);
            break;
        }
    }
    return "/static/"+script_dir;
}
#define DBUS_DESTTINATION_NAME      "online.syscall"
#define DBUS_INTERFACE_NAME         "online.syscall"
#define DBUS_METHOD_NAME            "status"
int main(int argc,const char *argv[],const char *env[])
{
    try
    {
        cgicc::Cgicc cgi;
        auto env=cgi.getEnvironment();
        auto UserAgent=env.getUserAgent();
        //判断是否在浏览器中,当在浏览器中时尝试跳转
        bool need_redirect=(UserAgent.find("Mozilla")!=UserAgent.npos);
        if((*cgi.getElement("forceapi")).getValue()=="true")
        {
            //当forceapi=true时不跳转
            need_redirect=false;
        }
        if(env.getRequestMethod()=="POST")
        {
            //当采用POST方法时不跳转
            need_redirect=false;
        }
        if(need_redirect)
        {
            //执行跳转
            std::cout<<cgicc::HTTPRedirectHeader(get_redirect_url(env.getScriptName()),true)<<std::endl;
        }
        else
        {
            std::string ret;
            {
                DBusError error;
                dbus_error_init(&error);
                DBusConnection *connection=dbus_bus_get(DBUS_BUS_SYSTEM, &error);
                if(dbus_error_is_set(&error) || connection == NULL)
                {
                    dbus_error_free(&error);
                    return EXIT_FAILURE;
                }
                std::string script_dir;
                {
                    std::string script_name=env.getScriptName();
                    for(size_t i=0; i<script_name.length(); i++)
                    {
                        if(script_name.c_str()[script_name.length()-i]=='/')
                        {
                            script_dir=script_name.substr(0,script_name.length()-i);
                            break;
                        }
                    }
                }
                DBusMessage *msg=dbus_message_new_method_call(DBUS_DESTTINATION_NAME,script_dir.c_str(),DBUS_INTERFACE_NAME,DBUS_METHOD_NAME);
                if(msg == NULL)
                {
                    dbus_error_free(&error);
                    return EXIT_FAILURE;
                }
                DBusMessage *reply=dbus_connection_send_with_reply_and_block(connection,msg,3000,&error);
                dbus_message_unref(msg);
                if(dbus_error_is_set(&error) || reply == NULL)
                {
                    dbus_error_free(&error);
                    return EXIT_FAILURE;
                }
                const char *retstr=NULL;
                dbus_message_get_args(reply,&error,DBUS_TYPE_STRING, &retstr,DBUS_TYPE_INVALID);
                dbus_error_free(&error);
                if(retstr!=NULL)
                {
                    ret=retstr;
                }
                else
                {
                    return EXIT_FAILURE;
                }
                dbus_message_unref(reply);

            }
            //输出接口信息（JSON格式）
            std::cout<<cgicc::HTTPContentHeader("application/json");
            //输出结果
            std::cout<<ret;

        }
    }
    catch(...)
    {
        return EXIT_FAILURE;
    }
    return 0;
}

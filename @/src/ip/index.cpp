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
        if(need_redirect)
        {
            //执行跳转
            std::cout<<cgicc::HTTPRedirectHeader(get_redirect_url(env.getScriptName()),true)<<std::endl;
        }
        else
        {
            //输出接口信息（JSON格式）
            std::cout<<cgicc::HTTPContentHeader("application/json")<<std::endl;
            Json::Value root(Json::objectValue);

            {
                std::string x_real_ip;
                {
                    const char * ip=NULL;
                    if((ip=getenv("HTTP_X_REAL_IP"))!=NULL)
                    {
                        x_real_ip=ip;
                    }
                }
                if(x_real_ip.empty())
                {
                    root["ip"]=env.getRemoteAddr();
                }
                else
                {
                    root["ip"]=x_real_ip;
                }
            }
            {
                Json::StyledStreamWriter writer;
                writer.write(std::cout,root);
            }
        }
    }
    catch(...)
    {
        return EXIT_FAILURE;
    }
    return 0;
}

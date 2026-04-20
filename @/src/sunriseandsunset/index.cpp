#include <cstdlib>
#include <new>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include "hsunriseandsunset.h"
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
            //输出接口信息（JSON格式）
            std::cout<<cgicc::HTTPContentHeader("application/json");
            Json::Value root(Json::objectValue);


            /*
             * 默认未程序的经纬度
             */
            double lat=30.5728;
            double lon=104.0688;

            /*
             * 年月日，默认为本地时间下当天的年月日
             */
            int year=0;
            int month=0;
            int day=0;
            {
                struct tm tm_now= {0};
                time_t time_now=time(NULL);
                tm_now=*localtime(&time_now);
                year=tm_now.tm_year+1900;
                month=tm_now.tm_mon+1;
                day=tm_now.tm_mday;
            }

            /*
             * 时区，默认为东8区
             */
            int timezone=8;

            /*
             * 天顶角(默认为标准天顶角)
             */
            double zenith=HSUNRISEANDSUNSET_ZENITH_STANDARD;

            {
                /*
                 * 处理参数
                 */
                auto has_query_number=[&](std::string name)->bool
                {
                    return !(*cgi.getElement(name)).isEmpty();
                };
                auto get_query_number=[&](std::string name)->double
                {
                    double ret=0;
                    if(!has_query_number(name))
                    {
                        return ret;
                    }
                    try
                    {
                        ret=std::stod((*cgi.getElement(name)).getValue());
                    }
                    catch(...)
                    {

                    }
                    return ret;
                };

                {
                    const char * name="lat";
                    if(has_query_number(name))
                    {
                        lat=get_query_number(name);
                    }
                }
                {
                    const char * name="lon";
                    if(has_query_number(name))
                    {
                        lon=get_query_number(name);
                    }
                }
                {
                    const char * name="year";
                    if(has_query_number(name))
                    {
                        year=get_query_number(name);
                    }
                }
                {
                    const char * name="month";
                    if(has_query_number(name))
                    {
                        month=get_query_number(name);
                    }
                }
                {
                    const char * name="day";
                    if(has_query_number(name))
                    {
                        day=get_query_number(name);
                    }
                }
                {
                    const char * name="timezone";
                    if(has_query_number(name))
                    {
                        timezone=get_query_number(name);
                    }
                }
                {
                    const char * name="zenith";
                    if(has_query_number(name))
                    {
                        zenith=get_query_number(name);
                    }
                }
            }

            {
                /*
                 * 保存输入参数到输出结果
                 */
                root["lat"]=lat;
                root["lon"]=lon;
                root["year"]=year;
                root["month"]=month;
                root["day"]=day;
                root["timezone"]=timezone;
                root["zenith"]=timezone;
            }

            {
                /*
                 * 进行计算
                 */
                hsunriseandsunset_result_t result= {0};
                hsunriseandsunset_date_t date= {0};
                date.year=year;
                date.month=month;
                date.day=day;
                hsunriseandsunset_calculate_ymd(date,lat,lon,&result,zenith);

                {
                    Json::Value sunrise(Json::objectValue);
                    hsunriseandsunset_time_t temp=hsunriseandsunset_utc_to_local(result.sunrise,timezone);
                    sunrise["h"]=temp.h;
                    sunrise["m"]=temp.m;
                    sunrise["s"]=temp.s;
                    root["sunrise"]=sunrise;
                }

                {
                    Json::Value sunset(Json::objectValue);
                    hsunriseandsunset_time_t temp=hsunriseandsunset_utc_to_local(result.sunset,timezone);
                    sunset["h"]=temp.h;
                    sunset["m"]=temp.m;
                    sunset["s"]=temp.s;
                    root["sunset"]=sunset;
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

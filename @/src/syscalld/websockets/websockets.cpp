#include <string>
#include "websockets.h"
#include "sysloginfo.h"
#include "libwebsockets.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <hbox.h>
#include <map>
#include <memory>
static std::recursive_mutex lock;
static std::map<std::string,websocket_interface_t> interfaces;
void websocket_register_interface(const char *uri,websocket_interface_t interface)
{
    if(uri!=NULL && uri[0]!='\0')
    {
        std::lock_guard<std::recursive_mutex> lc(lock);
        interfaces[uri]=interface;
    }
}

void websocket_unregister_interface(const char *uri)
{
    if(uri!=NULL && uri[0]!='\0')
    {
        std::lock_guard<std::recursive_mutex> lc(lock);
        auto it=interfaces.find(uri);
        if(it!=interfaces.end())
        {
            interfaces.erase(it);
        }
    }
}

static void websocket_echo(struct lws *wsi,websockets_connection_context *ctx,void *data,size_t len)
{
    if(!lws_frame_is_binary(wsi))
    {
        LOGI("WebSocket Text Echo:len=%d",(int)len);
        websocket_interface_send_text(ctx,data,len);
    }
    else
    {
        LOGI("WebSocket Binary Echo:len=%d,is_first=%d,is_final=%d",(int)len,(int)lws_is_first_fragment(wsi),(int)lws_is_final_fragment(wsi));
        websocket_interface_send_binary(ctx,data,len,lws_is_first_fragment(wsi),lws_is_final_fragment(wsi));
    }
}

bool websocket_interface_send_text(websockets_connection_context_t *ctx,void *data,size_t datalen)
{
    if(ctx!=NULL && ctx->wsi!=NULL && ctx->on_writeable !=NULL && data!=NULL && datalen !=0)
    {
        try
        {
            std::shared_ptr<uint8_t> buffer(new uint8_t[LWS_PRE+datalen]);
            memset(buffer.get(),0,LWS_PRE);
            memcpy(&buffer.get()[LWS_PRE],data,datalen);
            ctx->on_writeable->push([=]()
            {
                lws_write(ctx->wsi,&buffer.get()[LWS_PRE],datalen,LWS_WRITE_TEXT);
            });
            lws_callback_on_writable(ctx->wsi);
            return true;
        }
        catch(...)
        {

        }
    }
    return false;
}

bool websocket_interface_send_binary(websockets_connection_context_t *ctx,void *data,size_t datalen,int is_start,int is_final)
{
    if(ctx!=NULL && ctx->wsi!=NULL && ctx->on_writeable !=NULL && data!=NULL && datalen !=0)
    {
        try
        {
            std::shared_ptr<uint8_t> buffer(new uint8_t[LWS_PRE+datalen]);
            memset(buffer.get(),0,LWS_PRE);
            memcpy(&buffer.get()[LWS_PRE],data,datalen);
            ctx->on_writeable->push([=]()
            {
                lws_write(ctx->wsi,&buffer.get()[LWS_PRE],datalen,(enum lws_write_protocol)lws_write_ws_flags(LWS_WRITE_BINARY,is_start,is_final));
            });
            lws_callback_on_writable(ctx->wsi);
            return true;
        }
        catch(...)
        {

        }
    }
    return false;
}

static void websocket_interface_detect(websockets_connection_context_t &ctx)
{
    std::lock_guard<std::recursive_mutex> lc(lock);
    //TODO此处设置不同的ctor(构造)、dtor(析构)、process(处理)函数以处理不同接口
    if(interfaces.find(ctx.uri)!=interfaces.end())
    {
        auto it=interfaces.find(ctx.uri);
        ctx.ctor=it->second.ctor;
        ctx.dtor=it->second.dtor;
        ctx.process=it->second.process;
    }

    //默认ECHO
    if(ctx.process==NULL)
    {
        ctx.process=websocket_echo;
    }
}

static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    websockets_connection_context_t *ctx=(websockets_connection_context_t*)user;
    if(ctx==NULL)
    {
        //不是websocket的连接回调
        return 0;
    }
    switch (reason)
    {
    case LWS_CALLBACK_ESTABLISHED:
    {
        LOGI("WebSocket connection established\n");
        memset(ctx->uri,0,sizeof(ctx->uri));
        ctx->ctor=NULL;
        ctx->dtor=NULL;
        ctx->process=NULL;
        ctx->wsi=wsi;
        ctx->on_writeable=new std::queue<std::function<void ()>>();
        lws_hdr_copy(wsi,ctx->uri,sizeof(ctx->uri)-1,WSI_TOKEN_GET_URI);
        LOGI("WebSocket URI %s",ctx->uri);
        websocket_interface_detect(*ctx);
        if(ctx->ctor!=NULL)
        {
            ctx->ctor(ctx);
        }
    }
    break;
    case LWS_CALLBACK_RECEIVE:
    {
        LOGI("WebSocket Received data length=%d",(int)len);
        if(ctx->on_writeable==NULL)
        {
            break;
        }
        if(ctx->process!=NULL)
        {
            ctx->process(wsi,ctx,in,len);
        }
    }
    break;
    case LWS_CALLBACK_SERVER_WRITEABLE:
    {
        LOGI("WebSocket server writeable!\n");
        if(ctx->on_writeable==NULL)
        {
            break;
        }
        if(ctx->on_writeable->empty())
        {
            break;
        }
        auto cb=ctx->on_writeable->front();
        ctx->on_writeable->pop();
        if(cb!=NULL)
        {
            try
            {
                cb();
            }
            catch(...)
            {

            }

        }
        if(!ctx->on_writeable->empty())
        {
            if(ctx->on_writeable->size()>5)
            {
                lws_rx_flow_control(wsi,0);
            }
            lws_callback_on_writable(wsi);
        }
        else
        {
            lws_rx_flow_control(wsi,1);
        }
    }
    break;
    case LWS_CALLBACK_CLOSED:
    {
        if(ctx->dtor!=NULL)
        {
            ctx->dtor(ctx);
        }
        if(ctx->on_writeable!=NULL)
        {
            delete ctx->on_writeable;
            ctx->on_writeable=NULL;
        }
        LOGI("WebSocket connection closed\n");
    }
    break;
    default:
        break;
    }
    return 0;
}

static struct lws_protocols protocols[] =
{
    {"websocket",callback_websocket,sizeof(websockets_connection_context_t),4096,0,NULL,4096},
    {NULL,NULL,0} /* end of list */
};
static struct lws_context * context=NULL;
static void websockets_run()
{
    LOGI("WebSocket Run!");
    while(true)
    {
        {
            std::lock_guard<std::recursive_mutex> lc(lock);
            if(context==NULL)
            {
                break;
            }
            try
            {
                lws_service(context,100);

            }
            catch(...)
            {

            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
void websockets_init()
{
    LOGI("WebSocket Init!");
    struct lws_context_creation_info info= {0};
    info.port=9000;//采用9000作为默认端口
    info.protocols=protocols;
    lws_set_log_level(0xFFFFFFFF,NULL);
    if((context=lws_create_context(&info))==NULL)
    {
        LOGI("WebSocket Init Failed!");
    }
    std::thread run_thread(websockets_run);
    run_thread.detach();
}



void websockets_deinit()
{
    LOGI("WebSocket DeInit!");
    if(context!=NULL)
    {
        std::lock_guard<std::recursive_mutex> lc(lock);
        auto ctx=context;
        context=NULL;
        lws_context_destroy(context);
    }
}



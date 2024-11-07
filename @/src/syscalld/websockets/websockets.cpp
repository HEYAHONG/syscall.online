#include "websockets.h"
#include "sysloginfo.h"
#include "libwebsockets.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <hbox.h>
#include <string>
typedef struct
{
    uint8_t txringbuffer[4096];
    char    uri[4096];
} websockets_connection_context_t;
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    websockets_connection_context_t *ctx=(websockets_connection_context_t*)user;
    if(ctx==NULL)
    {
        //不是websocket的连接回调
        return 0;
    }
    hringbuf *txbuf=hringbuf_get(ctx->txringbuffer,sizeof(ctx->txringbuffer));
    if(txbuf==NULL)
    {
        LOGI("WebSocket TxBuf Error");
        return 0;
    }
    switch (reason)
    {
    case LWS_CALLBACK_ESTABLISHED:
    {
        LOGI("WebSocket connection established\n");
        memset(ctx->uri,0,sizeof(ctx->uri));
        lws_hdr_copy(wsi,ctx->uri,sizeof(ctx->uri)-1,WSI_TOKEN_GET_URI);
        LOGI("WebSocket URI %s",ctx->uri);
    }
    break;
    case LWS_CALLBACK_RECEIVE:
    {
        LOGI("WebSocket Received data length=%d",(int)len);
        hringbuf_input(txbuf,(uint8_t *)in,len);
        lws_callback_on_writable(wsi);
        lws_rx_flow_control(wsi,0);
    }
    break;
    case LWS_CALLBACK_SERVER_WRITEABLE:
    {
        LOGI("WebSocket server writeable!\n");
        uint8_t buffer[LWS_PRE+1024]= {0};
        size_t buffer_len=hringbuf_output(txbuf,&buffer[LWS_PRE],sizeof(buffer)-LWS_PRE);
        if(buffer_len==0)
        {
            //结束写入
            lws_rx_flow_control(wsi,1);
            break;
        }
        lws_write(wsi,&buffer[LWS_PRE],buffer_len,LWS_WRITE_TEXT);
        lws_callback_on_writable(wsi);
    }
    break;
    case LWS_CALLBACK_CLOSED:
        LOGI("WebSocket connection closed\n");
        break;
    default:
        break;
    }
    return 0;
}

static struct lws_protocols protocols[] =
{
    {"websocket",callback_websocket,sizeof(websockets_connection_context_t),1024,0,NULL,1024},
    {NULL,NULL,0} /* end of list */
};
static struct lws_context * context=NULL;
static std::recursive_mutex lock;
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
            lws_service(context,100);
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



#ifndef __WEBSOCKETS_H__
#define __WEBSOCKETS_H__
#ifdef __cplusplus
#include <queue>
#include <functional>
#endif // __cplusplus
#include "libwebsockets.h"
#include "hbox.h"
#ifdef __cplusplus
extern "C"
{
#endif


void websockets_init();

struct websockets_connection_context;
typedef struct websockets_connection_context websockets_connection_context_t;


typedef struct
{
    void (*ctor)(websockets_connection_context_t *ctx);
    void (*dtor)(websockets_connection_context_t *ctx);
    void (*process)(struct lws *wsi,struct websockets_connection_context *ctx,void *data,size_t len);
} websocket_interface_t;

/** \brief 注册websocket接口
 *
 * \param uri const char* 接口uri,对于syscalld需要添加前缀/ws/
 * \param interface websocket_interface_t 接口函数
 *
 */
void websocket_register_interface(const char *uri,websocket_interface_t interface);

/** \brief 注销websocket接口
 *
 * \param uri const char*  接口uri,对于syscalld需要添加前缀/ws/
 *
 */
void websocket_unregister_interface(const char *uri);

/** \brief 通过TEXT方式发送数据
 *
 * \param ctx websockets_connection_context_t* 上下文
 * \param data void* 数据
 * \param datalen size_t 数据长度
 * \return bool 是否加入发送队列
 *
 */
bool websocket_interface_send_text(websockets_connection_context_t *ctx,void *data,size_t datalen);

/** \brief 通过BINARY方式发送数据
 *
 * \param ctx websockets_connection_context_t* 上下文
 * \param data void* 数据
 * \param datalen size_t 数据长度
 * \return bool 是否加入发送队列
 *
 */
bool websocket_interface_send_binary(websockets_connection_context_t *ctx,void *data,size_t datalen,int is_start,int is_final);

void websockets_deinit();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
struct websockets_connection_context
{
    std::queue<std::function<void ()>> *on_writeable;
    char    uri[4096];//uri存储,'\0'字符之后的空间可用于其它用途,如存储用户参数，对于一个确定的接口uri是固定的，故而剩余空间也是确定的。
    struct lws *wsi;
    void (*ctor)(websockets_connection_context_t *ctx);
    void (*dtor)(websockets_connection_context_t *ctx);
    void (*process)(struct lws *wsi,struct websockets_connection_context *ctx,void *data,size_t len);
};
#endif // __cplusplus

#endif // __WEBSOCKETS_H__

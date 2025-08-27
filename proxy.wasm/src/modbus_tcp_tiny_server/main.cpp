#include <HCPPBox.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <signal.h>


typedef struct
{
    std::map<modbus_data_address_t,bool> coils;
    std::map<modbus_data_address_t,bool> discrete_inputs;
    std::map<modbus_data_address_t,modbus_data_register_t> registers;
    std::map<modbus_data_address_t,modbus_data_register_t> input_registers;
} modbus_data_t;
static std::map<modbus_rtu_slave_tiny_context_t*,modbus_data_t> mb_data;
static std::recursive_mutex mb_data_lock;
static bool    read_coil(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        if(mb_data[ctx].coils.find(addr)!=mb_data[ctx].coils.end())
        {
            return mb_data[ctx].coils[addr];
        }
    }
    return false;
}
static bool    read_discrete_input(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        if(mb_data[ctx].discrete_inputs.find(addr)!=mb_data[ctx].discrete_inputs.end())
        {
            return mb_data[ctx].discrete_inputs[addr];
        }
    }
    return !read_coil(ctx,addr);
}
static modbus_data_register_t  read_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        if(mb_data[ctx].registers.find(addr)!=mb_data[ctx].registers.end())
        {
            return mb_data[ctx].registers[addr];
        }
    }
    return 0xDEAD;
}
static modbus_data_register_t  read_input_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        if(mb_data[ctx].input_registers.find(addr)!=mb_data[ctx].input_registers.end())
        {
            return mb_data[ctx].input_registers[addr];
        }
    }
    return ~ read_holding_register(ctx, addr);
}
static void    write_coil(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,bool value)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        mb_data[ctx].coils[addr]=value;
    }
}
static void    write_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,modbus_data_register_t value)
{
    std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
    if(mb_data.find(ctx)!=mb_data.end())
    {
        mb_data[ctx].registers[addr]=value;
    }
}
static void modbus_init_ctx(modbus_rtu_slave_tiny_context_t* ctx)
{
    if(ctx==NULL)
    {
        return;
    }
    mb_data[ctx]=modbus_data_t();
    ctx->read_coil=read_coil;
    ctx->read_discrete_input=read_discrete_input;
    ctx->read_holding_register=read_holding_register;
    ctx->read_input_register=read_input_register;
    ctx->write_coil=write_coil;
    ctx->write_holding_register=write_holding_register;
}


static void bufferevent_read_callback(struct bufferevent *bev, void *ctx)
{
    if(bev==NULL || ctx==NULL)
    {
        return;
    }
    modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t *tcp_server_tiny=(modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t *)ctx;
    uint8_t buffer[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t buffer_length=bufferevent_read(bev,buffer,sizeof(buffer));
    if(buffer_length > 0)
    {
        //接收到正确的数据
        auto reply=[](modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t* ctx,const uint8_t *adu,size_t adu_length)
        {
            if(ctx==NULL|| adu==NULL ||adu_length==0)
            {
                return;
            }
            bufferevent_write((struct bufferevent *)ctx->usr,adu,adu_length);
        };
        tcp_server_tiny->reply=reply;
        tcp_server_tiny->usr=(void *)(intptr_t)bev;
        modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_parse_input(tcp_server_tiny,buffer,buffer_length);
    }
}

static void bufferevent_event_callback(struct bufferevent *bev, short what, void *ctx)
{
    if(bev==NULL || ctx==NULL)
    {
        return;
    }
    modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t *tcp_server_tiny=(modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t *)ctx;
    if((what & BEV_EVENT_ERROR)!=0 || (what &BEV_EVENT_EOF)!=0)
    {
        bufferevent_free(bev);
        std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
        auto it=mb_data.find(&tcp_server_tiny->slave);
        if(it!=mb_data.end())
        {
            mb_data.erase(it);
        }
        delete tcp_server_tiny;
    }
}

static void evconnlistener_callback(struct evconnlistener *lev, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *ptr)
{
    struct event_base *base=evconnlistener_get_base(lev);
    struct bufferevent *bufferevt=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    if(bufferevt!=NULL)
    {
        std::lock_guard<std::recursive_mutex> lock(mb_data_lock);
        modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t *tcp_server_tiny=new modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_t(modbus_tcp_gateway_server_context_with_modbus_rtu_tiny_context_default());
        modbus_init_ctx(&tcp_server_tiny->slave);
        bufferevent_setcb(bufferevt,bufferevent_read_callback,NULL,bufferevent_event_callback,tcp_server_tiny);
        bufferevent_enable(bufferevt,EV_READ);
    }
};

static void signal_callback(evutil_socket_t sig, short events, void *user_data)
{
    struct event_base *base = (struct event_base *)user_data;
    struct timeval delay = { 2, 0 };//设置延迟时间 2s
    event_base_loopexit(base, &delay);//延时 2s 退出
}

int main()
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    struct event_base *base = event_base_new();
    if(base==NULL)
    {
        return -1;
    }

    struct evconnlistener *lev=NULL;
    {
        //本地502端口
        HCPPSocketAddressIPV4 addr= {0};
        addr.sin_family=AF_INET;
        addr.sin_port=htons(502);
        lev=evconnlistener_new_bind(base,evconnlistener_callback,NULL,LEV_OPT_CLOSE_ON_FREE,100,(struct sockaddr *)&addr,sizeof(HCPPSocketAddressIPV4));
    }

    if(lev==NULL)
    {
        event_base_free(base);
        return -1;
    }

    evconnlistener_enable(lev);

    struct event *signal_event=evsignal_new(base, SIGINT, signal_callback, (void *)base);
    if(signal_event!=NULL)
    {
        event_add(signal_event, NULL);
    }

    event_base_dispatch(base);


    if(signal_event!=NULL)
    {
        evsignal_del(signal_event);
        signal_event=NULL;
    }

    if(lev!=NULL)
    {
        evconnlistener_free(lev);
        lev=NULL;
    }

    if(base!=NULL)
    {
        event_base_free(base);
        base=NULL;
    }

    return 0;
}

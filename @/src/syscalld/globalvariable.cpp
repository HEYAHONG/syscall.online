#include "globalvariable.h"
#include "sysloginfo.h"
#include "inttypes.h"
#include "stdint.h"
#include <queue>
#include <functional>
static std::chrono::system_clock::time_point boot_timepoint=std::chrono::system_clock::now();
boot_timepoint_t gv_get_boot_time_point()
{
    return boot_timepoint;
}
uint64_t gv_get_boot_time_point_seconds()
{
    return (gv_get_boot_time_point().time_since_epoch()/std::chrono::seconds(1));
}
static boost::uuids::random_generator random_generator;
static uuid_t global_uuid=random_generator();
uuid_t gv_get_global_uuid()
{
    return global_uuid;
}
uuid_t gv_get_new_uuid()
{
    return random_generator();
}
std::string gv_uuid_to_string(uuid_t uuid)
{
    return boost::uuids::to_string(uuid);
}

//此队列仅在初始化时使用，因此不加锁
static std::queue<std::function<int()>> init_queue;
extern "C" void __gv_add_init_func__(int(*_init)());
void __gv_add_init_func__(int(*_init)())
{
    init_queue.push(_init);
}

void gv_init()
{
    LOGI("Boot Time:%" PRIu64,gv_get_boot_time_point_seconds());
    LOGI("UUID:%s",gv_uuid_to_string().c_str());

    {
        size_t total=init_queue.size();
        size_t index=0;
        //处理init_queue
        while(init_queue.size()>0)
        {
            index++;
            std::function<int()> cb=init_queue.front();
            init_queue.pop();
            if(cb!=NULL)
            {
                LOGI("Init(%d/%d):%d",(int)index,(int)total,(int)cb());
            }
        }
    }
}

void gv_deinit()
{

}

static bool isrunning=true;
bool gv_is_running()
{
    return isrunning;
}

void gv_set_running(bool _is_running)
{
    isrunning=_is_running;
}



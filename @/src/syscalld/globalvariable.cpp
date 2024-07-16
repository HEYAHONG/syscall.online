#include "globalvariable.h"
#include "sysloginfo.h"
#include "inttypes.h"
#include "stdint.h"
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

void gv_init()
{
    LOGI("Boot Time:%" PRIu64,gv_get_boot_time_point_seconds());
    LOGI("UUID:%s",gv_uuid_to_string().c_str());
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



#ifndef  __COMMON_FSLOADER_H__
#define __COMMON_FSLOADER_H__
#include "stdbool.h"
#ifdef __cplusplus
extern "C"
{
#endif //cplusplus

void fsloader_init();

/** \brief 同步文件系统，在某些文件系统中需要手动保存加载
 *
 * \param saveload bool true=保存,false=加载
 *
 */
void fsloader_sync(bool saveload);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __COMMON_FSLOADER_H__

#ifndef __COMMON_FONT_H__
#define __COMMON_FONT_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** \brief 获取存放字体的文件目录
 *
 * \return const char* 目录
 *
 */
const char *font_get_root();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <string>
/** \brief 获取默认字体（通常是找到的第一个字体）
 *
 * \return std::string 字体文件名称
 *
 */
std::string font_get_default_font();
#endif // __cplusplus

#endif // __COMMON_H__


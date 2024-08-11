#include "fsloader.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include "unistd.h"

const char * const TAG="fsloader";

int fsloader_chdir(const char * const new_work_dir)
{
    return chdir(new_work_dir);
}

#ifdef __EMSCRIPTEN__
EM_JS(int,fsloader_ls_in_nodejs,(), {if (typeof global === 'object' && typeof window !== 'object') return 1; else return 0;});
#endif // __EMSCRIPTEN__

/*
 *加载文件系统(主要用于初始化WASM文件系统(用户部分)),默认情况下，EMSCRIPTEN的文件系统都在内存(MEMFS中)中。视情况需要启用NODEFS或者IDBFS
 *主要完成如下工作:
 *  在WASM模式下挂载一个可永久保存的文件系统到/var
 *
 */
void fsloader_init()
{
#ifdef __EMSCRIPTEN__
    emscripten_log(EM_LOG_CONSOLE|EM_LOG_INFO,"%s::fsloader started!",TAG);
    emscripten_log(EM_LOG_CONSOLE|EM_LOG_INFO,"%s::fsloader_source_root:%s!",TAG,FSLOADER_SOURCE_ROOT);
    {
        //写入文件
        EM_ASM(
            FS.writeFile("fsloader.common.flag", "loaded");
        );
    }
    if(fsloader_ls_in_nodejs()==1)
    {
        emscripten_log(EM_LOG_CONSOLE|EM_LOG_INFO,"%s::nodejs environment!",TAG);

#ifndef NODERAWFS
        //挂载NODEFS
        EM_ASM(
            FS.mkdir('/var');
            FS.mount(NODEFS, { root: '.' }, '/var');
        );
#endif // NODERAWFS
    }
    else
    {
        emscripten_log(EM_LOG_CONSOLE|EM_LOG_INFO,"%s::browser environment!",TAG);
        //挂载IDBFS
        EM_ASM(
            FS.mkdir('/var');
            FS.mount(IDBFS, {autoPersist: true}, '/var');
        );
    }
    fsloader_sync(false);
#else

#endif // __EMSCRIPTEN__
}

void fsloader_sync(bool saveload)
{
#ifdef __EMSCRIPTEN__
    if(saveload)
    {
        EM_ASM(FS.syncfs(false, function (err) {}));
    }
    else
    {
        EM_ASM(FS.syncfs(true, function (err) {}));
    }
#endif // __EMSCRIPTEN__
}

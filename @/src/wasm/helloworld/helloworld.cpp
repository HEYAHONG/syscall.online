#include <iostream>
#include "fsloader.h"

int main()
{
    //加载文件系统(主要进行一些用户操作)
    fsloader_init();

    std::cout << "hello world" << std::endl;
    return 0;
}

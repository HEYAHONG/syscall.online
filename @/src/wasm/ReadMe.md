# 说明

本目录主要为WebAssembly代码，执行安装时的默认根目录为/var/www/html/wasm,外部访问路径为 主机名[:端口]/wasm/。

# 本地调试

部分代码支持不编译成WebAssembly代码，直接编译成本地可执行文件。

可直接使用CMake编译。

需要注意的是除了安装好C/C++开发环境,还需要安装以下库:

- [libsdl](https://libsdl.org/):包括libsdl（版本1）与libsdl2.注意：某些组件（如SDL _ttf）可能要单独安装,如不能成功编译需要安装缺失的组件。

# 测试

在调试WASM程序时，可采用编译器自动生成的html代码，直接在浏览器上打开即可。

实际使用时需要自行设计html代码。
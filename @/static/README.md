# HTTP接口

**一般情况下，接口的访问路径为/api/前缀+接口名称。**

## syscalld接口

syscalld为守护进程，文档路径为<a href="api/syscalld/">syscalld</a>。

- <a href="api/syscalld/status/">syscalld/status/</a>:返回syscalld状态信息。
## 杂项接口

- <a href="api/ip">ip/</a>:返回一些IP信息。

# 测试

## WASM

wasm路径：<a href="/wasm/">wasm</a>。
- <a href="/wasm/helloworld/helloworld.html">helloworld</a>:C++语言的helloworld.
- <a href="/wasm/base_sdl/base_sdl.html">base_sdl</a>:WASM的（SDL）图形程序基础测试.
- <a href="/wasm/base_sdl2/base_sdl2.html">base_sdl2</a>:WASM的（SDL2）图形程序基础测试.
- <a href="/wasm/PDCurses">PDCurses</a>:[PDCurses](https://pdcurses.org/)库的demo程序，可演示Curses程序（早期终端的图形化程序）。每一个html就是一个演示程序。
- <a href="/wasm/PDCurses/ModbusTCPClient/ModbusTCPClient.html">ModbusTCPClient</a>：Modbus TCP测试程序。需要多线程支持。启用了socket代理且需要套接字支持。

### 多线程

若有些wasm程序不能正常工作(比如已启用pthread且使用PROXY_TO_PTHREAD编译选项的程序)，尝试通过https方式访问相应地址(可忽略浏览器的警告或者安装[自签证书](https://hyhsystem.cn/))。

### 套接字

由于浏览器不能直接操作套接字，需要运行一个websocket服务器进行代理socket操作。

若未特殊说明，本站默认的代理为ws://localhost:58080（即在本地运行服务器且端口号使用58080）,代理服务器的源代码可见[emscripten](https://github.com/emscripten-core/emscripten)源代码的[tools/websocket_to_posix_proxy](https://github.com/emscripten-core/emscripten/tree/main/tools/websocket_to_posix_proxy)。

此处提供已编译好的二进制文件：<a href="assets/websocket_to_posix_proxy.zip">websocket_to_posix_proxy.zip</a>

若为运行代理直接运行需要socket的程序，可能导致以下异常：

- 若启用了socket代理（PROXY_POSIX_SOCKET）时，使用socket进行网络操作时阻塞。表现为程序卡死。
- 控制台中报***无法连接ws://localhost:58080***错误。

# WebSocket接口



采用nginx转发。接口路径为/ws,即连接地址为为ws://syscall.online/ws。

注意：

- 由于采用了nginx代理，设定了300s超时，即若300s内无任何通信将断开连接。
- 默认情况下syscalld只支持WebSocket的TEXT模式。
- 默认情况下，当URI未被使用时，syscalld的WebSocket接口执行的操作为回显(即直接返回客户端发送的数据),不会主动发送数据。

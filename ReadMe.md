# 说明

主要一些服务器端物联网代码的测试代码,主要用于物联网测试。

测试地址为：[https://syscall.online](https://syscall.online)

**注意：某些wasm特性需要在https访问(localhost测试时可不使用https)。**

# 特点

- 采用nginx作为静态文件服务器
- 采用cgi技术提供接口
- 采用docker技术使代码更有容易部署
- 采用make作为主要构建工具
- 支持WebAssembly(采用[emscripten](https://emscripten.org/))
- 支持WebSocket(采用[libwebsockets](https://libwebsockets.org/)库)
- 采用`s6`作为多服务非特权容器服务管理工具
- 采用`libevent`作为网络服务库

# 网站

- [@](@)：作为子域名为www或不使用子域名时的网站源代码。
- [proxy.wasm](proxy.wasm):作为子域名为proxy.wasm的网址源代码。

注意：为适应在一个服务器上搭建多个虚拟主机（可使用同一端口使用域名区分不同虚拟主机），通常部署完成后会启动监听非http端口的docker容器,因此，如需使用http端口，需要配置nginx反向代理将请求转发至相应网站的端口。

# 要求

## docker

由于使用了docker技术,需要保证docker命令与docker-compose命令能正常运行。

## make

由于采用了make作为主要构建工具，需要保证make命令能正常运行。

## coreutils

在Makefile中使用了coreutils的相关工具，因此需要安装coreutils（对于ubuntu系统而言，一般无需手动安装）

## findutils

在Makefile中使用了findutils的相关工具，因此需要安装findutils（对于ubuntu系统而言，一般无需手动安装）

# 操作

注意：本源代码不得放在具有空格的路径下，否则会出现异常。若当前用户不是root用户且不在docker用户组，请在下列操作中使用sudo提升权限，否则将操作失败。

## 部署

```bash
make
```

## 清理

```bash
make clean
```


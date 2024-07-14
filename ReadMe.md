# 说明

主要一些服务器端物联网代码的测试代码,主要用于物联网测试。

测试地址为：http://syscall.online

# 特点

- 采用nginx作为静态文件服务器
- 采用cgi技术提供接口
- 采用docker技术使代码更有容易部署
- 采用make作为主要构建工具

# 网站

- [@](@)：作为子域名为www或不使用子域名时的网站源代码。

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

注意：本源代码不得放在具有空格的路径下，否则会出现异常。

## 部署

```bash
make
```

## 清理

```bash
make clean
```


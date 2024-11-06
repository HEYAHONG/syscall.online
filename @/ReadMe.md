# 说明

本网站主要是用于子域名为www或者子域名不存在时的网站。

注意：当部署完成后占用65080端口，如需在80端口访问需要使用反向代理服务器转发。

# nginx反向代理服务器设置

为使cgi程序能够获取正确的IP地址，需要在占用80端口的nginx的location配置中添加下列代码：

```nginx
proxy_set_header   Host             $host;
proxy_set_header   X-Real-IP        $remote_addr;
proxy_set_header   X-Forwarded-For  $proxy_add_x_forwarded_for;
proxy_set_header   X-Forwarded-Proto $scheme;
```


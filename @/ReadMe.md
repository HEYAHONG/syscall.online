# 说明

本网站主要是用于子域名为www或者子域名不存在时的网站。

注意：当部署完成后占用65080端口，如需在80端口访问需要使用反向代理服务器转发。

URL路径说明

| 路径    | 说明            | 备注                                          |
| ------- | --------------- | --------------------------------------------- |
| /       | 根              | 支持`http`、`https`请求,默认跳转至/static路径 |
| /static | 静态资源        | 支持`http`、`https`请求                       |
| /wasm   | `wasm`程序      | 支持`http`、`https`请求                       |
| /api    | api请求         | 支持`http`、`https`请求                       |
| /ws     | `WebSocket`请求 | 支持`ws`、`wss`请求                           |



# nginx反向代理服务器设置

## CGI设置

为使cgi程序能够获取正确的IP地址，需要在占用80端口的nginx的location配置中添加下列代码：

```nginx
proxy_set_header   Host             $host;
proxy_set_header   X-Real-IP        $remote_addr;
proxy_set_header   X-Forwarded-For  $proxy_add_x_forwarded_for;
proxy_set_header   X-Forwarded-Proto $scheme;
```

## WebSocket设置

server块前的配置：

```nginx
map $http_upgrade $connection_upgrade {
        default          keep-alive;
        'websocket'      upgrade;
}

```

server块内的设置(匹配转发路径)：

```nginx
location /ws {
            proxy_pass http://[::1]:65080;#设定为实际服务器地址
            proxy_http_version 1.1;
            proxy_read_timeout 300s;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection $connection_upgrade;
            proxy_set_header   Host             $host;
            proxy_set_header   X-Real-IP        $remote_addr;
            proxy_set_header   X-Forwarded-For  $proxy_add_x_forwarded_for;
            proxy_set_header   X-Forwarded-Proto $scheme;
        }

```


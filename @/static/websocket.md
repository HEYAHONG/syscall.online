# WebSocket

采用nginx转发。接口路径为/ws,即连接地址为为ws://syscall.online/ws。

注意：

- 由于采用了nginx代理，设定了300s超时，即若300s内无任何通信将断开连接。
- 默认情况下，当URI未被使用时，syscalld的WebSocket接口执行的操作为回显(即直接返回客户端发送的数据),不会主动发送数据。
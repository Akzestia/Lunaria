## Quic Client

```c++
std::unique_ptr<QuicClient> m_client = std::make_unique<QuicClient>("serverIp", port, thread_number, "alpn", "server.cert", "server.key");
```
> Note: 
> it's recommended to keep client private, and use it only for making requests. If u want to store any client related data, please create a Client Wrapper

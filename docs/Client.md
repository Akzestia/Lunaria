## Quic Client

```c++
std::unique_ptr<QuicClient> m_client = std::make_unique<QuicClient>("serverIp", port, thread_number, "alpn", "server.cert", "server.key");
```

## Quic Server

## Todo 

## Default init
```c++
QuicServer *server = new QuicServer("localhost", port, number_of_threads, "<alpn>" "<path_to_cert>", "<path_to_key>")
```



## Server's constructor
```c++
QuicServer(const char *Host, const uint16_t UdpPort, const size_t ThreadNumber, const char *Alpn, const char *cert, const char *key = nullptr);
```

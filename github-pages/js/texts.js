const QuicClient = `
class QuicClient {
  public:
    void Connect();

    void Disconnect();

    HQUIC getConnection();

    void send(const absl::Cord &message);

    QuicClient(const char *Host, const uint16_t UdpPort, const char* Alpn, const char *cert,
               const char *key = nullptr);

    ~QuicClient();

  private:
    const QUIC_API_TABLE *MsQuic = nullptr;
    static std::condition_variable cv;
    static std::mutex cv_m;
    static bool disconnected;
    HQUIC TunnelStream;

    bool openTunnel();

    bool AuthRequest(const absl::Cord &);

    void ClientLoadConfiguration(const char *cert, const char *key);

    _IRQL_requires_max_(PASSIVE_LEVEL)
        _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
        ClientConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                                 _Inout_ QUIC_CONNECTION_EVENT *Event);

    static QUIC_STATUS QUIC_API StaticClientConnectionCallback(
        _In_ HQUIC Connection, _In_opt_ void *Context,
        _Inout_ QUIC_CONNECTION_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
        ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                             _Inout_ QUIC_STREAM_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                               _Inout_ QUIC_STREAM_EVENT *Event);

    QUIC_CREDENTIAL_CONFIG CredConfig;
    uint16_t UdpPort;
    HQUIC Connection = nullptr;
    HQUIC Listener = nullptr;
    HQUIC Registration = nullptr;
    HQUIC Configuration = nullptr;
    uint8_t *ResumptionTicket = nullptr;
    uint16_t ResumptionTicketLength;
    QUIC_STATUS Status;
    const char *Host;
    const QUIC_REGISTRATION_CONFIG RegConfig = {
        "Client", QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME};
    const QUIC_BUFFER Alpn;

    uint32_t DecodeHexBuffer(_In_z_ const char *HexBuffer,
                             _In_ uint32_t OutBufferLen,
                             _Out_writes_to_(OutBufferLen, return)
                                 uint8_t *OutBuffer);

    uint8_t DecodeHexChar(_In_ char c);
};`;


const QuicServer = `
class QuicServer : protected ConnectionManager,
                   protected DbManager,
                   protected PeerHandler {
  public:
    void Start();

    void Close();

    bool getIsRunning();

    QuicServer(const char *Host, const uint16_t UdpPort, const char* Alpn, const char *cert,
               const char *key = nullptr);

    ~QuicServer();

  private:
    using ConnectionManager::addUser;
    using ConnectionManager::removeUser;
    using DbManager::getContacts;
    using DbManager::getGraphs;
    using DbManager::getMessages;
    using DbManager::getUser;
    using DbManager::test;
    using PeerHandler::GetPeers;
    using PeerHandler::HandlePeer;
    using PeerHandler::onPeerShutdown;
    using PeerHandler::SetPeer;

    const QUIC_API_TABLE *MsQuic = nullptr;
    static std::condition_variable cv;
    static std::condition_variable server_status;
    static std::mutex cv_m;
    static std::mutex server_status_m;
    static bool disconnected;

    static bool getUserCreds(HQUIC, void*);

    bool getUserCreds(HQUIC);

    static void send(HQUIC, void *);

    void send(HQUIC);

    void ServerLoadConfiguration(const char *hash);

    void ServerLoadConfiguration(const char *cert, const char *key);

    _IRQL_requires_max_(PASSIVE_LEVEL)
        _Function_class_(QUIC_LISTENER_CALLBACK) QUIC_STATUS QUIC_API
        ServerListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                               _Inout_ QUIC_LISTENER_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticServerListenerCallback(_In_ HQUIC Listener, _In_opt_ void *Context,
                                 _Inout_ QUIC_LISTENER_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_CONNECTION_CALLBACK) QUIC_STATUS QUIC_API
        ServerConnectionCallback(_In_ HQUIC Connection, _In_opt_ void *Context,
                                 _Inout_ QUIC_CONNECTION_EVENT *Event);

    static QUIC_STATUS QUIC_API StaticServerConnectionCallback(
        _In_ HQUIC Connection, _In_opt_ void *Context,
        _Inout_ QUIC_CONNECTION_EVENT *Event);

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_STREAM_CALLBACK) QUIC_STATUS QUIC_API
        ClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                             _Inout_ QUIC_STREAM_EVENT *Event);

    static QUIC_STATUS QUIC_API
    StaticClientStreamCallback(_In_ HQUIC Stream, _In_opt_ void *Context,
                               _Inout_ QUIC_STREAM_EVENT *Event);

    QUIC_CREDENTIAL_CONFIG CredConfig;
    QUIC_ADDR Address = {0};
    uint16_t UdpPort;
    HQUIC Connection = nullptr;
    HQUIC Registration = nullptr;
    HQUIC Configuration = nullptr;
    HQUIC Listener = nullptr;
    QUIC_STATUS Status;
    char *Host = nullptr;
    char *cert = nullptr;
    char *key = nullptr;
    const QUIC_REGISTRATION_CONFIG RegConfig = {
        "Server", QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME};
    const QUIC_BUFFER Alpn; // wq-vvv-01 
    std::atomic<bool> isRunning = false;
    std::thread serverThread;

    uint32_t DecodeHexBuffer(_In_z_ const char *HexBuffer,
                             _In_ uint32_t OutBufferLen,
                             _Out_writes_to_(OutBufferLen, return)
                                 uint8_t *OutBuffer);

    uint8_t DecodeHexChar(_In_ char c);

    QuicServer();
};`;


const Wayland = `
class WaylandOutputListener {
  public:
    static void handleGeometry(void *data, wl_output *output, int x, int y,
                               int physical_width, int physical_height,
                               int subpixel, const char *make,
                               const char *model, int transform) {
        auto *listener = static_cast<WaylandOutputListener *>(data);
        std::cout << "Output Geometry: x=" << x << ", y=" << y
                  << ", physical_width=" << physical_width
                  << ", physical_height=" << physical_height << std::endl;
    }

    static void handleMode(void *data, wl_output *output, uint32_t flags,
                           int width, int height, int refresh) {
        auto *listener = static_cast<WaylandOutputListener *>(data);
        std::cout << "Output Mode: width=" << width << ", height=" << height
                  << ", refresh=" << refresh << std::endl;
    }
};

class WaylandConnection {
  public:
    WaylandConnection()
        : display(nullptr), registry(nullptr), shm(nullptr),
          screencopy_manager(nullptr), output(nullptr) {}

    void init() {
        display = wl_display_connect(nullptr);
        if (!display) {
            std::cerr << "Failed to connect to Wayland display." << std::endl;
            return;
        }

        std::cout << "Successfully connected to Wayland display." << std::endl;

        registry = wl_display_get_registry(display);
        if (!registry) {
            std::cerr << "Failed to get Wayland registry." << std::endl;
            wl_display_disconnect(display);
            return;
        }

        static const wl_registry_listener registryListener = {
            registryHandleGlobal, registryHandleGlobalRemove};
        wl_registry_add_listener(registry, &registryListener, this);

        wl_display_roundtrip(display);
    }

    void cleanup() {
        if (display) {
            wl_display_disconnect(display);
            display = nullptr;
            std::cout << "Disconnected from Wayland display." << std::endl;
        }
    }

    void print() {
        auto iter_b = globalObjects.begin();
        auto iter_e = globalObjects.end();
        int count = 0;
        for (; iter_b != iter_e; iter_b++) {
            // std::cout << iter_b->second << "  Count: " << ++count << "\n";
            if (iter_b->second == "wl_output") {
                output = reinterpret_cast<wl_output *>(wl_registry_bind(
                    registry, iter_b->first, &wl_output_interface, 1));
                if (!output) {
                    std::cerr << "Failed to bind wl_output." << std::endl;
                }
            }
        }
    }

    wl_output *getOutput() { return output; }

    void setupListeners() {
        if (!output) {
            std::cerr << "No wl_output object available." << std::endl;
            return;
        }

        static const wl_output_listener outputListener = {
            WaylandOutputListener::handleGeometry,
            WaylandOutputListener::handleMode,
        };
        wl_output_add_listener(output, &outputListener, this);
    }

    void captureScreen() {
        if (!screencopy_manager) {
            std::cerr << "Screencopy manager is not available." << std::endl;
            return;
        }

        if (!output) {
            std::cerr << "No output available to capture." << std::endl;
            return;
        }

        zwlr_screencopy_frame_v1 *frame =
            zwlr_screencopy_manager_v1_capture_output(screencopy_manager, 0,
                                                      output);
        zwlr_screencopy_frame_v1_add_listener(frame, &frameListener, this);

        wl_display_roundtrip(display);
    }

  private:
    static void registryHandleGlobal(void *data, wl_registry *registry,
                                     uint32_t id, const char *interface,
                                     uint32_t version) {
        auto *connection = static_cast<WaylandConnection *>(data);
        connection->globalObjects[id] = interface;

        if (strcmp(interface, wl_shm_interface.name) == 0) {
            connection->shm = reinterpret_cast<wl_shm *>(
                wl_registry_bind(registry, id, &wl_shm_interface, 1));
        } else if (strcmp(interface,
                          zwlr_screencopy_manager_v1_interface.name) == 0) {
            connection->screencopy_manager =
                reinterpret_cast<zwlr_screencopy_manager_v1 *>(wl_registry_bind(
                    registry, id, &zwlr_screencopy_manager_v1_interface, 1));
        }
    }

    static void registryHandleGlobalRemove(void *, wl_registry *, uint32_t) {}

    static void shmFormat(void *data, wl_shm *shm, uint32_t format) {
        // Handle supported SHM formats here
    }

    static void frameBuffer(void *data, zwlr_screencopy_frame_v1 *frame,
                            uint32_t format, uint32_t width, uint32_t height,
                            uint32_t stride) {
        std::cout << "Frame properties: format=" << format
                  << ", width=" << width << ", height=" << height
                  << ", stride=" << stride << std::endl;

        int fd = shm_open("/example_shm", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            std::cout << "XXXX\n";
        }
        WaylandConnection *connection = static_cast<WaylandConnection *>(data);
        wl_buffer *buffer = connection->createOrObtainBuffer(
            connection->shm, width, height, stride, fd, 1024 * 1024 * 4);

        if (!buffer) {
            std::cerr << "Failed to create or obtain buffer." << std::endl;
            close(fd);
            return;
        }

        zwlr_screencopy_frame_v1_copy(frame, buffer);

        uint8_t *datax = static_cast<uint8_t *>(mmap(nullptr, stride * height,
                                                     PROT_READ | PROT_WRITE,
                                                     MAP_SHARED, fd, 0));

        if (datax == MAP_FAILED) {
            std::cerr << "Failed to map shared memory into process."
                      << std::endl;
            close(fd);
            wl_buffer_destroy(buffer);
            return;
        }

        connection->savePngImage("output.png", datax, width, height, stride);

        close(fd);
        wl_buffer_destroy(buffer);
    }
    void copyFrameToBuffer(zwlr_screencopy_frame_v1 *frame, wl_buffer *buffer) {
        zwlr_screencopy_frame_v1_copy(frame, buffer);
    }

    static void frameReady(void *data, zwlr_screencopy_frame_v1 *frame,
                           uint32_t tv_sec_hi, uint32_t tv_sec_lo,
                           uint32_t tv_nsec) {
        std::cout << "Frame ready." << std::endl;
    }

    wl_buffer *createOrObtainBuffer(wl_shm *shm, int width, int height,
                                    int stride, int fd, int size) {
        // Create a shared memory pool
        wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
        if (!pool) {
            std::cerr << "Failed to create shared memory pool." << std::endl;
            return nullptr;
        }

        // Create a buffer from the shared memory pool
        wl_buffer *buffer = wl_shm_pool_create_buffer(
            pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
        if (!buffer) {
            std::cerr << "Failed to create buffer." << std::endl;
            wl_shm_pool_destroy(pool);
            return nullptr;
        }

        // You can attach listeners to handle buffer events if needed

        return buffer;
    }

    static void savePngImage(const std::string &filename, const uint8_t *buffer,
                             uint32_t width, uint32_t height, uint32_t stride) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            std::cerr << "Error: Failed to open file for writing." << std::endl;
            return;
        }

        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  nullptr, nullptr, nullptr);
        if (!png) {
            std::cerr << "Error: Failed to create PNG write struct."
                      << std::endl;
            fclose(fp);
            return;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            std::cerr << "Error: Failed to create PNG info struct."
                      << std::endl;
            png_destroy_write_struct(&png, nullptr);
            fclose(fp);
            return;
        }

        png_init_io(png, fp);

        png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);

        std::vector<png_bytep> row_pointers(height);
        for (uint32_t y = 0; y < height; ++y) {
            row_pointers[y] = const_cast<png_bytep>(&buffer[y * stride]);
        }

        png_set_rows(png, info, row_pointers.data());

        png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);

        png_destroy_write_struct(&png, &info);
        fclose(fp);

        std::cout << "PNG image saved as " << filename << std::endl;
    }

    static void frameFailed(void *data, zwlr_screencopy_frame_v1 *frame) {
        std::cerr << "Failed to capture frame." << std::endl;
    }

    wl_display *display;
    wl_registry *registry;
    wl_shm *shm;
    zwlr_screencopy_manager_v1 *screencopy_manager;
    wl_output *output;
    std::unordered_map<uint32_t, std::string> globalObjects;

    static const zwlr_screencopy_frame_v1_listener frameListener;
};

const zwlr_screencopy_frame_v1_listener WaylandConnection::frameListener = {
    frameBuffer,
};

int main() {
    WaylandConnection connection;
    connection.init();

    connection.print();

    wl_output *output = connection.getOutput();
    if (output) {
        std::cout << "wl_output pointer is set." << std::endl;
        connection.setupListeners();
        connection.captureScreen();
    } else {
        std::cerr << "Failed to get wl_output pointer." << std::endl;
    }

    connection.cleanup();
    return 0;
}

`;


export { QuicClient, QuicServer, Wayland }
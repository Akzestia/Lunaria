#include <iostream>
#include <cstring>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <png.h>
#include "wlr-screencopy-unstable-v1-client-protocol.h"

struct wl_display *display = NULL;
struct wl_registry *registry = NULL;
struct wl_shm *shm = NULL;
struct wl_output *output = NULL;
struct zwlr_screencopy_manager_v1 *screencopy_manager = NULL;

struct ScreencopyFrameListener {
    struct zwlr_screencopy_frame_v1 *frame;
    int width, height, stride;
    enum wl_shm_format format;
    void *data;
    bool done;
};

static void registry_global(void *data, struct wl_registry *registry,
                            uint32_t name, const char *interface, uint32_t version) {
    if (strcmp(interface, wl_shm_interface.name) == 0) {
        shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_output_interface.name) == 0) {
        output = (struct wl_output*)wl_registry_bind(registry, name, &wl_output_interface, 1);
    } else if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
        screencopy_manager = (struct zwlr_screencopy_manager_v1*)wl_registry_bind(
            registry, name, &zwlr_screencopy_manager_v1_interface, 1);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
    // Handle removal of global objects
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

static int create_shm_file() {
    char filename[] = "/tmp/wayland-screenshot-XXXXXX";
    int fd = mkstemp(filename);
    if (fd < 0) {
        return -1;
    }
    unlink(filename);
    return fd;
}

static void write_png(const char *filename, int width, int height, void *data) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        std::cerr << "Failed to create PNG write struct" << std::endl;
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Failed to create PNG info struct" << std::endl;
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error writing PNG file" << std::endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    for (int y = 0; y < height; y++) {
        png_write_row(png, (png_bytep)data + y * width * 4);
    }

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

static void frame_handle_buffer(void *data, struct zwlr_screencopy_frame_v1 *frame,
                                uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
    ScreencopyFrameListener *listener = static_cast<ScreencopyFrameListener*>(data);
    listener->format = (wl_shm_format)format;
    listener->width = width;
    listener->height = height;
    listener->stride = stride;
}

static void frame_handle_linux_dmabuf(void *data, struct zwlr_screencopy_frame_v1 *frame,
                                      uint32_t format, uint32_t width, uint32_t height) {
    // Handle linux_dmabuf if needed
}

static void frame_handle_buffer_done(void *data, struct zwlr_screencopy_frame_v1 *frame) {
    // Handle buffer_done if needed
}

static void frame_handle_damage(void *data, struct zwlr_screencopy_frame_v1 *frame,
                                uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    // Handle damage if needed
}

static const struct zwlr_screencopy_frame_v1_listener frame_listener = {
    .buffer = frame_handle_buffer,
    .linux_dmabuf = frame_handle_linux_dmabuf,
    .buffer_done = frame_handle_buffer_done,
    .flags = frame_handle_flags,
    .ready = frame_handle_ready,
    .failed = frame_handle_failed,
    .damage = frame_handle_damage,
};

int main() {
    display = wl_display_connect(NULL);
    if (!display) {
        std::cerr << "Failed to connect to Wayland display" << std::endl;
        return 1;
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!screencopy_manager) {
        std::cerr << "Screencopy protocol not supported" << std::endl;
        return 1;
    }

    ScreencopyFrameListener listener = {};
    listener.frame = zwlr_screencopy_manager_v1_capture_output(screencopy_manager, 0, output);
    zwlr_screencopy_frame_v1_add_listener(listener.frame, &frame_listener, &listener);

    while (!listener.done) {
        wl_display_dispatch(display);
    }

    if (!listener.data) {
        std::cerr << "Failed to capture frame" << std::endl;
        return 1;
    }

    int size = listener.stride * listener.height;

    write_png("screenshot.png", listener.width, listener.height, listener.data);

    munmap(listener.data, size);

    zwlr_screencopy_frame_v1_destroy(listener.frame);
    zwlr_screencopy_manager_v1_destroy(screencopy_manager);
    wl_output_destroy(output);
    wl_shm_destroy(shm);
    wl_registry_destroy(registry);
    wl_display_disconnect(display);

    std::cout << "Screenshot saved as screenshot.png" << std::endl;

    return 0;
}

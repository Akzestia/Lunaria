#include "ScreenCapture.h"
bool ScreenCapture::isCapture = false;

void ScreenCapture::Start() {
    Display *display;
    Window root;
    int width = 0;
    int height = 0;
    XWindowAttributes gwa;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cout << "No display can be aquired" << std::endl;
    }
    root = DefaultRootWindow(display);

    XGetWindowAttributes(display, root, &gwa);
    width = gwa.width;
    height = gwa.height;
    std::cout << "Width: " << width << "; Height: " << height << std::endl;
    XImage *image = XGetImage(display, root, 0, 0, width, height, ZPixmap, ZPixmap);

    XDestroyImage(image);
    XCloseDisplay(display);
}

void ScreenCapture::Stop() { isCapture = false; }
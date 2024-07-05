#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <chrono>

class ScreenCapture {
    static bool isCapture;
    
public:
    static void Start();
    static void Stop();
};
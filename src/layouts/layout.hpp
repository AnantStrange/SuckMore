#pragma once

#include <xcb/xproto.h>
class Layout{
    public:
        virtual void arrange() = 0;
        virtual void mapWindow(xcb_window_t map_window) = 0;
        virtual void delWindow(xcb_window_t win)  = 0;
        virtual void setFocus(xcb_window_t win)  = 0;
        virtual void remFocus(xcb_window_t win)  = 0;
};


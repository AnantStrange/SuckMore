#pragma once 

#include <cstdint>
#include <optional>
#include <utility>
#include <xcb/xproto.h>

std::optional<xcb_window_t>getCurrFocusWin();
std::pair<uint16_t, uint16_t>getScreenDim();

struct WinDimension {
    uint32_t x, y, width, height;
};

// util.hpp or wm.hpp
bool configureWindow(xcb_window_t win, uint16_t mask, uint32_t* values);
bool configureWindow(xcb_window_t win, uint8_t x, uint8_t y, uint8_t height, uint8_t width);
bool configureWindow(xcb_window_t win, WinDimension win_dim);
bool setSelBorder(xcb_window_t win);
bool setNormBorder(xcb_window_t win);

#include "util.hpp"
#include "suckmore.hpp"
#include <cstdint>
#include <cstdio>
#include <xcb/xproto.h>

std::optional<xcb_window_t>getCurrFocusWin(){
    WindowManager &WM = WindowManager::get();
    xcb_get_input_focus_cookie_t focus_cookie = xcb_get_input_focus(WM.conn);
    xcb_get_input_focus_reply_t* focus_reply = xcb_get_input_focus_reply(WM.conn, focus_cookie,nullptr);

    if(!focus_reply){
        return std::nullopt;
    }
    if(focus_reply->focus == XCB_INPUT_FOCUS_POINTER_ROOT){
        free(focus_reply);
        return std::nullopt;
    }

    xcb_window_t focused = focus_reply->focus;
    free(focus_reply);
    return focused;
}

std::pair<uint16_t, uint16_t> getScreenDim() {
    WindowManager &WM = WindowManager::get();
    
    // Get screen from connection setup, NOT from root window geometry
    const xcb_setup_t* setup = xcb_get_setup(WM.conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;
    
    uint16_t width = screen->width_in_pixels;
    uint16_t height = screen->height_in_pixels;
    
    // printf("Screen dimensions: %dx%d\n", width, height);
    
    return {height, width};
}

bool configureWindow(xcb_window_t win, uint16_t mask, uint32_t* values){
    WindowManager& WM = WindowManager::get();
    xcb_configure_window(WM.conn, win, mask, values);
    xcb_void_cookie_t cookie = xcb_configure_window(WM.conn, win, mask, values);
    xcb_generic_error_t* err = xcb_request_check(WM.conn, cookie);
    if (err) {
        printf("xcb_configure_window ERROR: code=%d\n", err->error_code);
        free(err);
        return false;
    }
    return true;
};

bool configureWindow(xcb_window_t win, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    WindowManager& WM = WindowManager::get();
    
    uint32_t values[] = { x, y, width, height };
    uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                    XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    
    xcb_configure_window(WM.conn, win, mask, values);
    xcb_void_cookie_t cookie = xcb_configure_window(WM.conn, win, mask, values);
    xcb_generic_error_t* err = xcb_request_check(WM.conn, cookie);
    if (err) {
        printf("xcb_configure_window ERROR: code=%d\n", err->error_code);
        free(err);
        return false;
    }
    return true;
}

bool configureWindow(xcb_window_t win, WinDimension win_dim) {
    printf("configureWindow: win=%u, x=%d, y=%d, width=%d, height=%d\n", 
           win, win_dim.x, win_dim.y, win_dim.width, win_dim.height);
    WindowManager& WM = WindowManager::get();
    
    uint32_t values[] = {
        win_dim.x,
        win_dim.y,
        win_dim.width,
        win_dim.height,
    };
    
    uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                    XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;

    xcb_void_cookie_t cookie = xcb_configure_window(WM.conn, win, mask, values);
    xcb_generic_error_t* err = xcb_request_check(WM.conn, cookie);
    if (err) {
        printf("xcb_configure_window ERROR: code=%d\n", err->error_code);
        free(err);
        return false;
    }
    
    return true;;
}

bool setSelBorder(xcb_window_t win){
    WindowManager &WM = WindowManager::get();
    uint32_t sel_bord_clr = WM.sel_bord_clr;
    uint16_t borderc_mask = XCB_CW_BORDER_PIXEL;
    xcb_void_cookie_t borderc_cookie = xcb_change_window_attributes(WM.conn, win, borderc_mask, &sel_bord_clr);
    xcb_generic_error_t* borderc_err = xcb_request_check(WM.conn, borderc_cookie);
    if (borderc_err) {
        printf("error setting border color for new map_request window ERROR: code=%d\n", borderc_err->error_code);
        free(borderc_err);
        return false;
    }
    free(borderc_err);
    return true;
}

bool setNormBorder(xcb_window_t win){
    WindowManager &WM = WindowManager::get();
    uint32_t norm_bord_clr = WM.norm_bord_clr;
    uint16_t borderc_mask = XCB_CW_BORDER_PIXEL;
    xcb_void_cookie_t borderc_cookie = xcb_change_window_attributes(WM.conn, win, borderc_mask, &norm_bord_clr);
    xcb_generic_error_t* borderc_err = xcb_request_check(WM.conn, borderc_cookie);
    if (borderc_err) {
        printf("error setting border color for new map_request window ERROR: code=%d\n", borderc_err->error_code);
        free(borderc_err);
        return false;
    }
    free(borderc_err);
    return true;

}



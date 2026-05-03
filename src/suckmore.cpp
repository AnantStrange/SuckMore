#include "suckmore.hpp"
#include "util.hpp"
#include <cstddef>
#include <cstdint>
#include <xcb/xcb.h>
#include <iostream>
#include <xcb/xproto.h>
#include <inttypes.h>
#include <xcb/randr.h>
#include <StrangeLib/StrangeLib.hpp>

const uint32_t FULL_EVENT_MASK = 
    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
    XCB_EVENT_MASK_PROPERTY_CHANGE |
    XCB_EVENT_MASK_KEY_PRESS |
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_ENTER_WINDOW |
    XCB_EVENT_MASK_STRUCTURE_NOTIFY;

class WindowManager {
    private:
        WindowManager() = default;
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;
    public:
        xcb_connection_t *conn = nullptr;
        xcb_window_t root = 0;
        static WindowManager& get(){
            static WindowManager instance;
            return instance;
        }
};

void handle_map_event(const xcb_generic_event_t *ev){
    xcb_map_request_event_t *event = (xcb_map_request_event_t*)ev;
    WindowManager &WM = WindowManager::get();
    xcb_window_t window = event->window;
    xcb_map_window(WM.conn, window);
}

void handle_key_event(const xcb_generic_event_t *ev){
    std::cout<<ev->response_type;
}


int main(){

    int screen_num = 0;
    xcb_connection_t *conn = xcb_connect(NULL, &screen_num);
    if (!conn || xcb_connection_has_error(conn)) {
        std::cout<<"Cannot connect to X server\n";
        return 1;
    }

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    xcb_window_t root = screen->root;
    WindowManager &WM = WindowManager::get();
    WM.conn = conn;
    WM.root = root;
    
    xcb_change_window_attributes(conn, root, XCB_CW_EVENT_MASK, &SUB_EVENT_MASK);
    xcb_generic_event_t *ev;

    while (true) {
        ev = xcb_wait_for_event(conn);  // Blocks until something happens
        if (!ev) {
            std::cout<<"NULL received for event, probably proken connection to X?\n";
            break;
        }

        switch (ev->response_type & ~0x80) {
            case XCB_MAP_REQUEST:
                handle_map_event(ev);
                break;
            case XCB_DESTROY_NOTIFY:
                handle_key_event(ev);
                break;
            case XCB_CONFIGURE_REQUEST:
                handle_key_event(ev);
                break;
            case XCB_KEY_PRESS:
                handle_key_event(ev);
                break;
            case XCB_BUTTON_PRESS:
                handle_key_event(ev);
                break;
            case XCB_ENTER_NOTIFY:
                handle_key_event(ev);
                break;
            case XCB_PROPERTY_NOTIFY:
                handle_key_event(ev);
                break;
            case XCB_CLIENT_MESSAGE:
                handle_key_event(ev);
                break;
            case XCB_UNMAP_NOTIFY:
                handle_key_event(ev);
                break;
            case XCB_CREATE_NOTIFY:
                handle_key_event(ev);
                break;
            case XCB_FOCUS_IN:
            case XCB_FOCUS_OUT:
                handle_key_event(ev);
                break;
            case XCB_MAPPING_NOTIFY:
                handle_key_event(ev);
                break;
        }

        free(ev);
    }



    return 0;
}



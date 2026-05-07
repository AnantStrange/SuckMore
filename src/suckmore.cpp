#include "suckmore.hpp"
#include "layouts/layout.hpp"
#include <X11/X.h>
#include <cstddef>
#include <cstdio>
#include <xcb/xcb.h>
#include <iostream>
#include <xcb/xproto.h>
#include <inttypes.h>
#include <xcb/randr.h>


Workspace::Workspace(){
    curr_layout = &mstack_layout;
    for(int i=0;i<TAGS_N-1;i++){
        layouts[i] = &mstack_layout;
    }
}

Layout* Workspace::getCurrentLayout(){
    // std::cout<<"Tagset : "<<tagset.to_string()<<std::endl;
    if( tagset.count() == 1 ){
        for (int i = 0; i < 10; i++) {
            if (tagset.test(i)) {
                return layouts[i];
            }
        }
    }
    return curr_layout;
}

Workspace WindowManager::getCurrentWorkSpace(){
    return workspaces[workspace_n];
};

Layout* WindowManager::getCurrentLayout(){
    Workspace wksp = workspaces[workspace_n];
    return wksp.getCurrentLayout();
};

void Workspace::setTagBit(uint8_t nbit){
    tagset.set(nbit);
    if(tagset.count() > 1){
        // mix_curr_layout;
    }
};
void Workspace::remTagBit(uint8_t nbit){
    tagset.reset(nbit);
    if(tagset.none()){
        tagset.set(nbit);
    }
};

bool handle_layout_mapping(xcb_window_t map_window){
    WindowManager &WM = WindowManager::get();

    Layout* layout = WM.getCurrentLayout();
    // std::cout<<"calling layout->mapWindow"<<std::endl;
    layout->mapWindow(map_window);
    return true;
}


void handle_map_event(const xcb_generic_event_t *ev){
    xcb_map_request_event_t *event = (xcb_map_request_event_t*)ev;
    printf("handle_map_event(): Window: %u is being mapped\n", event->window);

    WindowManager &WM = WindowManager::get();
    xcb_window_t map_window = event->window;

    xcb_generic_error_t *err;
    xcb_get_window_attributes_cookie_t attr_cookie = xcb_get_window_attributes(WM.conn, map_window);
    xcb_get_window_attributes_reply_t *attr_reply = xcb_get_window_attributes_reply(WM.conn, attr_cookie, &err);
    if(err){
        printf("While handle_map_event, enountered :\n");
        printf("Err.code = %d, Err.Resp_type = %d\n",err->error_code,err->response_type);
        return;
    }

    switch(attr_reply->map_state){
        case XCB_MAP_STATE_UNMAPPED:
            handle_layout_mapping(map_window);
            break;
        case XCB_MAP_STATE_UNVIEWABLE:
            std::cout<<"CASE XCB_MAP_STATE_UNVIEWABLE"<<std::endl;
            break;
        case XCB_MAP_STATE_VIEWABLE:
            std::cout<<"CASE XCB_MAP_STATE_VIEWABLE"<<std::endl;
            break;
    }

    free(attr_reply);
}

void handle_key_event(const xcb_generic_event_t *ev){
    std::cout<<ev->response_type;
}

void print_root_attr(){
    WindowManager &WM = WindowManager::get();
    xcb_get_window_attributes_cookie_t attr_cookie = xcb_get_window_attributes(WM.conn, WM.root);
    xcb_get_window_attributes_reply_t* attr = xcb_get_window_attributes_reply(WM.conn, attr_cookie, NULL);
    if (attr) {
        printf("Root event mask : 0x%x\n", attr->your_event_mask);
        free(attr);
    }
}

void handle_create_event(xcb_generic_event_t* ev){};
void handle_configure_request_event(xcb_generic_event_t* ev){};
void handle_configure_notify_event(xcb_generic_event_t* ev){};
void handle_map_notify_event(xcb_generic_event_t* ev){};
void handle_unmap_event(xcb_generic_event_t* ev){};
void handle_destroy_event(xcb_generic_event_t* ev){
    xcb_destroy_notify_event_t* event = (xcb_destroy_notify_event_t*)ev;
    WindowManager &WM = WindowManager::get();
    Layout* layout = WM.getCurrentLayout();
    layout->delWindow(event->window);
    free(event);
};

void handle_key_event(xcb_generic_event_t* ev){};
void handle_button_event(xcb_generic_event_t* ev){};
void handle_enter_event(xcb_generic_event_t* ev){
    xcb_enter_notify_event_t* event = (xcb_enter_notify_event_t*) ev;
    Layout* layout = WindowManager::get().getCurrentLayout();
    layout->setFocus(event->event);
};
void handle_leave_event(xcb_generic_event_t* ev){
    xcb_leave_notify_event_t* event = (xcb_leave_notify_event_t*) ev;
    Layout* layout = WindowManager::get().getCurrentLayout();
    layout->remFocus(event->event);
};
void handle_property_event(xcb_generic_event_t* ev){};
void handle_client_message_event(xcb_generic_event_t* ev){};
void handle_mapping_notify_event(xcb_generic_event_t* ev){};
void handle_client_event(xcb_generic_event_t* ev){};
void handle_map_notif_event(xcb_generic_event_t* ev){};
void handle_configure_event(xcb_generic_event_t* ev){};
void handle_prop_notif_event(xcb_generic_event_t* ev){};

int main(){
    std::cout << "=== SUCKMORE STARTING ===" << std::endl;

    int screen_num = 0;
    xcb_connection_t *conn = xcb_connect(NULL, &screen_num);
    if (!conn || xcb_connection_has_error(conn)) {
        std::cout<<"Cannot connect to X server"<<std::endl;
        return 1;
    }

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    xcb_window_t root = screen->root;
    WindowManager &WM = WindowManager::get();
    WM.conn = conn;
    WM.root = root;
    
    xcb_change_window_attributes(conn, root, XCB_CW_EVENT_MASK, &FULL_EVENT_MASK);
    xcb_flush(conn);

    xcb_generic_event_t *ev;

    while (true) {
        // std::cout<<"Waiting for event"<<std::endl;
        ev = xcb_wait_for_event(conn);  // Blocks until something happens
        // fflush(stdout);
        if (!ev) {
            std::cout<<"NULL received for event, probably proken connection to X?"<<std::endl;
            break;
        }
        // Print event names, not just numbers
        uint8_t type = ev->response_type & ~0x80;
        const char* event_name = "UNKNOWN";
        switch(type) {
            case XCB_MAP_REQUEST:   event_name = "MAP_REQUEST"; break;
            case XCB_DESTROY_NOTIFY: event_name = "DESTROY_NOTIFY";break;
            case XCB_CONFIGURE_REQUEST: event_name = "CONFIGURE_REQUEST"; break;
            case XCB_KEY_PRESS:     event_name = "KEY_PRESS"; break;
            case XCB_BUTTON_PRESS:  event_name = "BUTTON_PRESS"; break;
            case XCB_ENTER_NOTIFY:  event_name = "ENTER_NOTIFY"; break;
            case XCB_PROPERTY_NOTIFY: event_name = "PROP_NOTIF"; break;
            case XCB_CLIENT_MESSAGE: event_name = "CLIENT_MESSAGE"; break;
            case XCB_UNMAP_NOTIFY: event_name = "UNMAP_NOTIF"; break;
            case XCB_CREATE_NOTIFY: event_name = "CREATE_NOTIFY"; break;
            case XCB_MAPPING_NOTIFY: event_name = "MAPPING_NOTIF"; break;
            case XCB_MAP_NOTIFY:    event_name = "MAP_NOTIFY"; break;
            case XCB_CONFIGURE_NOTIFY:  event_name = "CONFIGURE_NOTIFY"; break;
            case XCB_LEAVE_NOTIFY:  event_name = "LEAVE_WINDOW"; break;
        }
        printf("Event: %s (%d)\n", event_name, type);

        switch (ev->response_type & ~0x80) {
            case XCB_CREATE_NOTIFY:
                handle_create_event(ev);
                break;
            case XCB_MAP_REQUEST:
                handle_map_event(ev);
                break;
            case XCB_UNMAP_NOTIFY:
                handle_unmap_event(ev);
                break;
            case XCB_DESTROY_NOTIFY:
                handle_destroy_event(ev);
                break;
            case XCB_CONFIGURE_REQUEST:
                handle_configure_event(ev);
                break;
            case XCB_KEY_PRESS:
                handle_key_event(ev);
                break;
            case XCB_BUTTON_PRESS:
                handle_button_event(ev);
                break;
            case XCB_ENTER_NOTIFY:
                handle_enter_event(ev);
                break;
            case XCB_LEAVE_NOTIFY:
                handle_leave_event(ev);
                break;
            case XCB_PROPERTY_NOTIFY:
                handle_prop_notif_event(ev);
                break;
            case XCB_CLIENT_MESSAGE:
                handle_client_event(ev);
                break;
            case XCB_MAPPING_NOTIFY:
                handle_map_notif_event(ev);
                break;
        }
        if (xcb_connection_has_error(conn)) {
            printf("ERROR: X connection has error! Connection broken.\n");
            break;
        }
    }
    return 0;
}


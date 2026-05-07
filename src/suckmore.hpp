#pragma once

#include "layouts/layout.hpp"
#include "layouts/MStack.hpp"
#include <bitset>
#include <cstdint>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <inttypes.h>
#include <xcb/randr.h>

const uint32_t FULL_EVENT_MASK = 
    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
    XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
    XCB_EVENT_MASK_STRUCTURE_NOTIFY | 
    XCB_EVENT_MASK_PROPERTY_CHANGE |
    XCB_EVENT_MASK_KEY_PRESS |
    XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_ENTER_WINDOW |
    XCB_EVENT_MASK_LEAVE_WINDOW ;

constexpr uint8_t WORKSPACE_N = 5;
constexpr uint8_t TAGS_N = 10;
constexpr uint8_t LAYOUT_N = 1;

class Workspace{
    private:
        MStack mstack_layout;
    public:
        std::bitset<TAGS_N> tagset=1;
        Layout* layouts[TAGS_N-1];
        Layout* curr_layout;
        Workspace();
        Layout* getCurrentLayout();
        void setTagBit(uint8_t nbit);
        void remTagBit(uint8_t nbit);
};

class WindowManager {
    private:
        WindowManager() = default;
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;
    public:
        xcb_connection_t *conn = nullptr;
        xcb_window_t root = 0;
        uint8_t workspace_n=0;
        Workspace workspaces[5];
        uint8_t border_width = 3;
        // uint32_t sel_bord_clr  = 0x00FFFFFF;  // White (bright, visible)
        uint32_t sel_bord_clr  = 0xFF0000;  // RED
        // uint32_t norm_bord_clr = 0x00444444;  // Dark gray (subtle, but visible)
        uint32_t norm_bord_clr = 0x0000FF;  // blue
        static WindowManager &get() {
          static WindowManager instance;
          return instance;
        };
        Workspace getCurrentWorkSpace();
        Layout* getCurrentLayout();
};


void handle_create_event(xcb_create_notify_event_t* ev);
void handle_map_event(const xcb_generic_event_t *ev);
void handle_configure_request_event(xcb_configure_request_event_t* ev);
void handle_configure_notify_event(xcb_configure_notify_event_t* ev);
void handle_map_notify_event(xcb_map_notify_event_t* ev);
void handle_unmap_event(xcb_unmap_notify_event_t* ev);
void handle_destroy_event(xcb_destroy_notify_event_t* ev);
void handle_key_event(xcb_key_press_event_t* ev);
void handle_button_event(xcb_button_press_event_t* ev);
void handle_enter_event(xcb_enter_notify_event_t* ev);
void handle_leave_event(xcb_leave_notify_event_t* ev);
void handle_focus_in_event(xcb_focus_in_event_t* ev);
void handle_focus_out_event(xcb_focus_out_event_t* ev);
void handle_property_event(xcb_property_notify_event_t* ev);
void handle_client_message_event(xcb_client_message_event_t* ev);
void handle_mapping_notify_event(xcb_mapping_notify_event_t* ev);

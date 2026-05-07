#pragma once

#include "layout.hpp"
#include "../ds/CirDLinkedList.hpp"
#include <cstdint>
#include <vector>
#include <xcb/xproto.h>

class MStack : public Layout {
    private:
        CirDLinkedList stack;
        uint8_t n_master = 1;
        float master_ratio = 0.6f;
        float height_delta = 0.06f;
        float width_delta = 0.02f;
    public:
    void mapWindow(xcb_window_t win) override;
    void arrange() override;
    void delWindow(xcb_window_t win) override;
    void setFocus(xcb_window_t win) override;
    void remFocus(xcb_window_t win) override;
    void arrangeArea(const std::vector<Node*>& nodes, int x, int width, int screen_height);
    void incMaster();
    void decMaster();
    void pushUP();
    void pushDOWN();
    void incWinHeight(xcb_window_t win);
    void decWinHeight(xcb_window_t win);
    void fixRatio();
    void fixRatioArea(std::vector<Node*>& nodes);
};



#include "MStack.hpp"
#include "suckmore.hpp"
#include "util.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

void MStack::mapWindow(xcb_window_t win) {
    WindowManager &WM = WindowManager::get();
    uint32_t border_width = WM.border_width, norm_bord_clr = WM.norm_bord_clr;
    uint16_t borderw_mask = XCB_CONFIG_WINDOW_BORDER_WIDTH;
    uint16_t borderc_mask = XCB_CW_BORDER_PIXEL;
    std::cout<<"MStack::mapWindow is called"<<std::endl;
    stack.push(win);
    xcb_void_cookie_t borderw_cookie = xcb_configure_window(WM.conn, win, borderw_mask, &border_width);
    xcb_void_cookie_t borderc_cookie = xcb_change_window_attributes(WM.conn, win, borderc_mask, &norm_bord_clr);
    xcb_generic_error_t* borderw_err = xcb_request_check(WM.conn, borderw_cookie);
    if (borderw_err) {
        printf("error setting border width for new map_request window ERROR: code=%d\n", borderw_err->error_code);
    }
    xcb_generic_error_t* borderc_err = xcb_request_check(WM.conn, borderc_cookie);
    if (borderc_err) {
        printf("error setting border color for new map_request window ERROR: code=%d\n", borderc_err->error_code);
    }
    free(borderw_err);
    free(borderc_err);
    std::cout<<"after mapWindow called, stack.size = "<<(int)stack.size()<<std::endl;
    stack.print();
    arrange();
    // Set client window event mask
    uint32_t client_mask = 
        XCB_EVENT_MASK_ENTER_WINDOW |      // For mouse enter
        XCB_EVENT_MASK_LEAVE_WINDOW |      // For mouse leave
        XCB_EVENT_MASK_FOCUS_CHANGE |      // For focus events
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_BUTTON_PRESS;
    
    xcb_change_window_attributes(WM.conn, win, XCB_CW_EVENT_MASK, &client_mask);
    xcb_map_window(WM.conn, win);
    xcb_flush(WM.conn);
}

void MStack::arrange() {
    std::cout<<"MStack::arrange is called"<<std::endl;
    WindowManager &WM = WindowManager::get();
    auto [screen_height, screen_width] = getScreenDim();
    if(stack.empty()){
        std::cout<<"MStack::arrange Stack is Empty !"<<std::endl;
        return;
    }
    else if(stack.size() == 1){
        WinDimension win_dim = {
            .x = 0,
            .y = 0,
            .width = static_cast<uint32_t>(screen_width - (2*WM.border_width)),
            .height = static_cast<uint32_t>(screen_height - (2*WM.border_width)),
        };
        auto node_opt = stack[0];
        auto win = node_opt.value()->win;
        configureWindow(win,win_dim);
        xcb_flush(WM.conn);
        return;
    }
    fixRatio();

    int total = stack.size();
    int master_count = std::min((int)n_master, total);
    
    std::vector<Node*> master_nodes;
    std::vector<Node*> slave_nodes;
    
    for (int i = 0; i < total; i++) {
        auto node_opt = stack[i];
        if (!node_opt.has_value()) break;
        if (i < master_count) {
            master_nodes.push_back(node_opt.value());
        } else {
            slave_nodes.push_back(node_opt.value());
        }
    }
    // std::cout<<"Got Master and Slave Nodes"<<std::endl;

    // Calculate widths
    int master_width = screen_width * master_ratio;
    int slave_width = screen_width - master_width;

    // std::cout<<"Running arrangeArea() on Masters"<<std::endl;
    arrangeArea(master_nodes, 0, master_width, screen_height);
    // std::cout<<"Running arrangeArea() on Slaves"<<std::endl;
    arrangeArea(slave_nodes, master_width, slave_width, screen_height);
    // std::cout<<"End arrangeArea() on Slaves"<<std::endl;

    xcb_flush(WM.conn);
    std::cout<<"end of MStack::arrange"<<std::endl;
}

void MStack::arrangeArea(const std::vector<Node*>& nodes, int x, int width, int screen_height) {
    WindowManager &WM = WindowManager::get();
    int current_y = 0;
    for (Node* node : nodes) {
        std::cout<<"arrangeArea: screen_height :"<<screen_height<<"node->height_ratio: "<<node->height_ratio<<std::endl;
        int height = screen_height * node->height_ratio;
        WinDimension dim = {
            .x = static_cast<uint32_t>(x),
            .y = static_cast<uint32_t>(current_y),
            .width = static_cast<uint32_t>(width - 2*WM.border_width),
            .height = static_cast<uint32_t>(height - 2*WM.border_width),
        };
        configureWindow(node->win, dim);
        
        current_y += height;
    }
}

void MStack::incMaster(){
    n_master++;
    arrange();
};

void MStack::decMaster(){
    if(n_master != 1){
        n_master--;
        arrange();
    }
};

void MStack::pushUP(){
    auto focused = getCurrFocusWin();
    if (!focused.has_value()) {
        return;
    }
    stack.pushUP(focused.value());
    arrange();
};

void MStack::pushDOWN(){
    auto focused = getCurrFocusWin();
    if (!focused.has_value()) {
        return;
    }
    stack.pushDOWN(focused.value());
    arrange();
};

void MStack::incWinHeight(xcb_window_t win){
    auto node_opt = stack.getNode(win);
    if(!node_opt.has_value()) {
        return;
    }

    Node* node = node_opt.value();
    int index = stack.getIndex(node->win);

    // Must have next window to take from
    if(index == stack.size() - 1) return;

    Node* next_node = stack[index + 1].value();

    if(next_node->height_ratio - height_delta < 0.02f) return;

    node->height_ratio += height_delta;
    next_node->height_ratio -= height_delta;
    arrange();
}

void MStack::decWinHeight(xcb_window_t win){
    auto node_opt = stack.getNode(win);
    if(!node_opt.has_value()) {
        return;
    }

    Node* node = node_opt.value();
    int index = stack.getIndex(node->win);

    // Must have next window to give to
    if(index == stack.size() - 1) return;

    Node* next_node = stack[index + 1].value();

    if(node->height_ratio - height_delta < 0.02f) return;

    node->height_ratio -= height_delta;
    next_node->height_ratio += height_delta;
    arrange();
}

void MStack::fixRatio() {
    printf("fixRatio: stack.size=%d, n_master=%d\n", stack.size(), n_master);
    if (stack.empty()) {
        return;
    }
    
    int total = stack.size();
    int master_count = std::min((int)n_master, total);
    
    std::vector<Node*> masters;
    std::vector<Node*> slaves;
    
    for (int i = 0; i < total; i++) {
        auto node_opt = stack[i];
        if (!node_opt.has_value()) break;
        if (i < master_count) {
            masters.push_back(node_opt.value());
        } else {
            slaves.push_back(node_opt.value());
        }
    }

    printf("  masters: %zu, slaves: %zu\n", masters.size(), slaves.size());
    // Print masters
    for (size_t i = 0; i < masters.size(); i++) {
        printf("master[%zu]: win=%u, ratio=%.2f\n", i, masters[i]->win, masters[i]->height_ratio); }
    // Print slaves
    for (size_t i = 0; i < slaves.size(); i++) {
        printf("slave[%zu]: win=%u, ratio=%.2f\n", i, slaves[i]->win, slaves[i]->height_ratio);
    }
    
    fixRatioArea(masters);
    fixRatioArea(slaves);
}

void MStack::fixRatioArea(std::vector<Node*>& nodes) {
    if (nodes.empty()) {
        return;
    };

    printf("fixRatioArea: %zu nodes, first_win=%u\n", nodes.size(), nodes[0]->win);

    float total_ratio = 0.0f;
    int custom_count = 0;

    for (Node* node : nodes) {
        printf("node %u: curr_ratio=%.2f\n", node->win, node->height_ratio);
        if (node->height_ratio > 0.0f) {
            total_ratio += node->height_ratio;
            custom_count++;
        }
    }

    printf("total_ratio=%.2f, custom_ratio_win_count=%d\n", total_ratio, custom_count);

    if (total_ratio > 1.0f + 0.001f) {
        printf("case: total_ratio > 1, resetting evenly\n");
        float even = 1.0f / nodes.size();
        for (Node* node : nodes) {
            node->height_ratio = even;
        }
        return;
    }

    if (std::abs(total_ratio - 1.0f) < 0.001f) {
        printf("case: total_ratio == 1, nothing to fix\n");
        return;
    }

    float rem_ratio = 1.0f - total_ratio;
    int auto_ratio_win_count = nodes.size() - custom_count;
    printf("rem_ratio=%.2f, auto_ratio_win_count=%d\n", rem_ratio, auto_ratio_win_count);

    if (auto_ratio_win_count > 0 && rem_ratio > 0.0f) {
        float each = rem_ratio / auto_ratio_win_count;
        printf("distributing: each=%.2f to %d windows\n", each, auto_ratio_win_count);
        for (Node* node : nodes) {
            if (node->height_ratio <= 0.0f) {
                node->height_ratio = each;
            }
        }
    } else if (custom_count == 0) {
        float even = 1.0f / nodes.size();
        printf("no custom ratios, setting all to %.2f\n", even);
        for (Node* node : nodes) {
            node->height_ratio = even;
        }
    } else if (auto_ratio_win_count == 0 && std::abs(total_ratio - 1.0f) > 0.001f) {
        float scale = 1.0f / total_ratio;
        printf("scaling custom ratios by %.2f\n", scale);
        for (Node* node : nodes) {
            node->height_ratio *= scale;
        }
    }

    // Final verification
    float final_total = 0;
    for (Node* node : nodes) {
        final_total += node->height_ratio;
    }
    printf("final total_ratio=%.2f\n", final_total);
}

void MStack::delWindow(xcb_window_t win){
    WindowManager &WM = WindowManager::get();
    stack.pop(win);
    arrange();
    xcb_flush(WM.conn);
}

void MStack::setFocus(xcb_window_t win){
    setSelBorder(win);
}

void MStack::remFocus(xcb_window_t win){
    setNormBorder(win);
}


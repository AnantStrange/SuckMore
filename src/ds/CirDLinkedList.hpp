#pragma  once

#include <bitset>
#include <cstdint>
#include <optional>
#include <xcb/xproto.h>

struct Node {
    xcb_window_t win;
    std::bitset<10> tagset;
    float height_ratio = 0.0f;
    bool floating = false;
    Node* next;
    Node* prev;
};

Node* createNode(xcb_window_t window);

class CirDLinkedList {
    private:
        Node* head = nullptr;
        uint8_t count = 0;
    public:
        CirDLinkedList();
        uint8_t size() const;
        bool empty() const;
        int getIndex(xcb_window_t);
        Node* getHead();
        std::optional<Node*> getNode(xcb_window_t);
        std::optional<Node*> operator[](int i);
        void push(xcb_window_t window);
        void pushBottom(xcb_window_t window);
        bool push(xcb_window_t window, int index);
        void popTop();
        void popBottom();
        bool pop(int index);
        bool pop(xcb_window_t window);
        void pushUP(xcb_window_t window);
        void pushDOWN(xcb_window_t window);
        void print(int index);
        void print();
};



#include "CirDLinkedList.hpp"
#include "suckmore.hpp"
#include <cstdio>
#include <iostream>
#include <optional>
#include <ostream>
#include <xcb/xproto.h>

Node* createNode(xcb_window_t window){
    std::bitset<TAGS_N> tagset;
    tagset.set(0);
    Node *node = new Node{window, tagset ,0.0f, false, nullptr,nullptr};
    return node;
};

CirDLinkedList::CirDLinkedList() : head(nullptr), count(0){};

uint8_t CirDLinkedList::size() const { return count; }
bool CirDLinkedList::empty() const { return count == 0; }

int CirDLinkedList::getIndex(xcb_window_t win) {
    if (!head) {
        return -1;
    }

    Node* current = head;
    for (int i = 0; i < count; i++) {
        if (current->win == win) return i;
        current = current->next;
    }
    return -1;
}

Node* CirDLinkedList::getHead(){
    return head;
}

std::optional<Node*>CirDLinkedList::getNode(xcb_window_t win){
    if (!head) {
        return std::nullopt;
    }

    Node* current = head;
    for (int i = 0; i < count; i++) {
        if (current->win == win) {
            break;
        };
        current = current->next;
    }
    return current;
};

std::optional<Node*> CirDLinkedList::operator[](int i){
    if(!head || i < 0 || i >= count) {
        return std::nullopt;
    }

    Node* ptr = head;
    for (int idx = 0; idx < i; idx++) {
        ptr = ptr->next;
    }

    return ptr;
}

void CirDLinkedList::push(xcb_window_t window) {
    std::cout<<"push called with Win :"<<window<<std::endl;
    Node* new_node = createNode(window);
    if(!head){
        head = new_node;
        head->next = head;
        head->prev = head;
        count++;
        return;
    }
    Node* tail = head->prev;
    new_node->next = head;
    new_node->prev = tail;
    head->prev = new_node;
    tail->next = new_node;
    head = new_node;
    count++;
};

void CirDLinkedList::pushBottom(xcb_window_t window) {
    if (!head) {
        // List is empty
        head = createNode(window);
        head->next = head;
        head->prev = head;
        count++;
        return;
    }
    
    Node* new_node = createNode(window);
    Node* tail = head->prev;
    
    new_node->next = head;
    new_node->prev = tail;
    tail->next = new_node;
    head->prev = new_node;
    count++;
}

bool CirDLinkedList::push(xcb_window_t window, int index) {
    if (!head && index != 0) {
        return false;
    }
    Node* ptr = head;
    index--;
    while(index-->0){
        if(!ptr){
            return false;
        }
        ptr = ptr->next;
    }
    Node* new_node = createNode(window);
    Node* next_node = ptr->next;
    ptr->next = new_node;
    new_node->next = next_node;
    new_node->prev =  ptr;
    next_node->prev = new_node;

    count++;
    return true;
}


void CirDLinkedList::popTop(){
    if(!head){
        return;
    }
    Node* to_del = head;
    head = head->next;
    count--;
    delete to_del;
    return;
};

void CirDLinkedList::popBottom(){
    if(!head || !(head->prev)){
        return;
    }
    Node* to_del = head->prev;
    head->prev = head->prev->prev;
    head->prev->next = head;
    delete to_del;
};

bool CirDLinkedList::pop(int index){
    if(!head){
        return true;
    }
    Node* ptr = head;
    while(index-->0){
        if(!ptr->next){
            return false;
        }
        ptr = ptr->next;
    }
    ptr->next->prev = ptr->prev;
    ptr->prev->next = ptr->next;
    delete ptr;
    count--;
    return true;
};

bool CirDLinkedList::pop(xcb_window_t window){
    if (!head) return false;

    // Find the node
    Node* ptr = head;
    for (int i = 0; i < count; i++) {
        if (ptr->win == window) break;
        ptr = ptr->next;
        if (ptr == head) return false;  // Wrapped around, not found
    }

    if (ptr->win != window) return false;

    // Case 1: Only one node
    if (count == 1) {
        delete ptr;
        head = nullptr;
        count = 0;
        return true;
    }

    // Case 2: Multiple nodes
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;

    if (ptr == head) {
        head = ptr->next;
    }

    delete ptr;
    count--;
    return true;
}


void CirDLinkedList::pushUP(xcb_window_t window){
    int index = getIndex(window);
    if (index <= 0) {
        return; 
    }

    pop(window);
    push(window, index - 1);
};

void CirDLinkedList::pushDOWN(xcb_window_t window){
    int index = getIndex(window);
    if (index == -1 || index >= count - 1){
        return;
    }

    pop(window);
    push(window, index + 1);
};

void CirDLinkedList::print(int index) {
    if (!head) {
        printf("List is empty\n");
        return;
    }
    
    if (index < 0 || index >= count) {
        printf("Index %d out of range (0-%d)\n", index, count - 1);
        return;
    }
    
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    
    // Get actual window geometry
    WindowManager& wm = WindowManager::get();
    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(wm.conn, current->win);
    xcb_get_geometry_reply_t* geo = xcb_get_geometry_reply(wm.conn, cookie, NULL);
    
    printf("Node[%d]: win=%u, ratio=%.2f", 
           index, current->win, current->height_ratio);
    
    if (geo) {
        printf(", geometry: x=%d, y=%d, w=%d, h=%d\n", 
               geo->x, geo->y, geo->width, geo->height);
        free(geo);
    } else {
        printf("\n");
    }
}

void CirDLinkedList::print() {
    if (!head) {
        printf("CirDLinkedList: empty\n");
        return;
    }
    
    WindowManager& wm = WindowManager::get();
    Node* current = head;
    
    printf("CirDLinkedList: count=%d\n", count);
    
    for (int i = 0; i < count; i++) {
        // Get window geometry
        xcb_get_geometry_cookie_t cookie = xcb_get_geometry(wm.conn, current->win);
        xcb_get_geometry_reply_t* geo = xcb_get_geometry_reply(wm.conn, cookie, NULL);
        
        if (geo) {
            printf("  [%d]: win=%u, ratio=%.2f, x=%d, y=%d, w=%d, h=%d\n", 
                   i, current->win, current->height_ratio,
                   geo->x, geo->y, geo->width, geo->height);
            free(geo);
        } else {
            printf("  [%d]: win=%u, ratio=%.2f, geometry: ERROR\n", 
                   i, current->win, current->height_ratio);
        }
        
        current = current->next;
    }
}


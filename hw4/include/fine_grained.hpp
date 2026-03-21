#pragma once
#include <mutex>
#include "iset.hpp"

struct FineGrainedSet : ISet {
    struct Node {
        int value;
        Node *next;
        std::mutex m;

        void link(Node *next_node) {
            next_node->next = next;
            next = next_node;
        }

        void unlink() {
            next = next->next;
        }
    };

    FineGrainedSet() {
        head->link(tail);
    }

    bool contains(int value) final {
        auto *current = get_first_with_lock();

        while (current != tail && current->next->value <= value) {
            current = get_next_with_lock(current);
        }

        if (current == tail) {
            current->m.unlock();
            return false;
        }

        bool result = value == current->value;
        current->m.unlock();
        return result;
    }

    void add(int value) final {
        auto *current = get_first_with_lock();

        while (current != tail && current->next->value <= value) {
            current = get_next_with_lock(current);
        }

        if (current == tail || value == current->value) {
            current->m.unlock();
            return;
        }

        current->link(new Node(value));
        current->m.unlock();
    }

    void remove(int value) final {
        head->m.lock();
        head->next->m.lock();
        auto *previous = head;
        auto *current = head->next;

        while (current != tail && current->next->value <= value) {
            auto *next = current->next;
            next->m.lock();
            previous->m.unlock();
            previous = current;
            current = next;
        }

        if (current == tail || value != current->value) {
            previous->m.unlock();
            current->m.unlock();
            return;
        }

        previous->unlink();
        previous->m.unlock();
        current->m.unlock();
        // delete current;
    }

    std::string get_name() final {
        return "FineGrainedSet";
    }

private:
    Node *head = new Node(HEAD_VALUE);
    Node *tail = new Node(TAIL_VALUE);

    static Node *get_next_with_lock(Node *current) {
        auto *next = current->next;
        next->m.lock();
        current->m.unlock();
        return next;
    }

    Node *get_first_with_lock() {
        head->m.lock();
        return get_next_with_lock(head);
    }
};

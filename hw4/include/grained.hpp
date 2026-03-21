#pragma once
#include <mutex>
#include "iset.hpp"

struct GrainedSet : ISet {
    struct Node {
        int value;
        Node *next;

        void link(Node *next_node) {
            next_node->next = next;
            next = next_node;
        }

        void unlink() {
            next = next->next;
        }
    };

    GrainedSet() {
        head->link(tail);
    }

    bool contains(int value) final {
        std::unique_lock l{m};
        auto *current = head->next;

        while (current != tail && current->next->value <= value) {
            current = current->next;
        }

        return value == current->value;
    }

    void add(int value) final {
        std::unique_lock l{m};
        auto *current = head->next;

        while (current != tail && current->next->value <= value) {
            current = current->next;
        }

        if (value == current->value) {
            return;
        }
        current->link(new Node(value));
    }

    void remove(int value) final {
        std::unique_lock l{m};
        auto *previous = head;
        auto *current = head->next;

        while (current != tail && current->next->value <= value) {
            previous = current;
            current = current->next;
        }

        if (value != current->value) {
            return;
        }

        previous->unlink();
        // delete current;
    }

    std::string get_name() final {
        return "GrainedSet";
    }

private:
    std::mutex m;
    Node *head = new Node(HEAD_VALUE);
    Node *tail = new Node(TAIL_VALUE);
};

#pragma once
#include <atomic>
#include <mutex>
#include "iset.hpp"

struct LazySet : ISet {
    struct Node {
        int value;
        std::atomic<Node *> next;
        std::atomic_bool marked{false};
        std::mutex m;

        void link(Node *next_node) {
            next_node->next.store(next);
            next.store(next_node);
        }

        void unlink() {
            next.store(next.load()->next.load());
        }
    };

    LazySet() {
        head->link(tail);
    }

    bool contains(int value) final {
        auto *node = find(value);
        if (node == head || value != node->value) {
            return false;
        }

        std::unique_lock l{node->m};
        return validate(node);
    }

    void add(int value) final {
        auto *node = find(value);
        if (value == node->value) {
            return;
        }

        std::unique_lock l{node->m};
        if (validate(node) && value < node->next.load()->value) {
            node->link(new Node(value));
        } else {
            add(value);
        }
    }

    void remove(int value) final {
        auto *previous = find(value - 1);
        std::unique_lock l{previous->m};

        auto *current = previous->next.load();
        std::unique_lock l_cur{current->m};

        if (value != current->value) {
            return;
        }

        if (validate(previous) && current == previous->next.load()) {
            previous->unlink();
            // delete current;
        } else {
            remove(value);
        }
    }

    std::string get_name() final {
        return "LazySet";
    }

private:
    Node *head = new Node(HEAD_VALUE);
    Node *tail = new Node(TAIL_VALUE);

    Node *find(int value) {
        auto *current = head;
        auto *next = current->next.load();
        while (next != tail && next->value <= value) {
            current = next;
            next = current->next.load();
        }
        return current;
    }

    bool validate(Node *node) {
        return !node->marked.load();
    }
};

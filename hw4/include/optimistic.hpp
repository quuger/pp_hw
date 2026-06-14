#pragma once
#include <mutex>
#include "iset.hpp"

struct OptimisticSet : ISet {
    struct Node {
        int value;
        std::atomic<Node *> next;
        std::mutex m;

        void link(Node *next_node) {
            next_node->next.store(next);
            next.store(next_node);
        }

        void unlink() {
            next.store(next.load()->next.load());
        }
    };

    OptimisticSet() {
        head->link(tail);
    }

    bool contains(int value) final {
        auto [node, _] = find(value);
        if (node == head || value != node->value) {
            return false;
        }

        std::unique_lock l{node->m};
        return validate(value, node, node->next.load());
    }

    void add(int value) final {
        while (true) {
            auto [prev, next] = find(value);
            if (value == prev->value) {
                return;
            }

            std::unique_lock l{prev->m};
            if (validate(value, prev, next)) {
                prev->link(new Node(value));
                return;
            }
        }
    }

    void remove(int value) final {
        while (true) {
            auto [previous, current] = find(value - 1);

            std::unique_lock l_prev{previous->m};
            std::unique_lock l_cur{current->m};

            if (value != current->value) {
                return;
            }

            if (validate(value - 1, previous, current)) {
                previous->unlink();
                return;
            }
        }
    }

    std::string get_name() final {
        return "OptimisticSet";
    }

private:
    Node *head = new Node(HEAD_VALUE);
    Node *tail = new Node(TAIL_VALUE);

    std::pair<Node *, Node *> find(int value) {
        auto *current = head;
        auto *next = current->next.load();
        while (next != tail && next->value <= value) {
            current = next;
            next = current->next.load();
        }
        return {current, next};
    }

    bool validate(int value, Node *expected_prev, Node *expected_next) {
        if (expected_prev->next.load() != expected_next) {
            return false;
        }

        auto [prev, next] = find(value);
        return prev == expected_prev;
    }
};

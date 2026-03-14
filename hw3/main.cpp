#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

std::mutex mtx;
std::condition_variable h_printable;
std::condition_variable o_printable;

unsigned int h_count = 0;
unsigned int o_count = 0;

void reset_unsafe() {
    if (h_count == 2 && o_count == 1) {
        h_count = 0;
        o_count = 0;
        std::cout << ' ';
    }
}

bool may_print_h() {
    return h_count < 2;
}

bool may_print_o() {
    return o_count < 1;
}

void hydrogen() {
    std::unique_lock l{mtx};
    h_printable.wait(l, [&]() { return may_print_h(); });

    std::cout << "H";
    h_count++;

    reset_unsafe();
    if (may_print_o()) {
        o_printable.notify_one();
    }
}

void oxygen() {
    std::unique_lock l{mtx};
    o_printable.wait(l, [&]() { return may_print_o(); });

    std::cout << "O";
    o_count++;

    reset_unsafe();
    if (may_print_h()) {
        h_printable.notify_one();
    }
}

int main() {
    constexpr int N = 1000;

    unsigned int o_threads = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1);

    std::vector<std::thread> ts;
    ts.reserve(N);

    for (int i = 0; i < 3 * N; ++i) {
        ts.emplace_back([&] {
            if (dis(gen) || o_threads == N) {
                hydrogen();
            } else {
                ++o_threads;
                oxygen();
            }
        });
    }

    for (auto &t : ts) {
        t.join();
    }

    std::cout << std::endl;
}
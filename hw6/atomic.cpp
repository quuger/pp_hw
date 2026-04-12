#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

std::atomic<bool> ready;
int data;

void run_once() {
    ready.store(false, std::memory_order_relaxed);
    data = 0;

    std::thread t1([] {
        data = 42;
        ready.store(true, std::memory_order_relaxed);
    });

    std::thread t2([] {
        while (!ready.load(std::memory_order_relaxed)) {
        }
        assert(data == 42);
    });

    t1.join();
    t2.join();
}

int main() {
    for (int i = 0; i < 1'000'000; ++i) {
        run_once();
    }
    std::cout << "OK\n";
}
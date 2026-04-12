#include <cassert>
#include <iostream>
#include <thread>

bool ready;
int data;

void run_once() {
    ready = false;
    data = 0;

    std::thread t1([] {
        data = 42;
        ready = true;
    });

    std::thread t2([] {
        while (!ready) {
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
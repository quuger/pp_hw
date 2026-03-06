#include <concepts>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <thread>

struct Dancer {
    virtual void dance() const = 0;
    std::shared_ptr<Dancer> pair;

    void ensure_unique_pair() const {
        static std::set<Dancer *> all_dancers;

        if (all_dancers.find(pair.get()) != all_dancers.end()) {
            std::stringstream ss;
            ss << "duplicate pair at " << pair.get();
            throw std::runtime_error(ss.str());
        }
        all_dancers.insert(pair.get());
    }

    void log(const std::string &str) const {
#ifdef INFO
        std::cout << str;
#endif
    }

    virtual ~Dancer() = default;
};

template <typename T>
concept Dancer_ = std::derived_from<T, Dancer>;

struct Leader : Dancer {
    void dance() const final {
        ensure_unique_pair();

        std::stringstream ss;
        ss << "Leader dancing at " << this << ", with Follower at "
           << pair.get() << '\n';

        log(ss.str());
    };
};

struct Follower : Dancer {
    void dance() const final {
        ensure_unique_pair();

        std::stringstream ss;
        ss << "Follower dancing at " << this << ", with Leader at "
           << pair.get() << '\n';
        log(ss.str());
    };
};

struct DanceHall {
    std::queue<std::shared_ptr<Dancer>> queue;

    struct {
        std::set<std::shared_ptr<Dancer>> set;

        void insert(std::shared_ptr<Dancer> dancer) {
            set.insert(dancer);
            dancer->dance();
        }
    } dance_hall;

    std::mutex m;

    template <Dancer_ T>
    void add_dancer(T &&dancer) {
        auto dancer_ptr = std::make_shared<T>(std::move(dancer));

        std::unique_lock l{m};

        if (queue.empty()) {
            queue.push(std::move(dancer_ptr));
            return;
        }

        auto top = queue.front();
        if (dynamic_cast<T *>(top.get())) {
            queue.push(std::move(dancer_ptr));
            return;
        }

        queue.pop();
        make_dance_pair(std::move(dancer_ptr), top);
    }

    void make_dance_pair(
        std::shared_ptr<Dancer> dancer_1,
        std::shared_ptr<Dancer> dancer_2
    ) {
        dancer_1->pair = dancer_2;
        dancer_2->pair = dancer_1;

        dance_hall.insert(std::move(dancer_1));
        dance_hall.insert(std::move(dancer_2));
    }

    void log_unmatched_dancers() {
        int count = 0;
        std::unique_lock l{m};
        if (queue.empty()) {
            return;
        }

        auto first = queue.front();

        while (!queue.empty()) {
            ++count;
            auto dancer_ptr = queue.front();
            queue.pop();

            if (typeid(*dancer_ptr) != typeid(*first)) {
                throw std::runtime_error("two differente type after matching");
            }
        }

        std::cout << "Total dancers without pair = " << count << '\n';
    }
}

DanceHall;

template <Dancer_ T>
void worker(const int n) {
    for (int i = 0; i < n; ++i) {
        DanceHall.add_dancer(T());
    }
}

int main() {
    std::vector<std::thread> ts;
    ts.emplace_back(worker<Leader>, 1000);
    ts.emplace_back(worker<Follower>, 10000);
    for (auto &t : ts) {
        t.join();
    }

    DanceHall.log_unmatched_dancers();
    std::cout << "Total matched dancers = " << DanceHall.dance_hall.set.size()
              << '\n';
}

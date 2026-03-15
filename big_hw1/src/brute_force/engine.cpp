#include "brute_force/engine.hpp"
#include <cstdint>
#include "models/key.hpp"
#include "signal_handler.hpp"

namespace brute_force_node::brute_force {
namespace {
bool process_value(const Key &key, uint64_t value) noexcept {
    return key == value;  // TODO: calculate hash from value
}
}  // namespace

Engine::Engine(size_t threads) : pool_(threads), thread_count_(threads) {
}

std::optional<uint64_t> Engine::search(Key key, SearchRange range) {
    cancel_requested_.store(false);

    auto parts = range.split(thread_count_);

    std::vector<std::promise<void>> promises(parts.size());
    futures_.clear();

    for (auto &p : promises) {
        futures_.push_back(p.get_future());
    }

    for (size_t i = 0; i < parts.size(); ++i) {
        auto part = parts[i];

        pool_.enqueue([this, part, &promise = promises[i],
                       key](std::stop_token st) mutable {
            for (uint64_t v = part.begin(); v <= part.end(); ++v) {
                if (st.stop_requested() || cancel_requested_.load() ||
                    signal_handler::get().load()) {
                    break;
                }

                if (process_value(key, v)) {
                    bool expected = false;
                    if (found_.compare_exchange_strong(expected, true)) {
                        found_value_.store(v);
                    }
                    break;
                }
            }

            promise.set_value();
        });
    }

    for (auto &f : futures_) {
        f.wait();
    }

    if (found_.load()) {
        return found_value_.load();
    }

    return std::nullopt;
}

void Engine::cancel() noexcept {
    cancel_requested_.store(true);
}

}  // namespace brute_force_node::brute_force

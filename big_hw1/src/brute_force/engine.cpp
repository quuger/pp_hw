#include "brute_force/engine.hpp"
#include "brute_force/hasher.hpp"
#include "models/bigint.hpp"
#include "models/search_range.hpp"
#include "signal_handler.hpp"

namespace brute_force_node::brute_force {
namespace {
bool process_value(const std::string &key, const models::Bigint &value) {
    return key == get_hash(value.to_string());
}
}  // namespace

Engine::Engine(size_t threads) : pool_(threads), thread_count_(threads) {
}

std::optional<std::string>
Engine::search(const std::string &key, SearchRange range) {
    reset();

    auto parts = range.split(thread_count_);

    std::vector<std::promise<void>> promises(parts.size());
    futures_.clear();

    for (auto &p : promises) {
        futures_.push_back(p.get_future());
    }

    for (size_t i = 0; i < parts.size(); ++i) {
        auto part = parts[i];
        pool_.enqueue([this, part, &promise = promises[i],
                       &key](std::stop_token st) mutable {
            for (models::Bigint v = part.begin(); v <= part.end(); v += 1) {
                if (st.stop_requested() || cancel_requested_.load() ||
                    signal_handler::get().test()) {
                    break;
                }

                if (process_value(key, v)) {
                    bool expected = false;
                    if (found_.compare_exchange_strong(expected, true)) {
                        found_value_ = v;
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
        return found_value_.to_string();
    }

    return std::nullopt;
}

void Engine::cancel() noexcept {
    cancel_requested_.store(true);
}

void Engine::reset() {
    cancel_requested_.store(false);
    futures_.clear();
    found_.store(false);
    found_value_ = 0;
}

}  // namespace brute_force_node::brute_force

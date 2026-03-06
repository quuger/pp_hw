#include <immintrin.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <random>

constexpr std::size_t N = 1 << 20;

void add_naive(const float *a, const float *b, float *c, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void add_sse(const float *a, const float *b, float *c, std::size_t n) {
    std::size_t i = 0;

    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vc = _mm_add_ps(va, vb);
        _mm_storeu_ps(c + i, vc);
    }

    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void add_avx(const float *a, const float *b, float *c, std::size_t n) {
    std::size_t i = 0;

    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }

    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

template <typename Func>
double
measure(Func f, const float *a, const float *b, float *c, std::size_t n) {
    auto start = std::chrono::high_resolution_clock::now();
    f(a, b, c, n);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> diff = end - start;
    return diff.count();
}

int main() {
    float *a = new (std::align_val_t(32)) float[N];
    float *b = new (std::align_val_t(32)) float[N];
    float *c = new (std::align_val_t(32)) float[N];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(1.0f, 10.0f);

    for (std::size_t i = 0; i < N; ++i) {
        a[i] = dis(gen);
        b[i] = dis(gen);
    }

    auto naive_time = measure(add_naive, a, b, c, N);
    auto sse_time = measure(add_sse, a, b, c, N);
    auto avx_time = measure(add_avx, a, b, c, N);

    std::cout << "Naive: " << naive_time << " ms\n";
    std::cout << "SSE  : " << sse_time << " ms\t(меньше Naive в "
              << sse_time / naive_time << " раз)\n";
    std::cout << "AVX  : " << avx_time << " ms\t(меньше Naive в "
              << avx_time / naive_time << " раз)\n";

    operator delete[](a, std::align_val_t(32));
    operator delete[](b, std::align_val_t(32));
    operator delete[](c, std::align_val_t(32));

    return 0;
}

#include "lakakey_engine.h"
#include <chrono>
#include <iomanip>
#include <iostream>
int main() {
    constexpr int words=100000;
    lakakey::Engine engine;
    const std::u32string test=U"tieengs ";
    auto start=std::chrono::steady_clock::now();
    for(int w=0; w<words; ++w) for(char32_t cp:test) engine.feed(cp);
    auto end=std::chrono::steady_clock::now();
    double seconds=std::chrono::duration<double>(end-start).count();
    double us_per_key=seconds*1e6/(words*test.size());
    std::cout << std::fixed << std::setprecision(3)
              << "Events: " << words*test.size() << "\n"
              << "Elapsed seconds: " << seconds << "\n"
              << "Average microseconds / key: " << us_per_key << "\n"
              << "(Single local Linux x86_64 benchmark, not a p95 OS-level latency test)\n";
}

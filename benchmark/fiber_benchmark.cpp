#include "fiber.h"
#include <iostream>
#include <chrono>
#include <atomic>

void test_fiber_creation() {
    std::cout << "=== Test Fiber Creation Performance ===" << std::endl;
    
    const int count = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < count; i++) {
        wbfiber::Fiber::ptr fiber = std::make_shared<wbfiber::Fiber>([]() {
            // 空协程
        });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Created " << count << " fibers in " << duration << " ms" << std::endl;
    std::cout << "Average: " << (double)duration / count * 1000 << " us per fiber" << std::endl;
    std::cout << "=== Test Fiber Creation Pass ===" << std::endl;
}

void test_fiber_switch() {
    std::cout << "\n=== Test Fiber Switch Performance ===" << std::endl;
    
    const int count = 1000000;
    std::atomic<int> switch_count = 0;
    
    wbfiber::Fiber::ptr main_fiber = wbfiber::Fiber::GetThis();
    wbfiber::Fiber::ptr fiber = std::make_shared<wbfiber::Fiber>([&switch_count, main_fiber, count]() {
        while (switch_count < count) {
            switch_count++;
            wbfiber::Fiber::GetThis()->yield();
        }
    });
    
    auto start = std::chrono::high_resolution_clock::now();
    
    while (switch_count < count) {
        fiber->resume();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Switched " << count << " times in " << duration << " ms" << std::endl;
    std::cout << "Average: " << (double)duration / count * 1000 << " us per switch" << std::endl;
    std::cout << "=== Test Fiber Switch Pass ===" << std::endl;
}

void test_fiber_concurrent() {
    std::cout << "\n=== Test Fiber Concurrent Performance ===" << std::endl;
    
    const int fiber_count = 10000;
    const int task_per_fiber = 1000;
    std::atomic<int> total_tasks = 0;
    
    std::vector<wbfiber::Fiber::ptr> fibers;
    for (int i = 0; i < fiber_count; i++) {
        fibers.push_back(std::make_shared<wbfiber::Fiber>([&total_tasks, task_per_fiber]() {
            for (int j = 0; j < task_per_fiber; j++) {
                total_tasks++;
            }
        }));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (auto &fiber : fibers) {
        fiber->resume();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "Ran " << fiber_count << " fibers with " << task_per_fiber << " tasks each in " << duration << " ms" << std::endl;
    std::cout << "Total tasks: " << total_tasks << std::endl;
    std::cout << "Tasks per second: " << (double)total_tasks / duration * 1000 << std::endl;
    std::cout << "=== Test Fiber Concurrent Pass ===" << std::endl;
}

int main() {
    test_fiber_creation();
    test_fiber_switch();
    test_fiber_concurrent();
    return 0;
}
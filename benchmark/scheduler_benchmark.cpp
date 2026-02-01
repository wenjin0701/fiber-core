#include "scheduler.h"
#include <iostream>
#include <chrono>
#include <atomic>

void test_scheduler_throughput() {
    std::cout << "=== Test Scheduler Throughput ===" << std::endl;
    
    const int task_count = 1000000;
    const int thread_count = 4;
    std::atomic<int> completed_tasks = 0;
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(thread_count, false, "BenchmarkScheduler");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 添加任务
    for (int i = 0; i < task_count; i++) {
        scheduler->scheduler([&completed_tasks]() {
            completed_tasks++;
        });
    }
    
    // 启动调度器
    scheduler->start();
    
    // 等待所有任务完成
    while (completed_tasks < task_count) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // 停止调度器
    scheduler->stop();
    
    std::cout << "Completed " << task_count << " tasks with " << thread_count << " threads in " << duration << " ms" << std::endl;
    std::cout << "Throughput: " << (double)task_count / duration * 1000 << " tasks per second" << std::endl;
    std::cout << "=== Test Scheduler Throughput Pass ===" << std::endl;
}

void test_scheduler_concurrent_fibers() {
    std::cout << "\n=== Test Scheduler Concurrent Fibers ===" << std::endl;
    
    const int fiber_count = 100000;
    const int thread_count = 8;
    std::atomic<int> completed_fibers = 0;
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(thread_count, false, "FiberSchedulerBenchmark");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 添加协程任务
    for (int i = 0; i < fiber_count; i++) {
        scheduler->scheduler(std::make_shared<wbfiber::Fiber>([&completed_fibers]() {
            // 简单计算
            int sum = 0;
            for (int j = 0; j < 100; j++) {
                sum += j;
            }
            completed_fibers++;
        }));
    }
    
    // 启动调度器
    scheduler->start();
    
    // 等待所有协程完成
    while (completed_fibers < fiber_count) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // 停止调度器
    scheduler->stop();
    
    std::cout << "Completed " << fiber_count << " fibers with " << thread_count << " threads in " << duration << " ms" << std::endl;
    std::cout << "Throughput: " << (double)fiber_count / duration * 1000 << " fibers per second" << std::endl;
    std::cout << "=== Test Scheduler Concurrent Fibers Pass ===" << std::endl;
}

void test_scheduler_load_balancing() {
    std::cout << "\n=== Test Scheduler Load Balancing ===" << std::endl;
    
    const int task_count = 100000;
    const int thread_count = 4;
    std::vector<std::atomic<int>> thread_tasks(thread_count);
    
    for (int i = 0; i < thread_count; i++) {
        thread_tasks[i] = 0;
    }
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(thread_count, false, "LoadBalancingScheduler");
    
    // 添加任务
    for (int i = 0; i < task_count; i++) {
        scheduler->scheduler([&thread_tasks]() {
            int thread_id = wbfiber::Thread::GetThis()->getId() % thread_tasks.size();
            thread_tasks[thread_id]++;
        });
    }
    
    // 启动调度器
    scheduler->start();
    
    // 等待所有任务完成
    bool all_done = false;
    while (!all_done) {
        all_done = true;
        int total = 0;
        for (int i = 0; i < thread_count; i++) {
            total += thread_tasks[i];
            if (total < task_count) {
                all_done = false;
            }
        }
        if (!all_done) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    // 停止调度器
    scheduler->stop();
    
    std::cout << "Load balancing result:" << std::endl;
    for (int i = 0; i < thread_count; i++) {
        std::cout << "Thread " << i << ": " << thread_tasks[i] << " tasks" << std::endl;
    }
    
    // 计算负载均衡度
    int min_tasks = thread_tasks[0];
    int max_tasks = thread_tasks[0];
    for (int i = 1; i < thread_count; i++) {
        if (thread_tasks[i] < min_tasks) {
            min_tasks = thread_tasks[i];
        }
        if (thread_tasks[i] > max_tasks) {
            max_tasks = thread_tasks[i];
        }
    }
    
    double balance_ratio = (double)min_tasks / max_tasks;
    std::cout << "Load balance ratio: " << balance_ratio << std::endl;
    std::cout << "=== Test Scheduler Load Balancing Pass ===" << std::endl;
}

int main() {
    test_scheduler_throughput();
    test_scheduler_concurrent_fibers();
    test_scheduler_load_balancing();
    return 0;
}
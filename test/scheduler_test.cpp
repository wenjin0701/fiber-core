#include "scheduler.h"
#include <iostream>
#include <thread>
#include <atomic>

void test_scheduler_basic() {
    std::cout << "=== Test Scheduler Basic ===" << std::endl;
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(2, false, "TestScheduler");
    std::atomic<int> count = 0;
    
    // 添加任务
    for (int i = 0; i < 10; i++) {
        scheduler->scheduler([&count, i]() {
            std::cout << "Task " << i << " run in thread " << wbfiber::Thread::GetThis()->getId() << std::endl;
            count++;
        });
    }
    
    // 启动调度器
    scheduler->start();
    
    // 添加更多任务
    for (int i = 10; i < 20; i++) {
        scheduler->scheduler([&count, i]() {
            std::cout << "Task " << i << " run in thread " << wbfiber::Thread::GetThis()->getId() << std::endl;
            count++;
        });
    }
    
    // 停止调度器
    scheduler->stop();
    
    std::cout << "Total tasks run: " << count << std::endl;
    std::cout << "=== Test Scheduler Basic Pass ===" << std::endl;
}

void test_scheduler_fiber() {
    std::cout << "\n=== Test Scheduler Fiber ===" << std::endl;
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(3, false, "FiberScheduler");
    std::atomic<int> count = 0;
    
    // 添加协程任务
    for (int i = 0; i < 5; i++) {
        scheduler->scheduler(std::make_shared<wbfiber::Fiber>([&count, i]() {
            std::cout << "Fiber task " << i << " start, id: " << wbfiber::Fiber::GetCurFiberID() << std::endl;
            count++;
            // 让出执行权
            wbfiber::Fiber::GetThis()->yield();
            std::cout << "Fiber task " << i << " resume" << std::endl;
            count++;
            std::cout << "Fiber task " << i << " end" << std::endl;
        }));
    }
    
    scheduler->start();
    scheduler->stop();
    
    std::cout << "Total fiber tasks run: " << count << std::endl;
    std::cout << "=== Test Scheduler Fiber Pass ===" << std::endl;
}

void test_scheduler_thread_specify() {
    std::cout << "\n=== Test Scheduler Thread Specify ===" << std::endl;
    
    wbfiber::Scheduler::ptr scheduler = std::make_shared<wbfiber::Scheduler>(3, false, "ThreadSpecifyScheduler");
    std::atomic<int> count = 0;
    
    // 启动调度器
    scheduler->start();
    
    // 指定线程0执行任务
    scheduler->scheduler([]() {
        std::cout << "Task in thread 0: " << wbfiber::Thread::GetThis()->getId() << std::endl;
    }, 0);
    
    // 指定线程1执行任务
    scheduler->scheduler([]() {
        std::cout << "Task in thread 1: " << wbfiber::Thread::GetThis()->getId() << std::endl;
    }, 1);
    
    // 不指定线程
    scheduler->scheduler([]() {
        std::cout << "Task in any thread: " << wbfiber::Thread::GetThis()->getId() << std::endl;
    });
    
    scheduler->stop();
    
    std::cout << "=== Test Scheduler Thread Specify Pass ===" << std::endl;
}

int main() {
    test_scheduler_basic();
    test_scheduler_fiber();
    test_scheduler_thread_specify();
    return 0;
}
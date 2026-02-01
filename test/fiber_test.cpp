#include "fiber.h"
#include <iostream>
#include <thread>

void test_fiber_basic() {
    std::cout << "=== Test Fiber Basic ===" << std::endl;
    
    // 获取主线程协程
    wbfiber::Fiber::ptr main_fiber = wbfiber::Fiber::GetThis();
    std::cout << "Main fiber id: " << main_fiber->getId() << std::endl;
    
    int count = 0;
    // 创建子协程
    wbfiber::Fiber::ptr fiber = std::make_shared<wbfiber::Fiber>([&count, main_fiber]() {
        std::cout << "Fiber 1 start, id: " << wbfiber::Fiber::GetCurFiberID() << std::endl;
        count++;
        
        // 让出执行权
        wbfiber::Fiber::GetThis()->yield();
        std::cout << "Fiber 1 resume, id: " << wbfiber::Fiber::GetCurFiberID() << std::endl;
        count++;
        
        // 再次让出执行权
        wbfiber::Fiber::GetThis()->yield();
        std::cout << "Fiber 1 resume again, id: " << wbfiber::Fiber::GetCurFiberID() << std::endl;
        count++;
        
        std::cout << "Fiber 1 end" << std::endl;
    });
    
    std::cout << "Created fiber id: " << fiber->getId() << std::endl;
    
    // 切换到子协程
    fiber->resume();
    std::cout << "Main resume after first yield, count: " << count << std::endl;
    
    // 再次切换到子协程
    fiber->resume();
    std::cout << "Main resume after second yield, count: " << count << std::endl;
    
    // 再次切换到子协程
    fiber->resume();
    std::cout << "Main resume after fiber end, count: " << count << std::endl;
    
    // 测试协程总数
    std::cout << "Total fiber count: " << wbfiber::Fiber::TotalFiberNum() << std::endl;
    std::cout << "=== Test Fiber Basic Pass ===" << std::endl;
}

void test_fiber_reset() {
    std::cout << "\n=== Test Fiber Reset ===" << std::endl;
    
    int count = 0;
    wbfiber::Fiber::ptr fiber = std::make_shared<wbfiber::Fiber>([&count]() {
        std::cout << "Fiber reset test: count = " << count << std::endl;
        count++;
    });
    
    fiber->resume();
    std::cout << "After first run, count: " << count << std::endl;
    
    // 重置协程
    fiber->reset([&count]() {
        std::cout << "Fiber reset again: count = " << count << std::endl;
        count++;
    });
    
    fiber->resume();
    std::cout << "After reset run, count: " << count << std::endl;
    std::cout << "=== Test Fiber Reset Pass ===" << std::endl;
}

void test_fiber_thread() {
    std::cout << "\n=== Test Fiber Thread ===" << std::endl;
    
    std::thread t([]() {
        std::cout << "Thread start" << std::endl;
        // 在新线程中获取协程
        wbfiber::Fiber::ptr fiber = wbfiber::Fiber::GetThis();
        std::cout << "Thread main fiber id: " << fiber->getId() << std::endl;
        
        wbfiber::Fiber::ptr sub_fiber = std::make_shared<wbfiber::Fiber>([]() {
            std::cout << "Thread sub fiber id: " << wbfiber::Fiber::GetCurFiberID() << std::endl;
        });
        
        sub_fiber->resume();
        std::cout << "Thread end" << std::endl;
    });
    
    t.join();
    std::cout << "=== Test Fiber Thread Pass ===" << std::endl;
}

int main() {
    test_fiber_basic();
    test_fiber_reset();
    test_fiber_thread();
    return 0;
}
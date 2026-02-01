#include "iomanager.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void test_iomanager_basic() {
    std::cout << "=== Test IOManager Basic ===" << std::endl;
    
    wbfiber::IOManager::ptr iom = std::make_shared<wbfiber::IOManager>(2, false, "TestIOManager");
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }
    
    // 设置非阻塞
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // 尝试连接（非阻塞）
    int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        std::cerr << "Failed to connect" << std::endl;
        close(sockfd);
        return;
    }
    
    // 添加写事件
    iom->addEvent(sockfd, wbfiber::Event::WRITE, [sockfd, iom]() {
        std::cout << "Connect write event triggered" << std::endl;
        // 检查连接是否成功
        int error = 0;
        socklen_t len = sizeof(error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        if (error) {
            std::cerr << "Connect failed: " << strerror(error) << std::endl;
        } else {
            std::cout << "Connect success" << std::endl;
        }
        
        // 关闭socket
        close(sockfd);
        // 取消事件
        iom->delEvent(sockfd, wbfiber::Event::WRITE);
    });
    
    // 启动IO管理器
    iom->start();
    
    // 等待一段时间
    sleep(2);
    
    // 停止IO管理器
    iom->stop();
    
    std::cout << "=== Test IOManager Basic Pass ===" << std::endl;
}

void test_iomanager_timer() {
    std::cout << "\n=== Test IOManager Timer ===" << std::endl;
    
    wbfiber::IOManager::ptr iom = std::make_shared<wbfiber::IOManager>(1, false, "TimerIOManager");
    
    int count = 0;
    // 添加一次性定时器
    auto timer1 = iom->addTimer(1000, [&count]() {
        std::cout << "Timer 1 triggered, count: " << count << std::endl;
        count++;
    });
    
    // 添加循环定时器
    auto timer2 = iom->addTimer(500, [&count]() {
        std::cout << "Timer 2 triggered, count: " << count << std::endl;
        count++;
    }, true);
    
    // 启动IO管理器
    iom->start();
    
    // 等待一段时间
    sleep(5);
    
    // 取消循环定时器
    timer2->cancel();
    std::cout << "Timer 2 canceled" << std::endl;
    
    // 再等待一段时间
    sleep(2);
    
    // 停止IO管理器
    iom->stop();
    
    std::cout << "Total timer triggers: " << count << std::endl;
    std::cout << "=== Test IOManager Timer Pass ===" << std::endl;
}

void test_iomanager_event() {
    std::cout << "\n=== Test IOManager Event ===" << std::endl;
    
    wbfiber::IOManager::ptr iom = std::make_shared<wbfiber::IOManager>(2, false, "EventIOManager");
    
    // 创建管道
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        std::cerr << "Failed to create pipe" << std::endl;
        return;
    }
    
    // 添加读事件
    iom->addEvent(pipefd[0], wbfiber::Event::READ, [pipefd, iom]() {
        char buf[1024];
        int n = read(pipefd[0], buf, sizeof(buf));
        if (n > 0) {
            buf[n] = '\0';
            std::cout << "Read from pipe: " << buf << std::endl;
        }
        
        // 取消事件
        iom->delEvent(pipefd[0], wbfiber::Event::READ);
        close(pipefd[0]);
    });
    
    // 启动IO管理器
    iom->start();
    
    // 向管道写入数据
    write(pipefd[1], "Hello IOManager", strlen("Hello IOManager"));
    close(pipefd[1]);
    
    // 等待事件处理
    sleep(1);
    
    // 停止IO管理器
    iom->stop();
    
    std::cout << "=== Test IOManager Event Pass ===" << std::endl;
}

int main() {
    test_iomanager_basic();
    test_iomanager_timer();
    test_iomanager_event();
    return 0;
}
#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class Socket_Tcp
{
public:
    int sock;
    
    Socket_Tcp(const char* ip, const int port);
    int server_config(void);
    bool img_imencode(cv::Mat &img);
    int send_img(void);
    int send_int(int value);  // 新增：发送整数函数
    ~Socket_Tcp();

private:
    sockaddr_in server_addr;
    const char* server_ip;
    const int server_port;
    std::vector<uchar> img_buffer;
};

#endif
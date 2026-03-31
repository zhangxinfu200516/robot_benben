#include "Socket_Tcp.h"

Socket_Tcp::Socket_Tcp(const char* ip, const int port) : server_ip(ip), server_port(port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        std::cout << "Error: 创建Socket失败" << std::endl;
    }
}

int Socket_Tcp::server_config(void)
{
    int ret;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    ret = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if(ret <= 0) {
        std::cout << "Error: 无效的服务器地址" << std::endl;
        close(sock);
        ret = -EINVAL;
        return ret;
    }

    ret = connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0) {
        std::cerr << "Error: 连接服务器失败" << std::endl;
        close(sock);
        ret = -EINVAL;
        return ret;
    }

    std::cout << "成功连接到服务器: " << server_ip << ":" << server_port << std::endl;

    return 0;
}

bool Socket_Tcp::img_imencode(cv::Mat &img)
{
    bool successfully = cv::imencode(".jpg", img, img_buffer);
    if(!successfully) {
        std::cerr << "Error: 图像编码失败" << std::endl;
    }
    return successfully;
}

int Socket_Tcp::send_img(void)
{
    int ret;

    uint32_t img_size = htonl(img_buffer.size());
    ret = send(sock, &img_size, sizeof(img_size), 0);
    if (ret <= 0) {
        ret = -EINVAL;
        std::cerr << "Error: 发送数据长度失败" << std::endl;
        return ret;
    }

    int total_sent = 0;
    while (total_sent < img_buffer.size()) {
        int sent = send(sock, img_buffer.data() + total_sent,
                        img_buffer.size() - total_sent, 0);
        if (sent <= 0) {
            ret = -EINVAL;
            std::cerr << "Error: 数据传输中断" << std::endl;
            return ret;
        }
        total_sent += sent;
    }

    return 0;
}


int Socket_Tcp::send_int(int value)
{
    // 将整数转换为网络字节序
    uint32_t net_value = htonl(static_cast<uint32_t>(value));
    
    int ret = send(sock, &net_value, sizeof(net_value), 0);
    if (ret <= 0) {
        std::cerr << "Error: 发送整数失败" << std::endl;
        return -1;
    }
    return 0;
}

Socket_Tcp::~Socket_Tcp()
{
    if(sock != -1) {
        close(sock);
        std::cout << "socket连接已关闭" << std::endl;
    } else {
        std::cout << "socket连接未创建" << std::endl;
    }
}


//示例
// #include "Socket_Tcp.h"

// int main() {
//     Socket_Tcp client("192.168.1.100", 8080);
//     if (client.server_config() != 0) return -1;

//     // 发送测试数据
//     client.send_int(42);                     // 发送整数
//     client.send_string("Hello Server!");     // 发送字符串

//     // 发送图像
//     cv::Mat frame = cv::imread("test.jpg");
//     if (client.img_imencode(frame)) {
//         client.send_img();
//     }

//     return 0;
// }

// int main() {
//     // 1. 创建TCP套接字对象（服务器IP和端口）
//     Socket_Tcp sender("127.0.0.1", 8080);  // 替换为实际服务器IP和端口
    
//     // 2. 配置并连接到服务器
//     if (sender.server_config() != 0) {
//         std::cerr << "服务器连接失败，程序终止" << std::endl;
//         return -1;
//     }
    
//     // 3. 创建OpenCV摄像头捕获对象
//     cv::VideoCapture cap(0);  // 0表示默认摄像头
//     if (!cap.isOpened()) {
//         std::cerr << "无法打开摄像头" << std::endl;
//         return -1;
//     }
    
//     cv::Mat frame;
//     std::cout << "开始发送视频流 (按ESC键退出)..." << std::endl;
    
//     while (true) {
//         // 4. 从摄像头捕获帧
//         cap >> frame;
//         if (frame.empty()) break;
        
//         // 5. 调整图像大小（可选，减小传输数据量）
//         cv::resize(frame, frame, cv::Size(640, 480));
        
//         // 6. 编码图像为JPEG格式
//         if (!sender.img_imencode(frame)) {
//             std::cerr << "图像编码失败，跳过此帧" << std::endl;
//             continue;
//         }
        
//         // 7. 发送图像数据
//         if (sender.send_img() != 0) {
//             std::cerr << "图像发送失败，终止程序" << std::endl;
//             break;
//         }
        
//         // 8. 显示本地预览
//         cv::imshow("Sender Preview", frame);
        
//         // 9. 检查退出键
//         if (cv::waitKey(10) == 27) {  // ESC键
//             break;
//         }
//     }
    
//     std::cout << "视频流发送结束" << std::endl;
//     return 0;
// }
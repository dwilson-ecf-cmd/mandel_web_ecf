#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Простой генератор фрактала в формат BMP (чтобы браузер сразу его показал)
vector<uint8_t> generate_bmp(int width, int height) {
    int rowSize = (width * 3 + 3) & ~3;
    int dataSize = rowSize * height;
    int fileSize = 54 + dataSize;

    vector<uint8_t> img(fileSize, 0);

    // Заголовок BMP
    img[0] = 'B'; img[1] = 'M';
    img[2] = fileSize; img[3] = fileSize >> 8; img[4] = fileSize >> 16; img[5] = fileSize >> 24;
    img[10] = 54;
    img[14] = 40;
    img[18] = width; img[19] = width >> 8;
    img[22] = height; img[23] = height >> 8;
    img[26] = 1;
    img[28] = 24;

    uint8_t* pixels = img.data() + 54;

    for (int y = 0; y < height; y++) {
        double cy = (y - height / 2.0) / (height / 2.0);
        for (int x = 0; x < width; x++) {
            double cx = (x - width / 1.5) / (height / 2.0);
            double zx = 0, zy = 0;
            int iter = 0;
            while (zx*zx + zy*zy < 4.0 && iter < 100) {
                double tmp = zx*zx - zy*zy + cx;
                zy = 2.0*zx*zy + cy;
                zx = tmp;
                iter++;
            }

            int pIdx = y * rowSize + x * 3;
            if (iter == 100) {
                pixels[pIdx] = pixels[pIdx+1] = pixels[pIdx+2] = 0;
            } else {
                pixels[pIdx] = (iter * 5) % 256;       // B
                pixels[pIdx+1] = (iter * 11) % 256;    // G
                pixels[pIdx+2] = (iter * 23) % 256;    // R
            }
        }
    }
    return img;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    cout << "========================================\n";
    cout << "🚀 Сервер запущен!\n";
    cout << "📱 Открой браузер и перейди по адресу:\n";
    cout << "   http://localhost:8080\n";
    cout << "========================================\n";

    while (true) {
        int new_socket = accept(server_fd, NULL, NULL);
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        string req(buffer);

        if (req.find("GET /render") != string::npos) {
            vector<uint8_t> bmp = generate_bmp(600, 400);
            string header = "HTTP/1.1 200 OK\r\nContent-Type: image/bmp\r\nContent-Length: " + to_string(bmp.size()) + "\r\n\r\n";
            send(new_socket, header.c_str(), header.length(), 0);
            send(new_socket, (char*)bmp.data(), bmp.size(), 0);
        } else {
            ifstream file("index.html");
            stringstream ss; ss << file.rdbuf();
            string html = ss.str();
            string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(html.size()) + "\r\n\r\n";
            send(new_socket, header.c_str(), header.length(), 0);
            send(new_socket, html.c_str(), html.size(), 0);
        }
        close(new_socket);
    }
    return 0;
}

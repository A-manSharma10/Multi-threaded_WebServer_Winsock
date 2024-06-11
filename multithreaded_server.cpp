#include <iostream>
#include <string>
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Server {
public:
    void run() {
        int port = 8080;
        WSADATA wsaData;
        SOCKET server_fd = INVALID_SOCKET, new_socket = INVALID_SOCKET;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw runtime_error("WSAStartup failed");
        }

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            WSACleanup();
            throw runtime_error("Socket creation error");
        }

        // Forcefully attaching socket to the port 8010
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == SOCKET_ERROR) {
            closesocket(server_fd);
            WSACleanup();
            throw runtime_error("Setsockopt error");
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        // Forcefully attaching socket to the port 8010
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
            closesocket(server_fd);
            WSACleanup();
            throw runtime_error("Bind error");
        }

        if (listen(server_fd, 3) == SOCKET_ERROR) {
            closesocket(server_fd);
            WSACleanup();
            throw runtime_error("Listen error");
        }

        cout << "Server is listening on port: " << port << endl;

        while (true) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
                closesocket(server_fd);
                WSACleanup();
                throw runtime_error("Accept error");
            }

            cout << "Yes I will handle the request of the client " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << endl;

            const char *message = "Hello World from the server";
            send(new_socket, message, strlen(message), 0);

            closesocket(new_socket);
        }

        closesocket(server_fd);
        WSACleanup();
    }
};

int main() {
    Server server;
    try {
        server.run();
    } catch (const exception &ex) {
        cerr << ex.what() << endl;
    }
    return 0;
}
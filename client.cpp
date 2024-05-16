#include <iostream>
#include <string>
#include <WinSock2.h>
#include <thread> // Include the thread header

#pragma comment(lib, "ws2_32.lib")

class Client {
public:
    void operator()() 
    {
        int port = 8010;
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use inet_addr for IP conversion

        SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            return;
        }

        if (connect(client_socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
            std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            return;
        }

        std::string message = "Hello from Client";
        if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            return;
        }

        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR) {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
            closesocket(client_socket);
            return;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Response from Server: " << buffer << std::endl;

        closesocket(client_socket);
    }
};

int main() 
{
    // Initialize Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return -1;
    }

    Client client;
    for (int i = 0; i < 100; ++i) {
        std::thread t(client); // corrected from 'tread' to 'thread'
        t.detach(); // Detach the thread to avoid joining
    }

    // Cleanup Winsock
    WSACleanup();

    return 0;
}

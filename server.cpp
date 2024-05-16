#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector> // Include the vector header for managing threads

#pragma comment(lib, "ws2_32.lib")

class Server {
public:
    void operator()(SOCKET client_socket) {
        std::string message = "Hello from server";
        if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
        }
        closesocket(client_socket);
    }
};

int main() {
    // Initialize Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return -1;
    }

    int port = 8010;
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listening failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    std::vector<std::thread> threads; // Vector to store threads

    while (true) {
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return -1;
        }

        Server server;
        // Launch thread and store it in the vector
        threads.emplace_back(server, client_socket);
    }

    // Join all threads before exiting
    for (auto& t : threads) {
        t.join();
    }

    // Close the server socket
    closesocket(server_socket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}

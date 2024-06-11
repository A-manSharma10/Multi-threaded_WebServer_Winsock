#include <iostream>
#include <string>
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Client {
public:
    void run() {
        int port = 8010; // Change port to match the server
        string address = "127.0.0.1";
        WSADATA wsaData;
        SOCKET sock = INVALID_SOCKET;
        struct sockaddr_in serv_addr;

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw runtime_error("WSAStartup failed");
        }

        // Creating socket file descriptor
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            WSACleanup();
            throw runtime_error("Socket creation error");
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 address from text to binary form
        if (inet_addr(address.c_str()) == INADDR_NONE) {
            closesocket(sock);
            WSACleanup();
            throw runtime_error("Invalid address / Address not supported");
        }
        serv_addr.sin_addr.s_addr = inet_addr(address.c_str());

        // Connect to the server
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
            closesocket(sock);
            WSACleanup();
            throw runtime_error("Connection Failed");
        }

        // Send data to the server
        string message = "Hello World from socket " + to_string(sock);
        send(sock, message.c_str(), message.length(), 0);
        cout << "Message sent: " << message << endl;

        // Cleanup
        closesocket(sock);
        WSACleanup();
    }
};

int main() {
    Client client;
    try {
        client.run();
    } catch (const exception &ex) {
        cerr << ex.what() << endl;
    }
    return 0;
}
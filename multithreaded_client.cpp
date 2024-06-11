#include <iostream>
#include <string>
#include <stdexcept>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Client {
public:
    void run(int numClients) {
        for (int i = 0; i < numClients; ++i) {
            connectToServer();
        }
    }

private:
    void connectToServer() {
        int port = 8080; // Change port to match the server
        string address = "127.0.0.1";
        WSADATA wsaData;
        SOCKET sock = INVALID_SOCKET;
        struct sockaddr_in serv_addr;

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed" << endl;
            return;
        }

        // Creating socket file descriptor
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            cerr << "Socket creation error" << endl;
            WSACleanup();
            return;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 address from text to binary form
        if (inet_addr(address.c_str()) == INADDR_NONE) {
            cerr << "Invalid address / Address not supported" << endl;
            closesocket(sock);
            WSACleanup();
            return;
        }
        serv_addr.sin_addr.s_addr = inet_addr(address.c_str());

        // Connect to the server
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
            cerr << "Connection Failed" << endl;
            closesocket(sock);
            WSACleanup();
            return;
        }

        // Send data to the server
        string message = "Hi! from socket " + to_string(sock);
        send(sock, message.c_str(), message.length(), 0);
        cout << "Message sent: " << message << endl;

        // Cleanup
        closesocket(sock);
        WSACleanup();
    }
};

int main() {
    int numClients = 5; // Number of client connections
    Client client;
    try {
        client.run(numClients);
    } catch (const exception &ex) {
        cerr << ex.what() << endl;
    }
    return 0;
}
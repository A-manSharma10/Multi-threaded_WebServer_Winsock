# Multi-threaded and Single-threaded Client-Server Application

This project demonstrates a basic client-server architecture implemented in C++ using the Winsock library. The project includes both single-threaded and multi-threaded versions of the client and server applications.

## Project Structure

The project is structured into the following files:

- **Multithreaded Client**: `multithreaded_client.cpp`
- **Multithreaded Server**: `multithreaded_server.cpp`
- **Single-threaded Client**: `singlethreaded_client.cpp`
- **Single-threaded Server**: `singlethreaded_server.cpp`

## Requirements

To compile and run these programs, you will need the following:

- **C++ Compiler**: Ensure you have a C++ compiler installed (e.g., MinGW for Windows).
- **Winsock Library**: The Winsock library is required for network programming on Windows. It is usually included with your compiler, but ensure that it is properly linked during compilation.

## Compilation and Execution

### Compiling

You can compile the programs using a C++ compiler. Here are the compilation commands for each file:

**Multithreaded Client:**
```bash
g++ multithreaded_client.cpp -o multithreaded_client -lws2_32
```

**Multithreaded Server:**
```bash
g++ multithreaded_server.cpp -o multithreaded_server -lws2_32
```

**Single-threaded Client:**
```bash
g++ singlethreaded_client.cpp -o singlethreaded_client -lws2_32
```

**Single-threaded Server:**
```bash
g++ singlethreaded_server.cpp -o singlethreaded_server -lws2_32
```

### Running

To run the compiled programs, execute the following commands in separate terminal windows:

**Start the Single-threaded Server:**
```bash
./singlethreaded_server
```

**Start the Single-threaded Client:**
```bash
./singlethreaded_client
```

**Start the Multithreaded Server:**
```bash
./multithreaded_server
```

**Start the Multithreaded Client:**
```bash
./multithreaded_client
```

## Description

### Multithreaded Client and Server

- **Multithreaded Client**: The client creates multiple connections to the server by running multiple threads.
- **Multithreaded Server**: The server listens for incoming connections and handles each client request concurrently using threads.

### Single-threaded Client and Server

- **Single-threaded Client**: The client connects to the server and sends a simple message.
- **Single-threaded Server**: The server listens for incoming connections and handles each client request sequentially.

## Features

- **Multithreading**: Demonstrates handling multiple client connections simultaneously.
- **Networking**: Basic socket programming using the Winsock library.
- **Error Handling**: Basic error handling for network operations.

## Notes

- Ensure that the server is running before starting the client to avoid connection errors.
- Modify the port number in the client and server code if you encounter port conflicts.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any enhancements or bug fixes.

---

Feel free to explore the code and modify it as per your requirements. If you have any questions or issues, please open an issue in the repository.

Happy Coding!

## Contact

For any inquiries or support, please contact Aman Sharma at amn.10.sharma@gmail.com .



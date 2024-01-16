#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

#define PORT 8080
#define BUFFER_SIZE 1024

// Enhanced logging function
void logServerActivity(const std::string& message) {
    std::ofstream logFile("server_log.txt", std::ios::app);
    if (logFile.is_open()) {
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        logFile << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Failed to open log file." << std::endl;
    }
}



int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    //Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    logServerActivity("Server started.");

    // Main loop for handling client requests
    while (true) {
        int bytesRead = read(new_socket, buffer, BUFFER_SIZE);
        if (bytesRead <= 0) {
            break; // Exit if no data is received
        }

        // Handle synchronization request
        std::string request(buffer, bytesRead);
        if (request.rfind("update:", 0) == 0) { // Check if it's an update request
            std::string filename = request.substr(7); // Extract filename
            // Perform file update logic here
            logServerActivity("Received update request for: " + filename);
        }
    }
    logServerActivity("Server ended.");
    
    close(new_socket);
    close(server_fd);



}
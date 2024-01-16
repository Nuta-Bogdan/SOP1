
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include <chrono>
#include <iomanip>
#include <sstream>


#define PORT 8080
#define BUFFER_SIZE 1024


// Logging function
void logMessage(const std::string& message) {
    std::ofstream logFile("client_log.txt", std::ios::app);
    if (logFile.is_open()) {
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        logFile << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
        logFile.close();
    }
}

// Function to request and receive a file from the server
void downloadFile(int sock, const std::string& filename) {
    std::string request = "download:" + filename;
    send(sock, request.c_str(), request.size(), 0);
    // Further code to receive the file and save it locally
    logMessage("Requested download for: " + filename);
}

// Function to check for file changes
std::map<std::string, time_t> checkForChanges(const std::string& directoryPath, std::map<std::string, time_t>& lastModified) {
    std::map<std::string, time_t> currentModified;
    DIR *dir;
    struct dirent *ent;
    struct stat fileInfo;

    if ((dir = opendir(directoryPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string filePath = directoryPath + "/" + ent->d_name;
            if (stat(filePath.c_str(), &fileInfo) == 0) {
                currentModified[filePath] = fileInfo.st_mtime;
                if (lastModified[filePath] != fileInfo.st_mtime) {
                    std::cout << "File changed: " << filePath << std::endl;
                    // Here you can add code to send this file to the server
                }
            }
        }
        closedir(dir);
    }
    return currentModified;
}
int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        return -1;
    }

    std::string directoryPath = "/path/to/your/directory"; // Set your directory path here
    std::map<std::string, time_t> lastModified;

    // Initial filling of the last modified times
    lastModified = checkForChanges(directoryPath, lastModified);

    // Main loop for monitoring changes
    while (true) {
        sleep(10); // Sleep for 10 seconds between checks
        std::map<std::string, time_t> currentModified = checkForChanges(directoryPath, lastModified);
        lastModified = currentModified; // Update the last modified times
    }
    while (true) {
      
        // downloadFile(sock, "example.txt");

        logMessage("Checking for latest updates...");
        sleep(10); // Sleep for 10 seconds between checks
    }

    close(sock);

    return 0;
}
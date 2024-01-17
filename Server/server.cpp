#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <ctime>
#include <sstream>

//Define constants
#define PORT 8080
#define BUFFER_SIZE 1024


void logActivity(const std::string& filename, bool isNewFile) {
    std::ofstream logFile("server_log.txt", std::ios::app);
    if (!logFile) 
    {
        std::cerr << "Failed to open\n" << std::endl;
        return;
    }

    time_t now = time(nullptr);
    char formattedTime[20]; 
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", localtime(&now));

   
    logFile << formattedTime << " - " << filename 
            << (isNewFile ? " added." : " updated.") << std::endl;
}


bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void sendFile(const std::string& filename, int socket) {

    std::ifstream file(filename, std::ios::binary);
    if (!file) 
    {
        std::cerr << "Error\n" << filename << std::endl;

        return;
    }
    char buffer[BUFFER_SIZE];

    while (file) 
    {
        file.read(buffer, sizeof(buffer));
        int bytesRead = file.gcount(); 
        if (bytesRead > 0) {
            send(socket, buffer, bytesRead, 0);
        }
    }


}

void removeFile(const std::string& filename) {
    if (std::remove(filename.c_str()) != 0) {
        perror("Error\n");
    } else {
        std::cout << "File successfully deleted\n" << std::endl;
    }
}

void sendLog(int socket) {
    std::ifstream logFile("server_log.txt");
    std::stringstream logContents;

    if (!logFile) 
    {
        std::cerr << "Error\n" << std::endl;
        return;
    }
    logContents << logFile.rdbuf();

    //send the log contents 
    std::string logStr = logContents.str();
    send(socket, logStr.c_str(), logStr.size(), 0);
}


int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");

        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //Setup server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    //Bind the socket to the address
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

    //1
    char filename[BUFFER_SIZE] = {0};
    read(new_socket, filename, BUFFER_SIZE);

    std::string filePath = std::string(filename); // File saved in the current working directory

    // Check if file exists
    bool isNewFile = !fileExists(filePath);

    std::ofstream outputFile(filePath, std::ios::binary);

    if (!outputFile.is_open()) {
        perror("Failed to open file for writing\n");
        exit(EXIT_FAILURE);
    }

    ssize_t read_bytes;
    while ((read_bytes = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        outputFile.write(buffer, read_bytes);
    }
    
    
    logActivity(filename, isNewFile);
    
    //2 Send File

    //char filename[BUFFER_SIZE] = {0};
    //read(client_socket, filename, BUFFER_SIZE);

    //sendFile(std::string(filename), client_socket);

    //3 Remove File

    //char filename[BUFFER_SIZE] = {0};
    //read(client_socket, filename, BUFFER_SIZE);

    //removeFile(std::string(filename));

    //4 Send Log

    //char filename[BUFFER_SIZE] = {0};
    //read(client_socket, filename, BUFFER_SIZE);

    //sendLog(client_socket);

    outputFile.close();
    close(new_socket);
    close(server_fd);



}
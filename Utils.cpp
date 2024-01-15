
#include <io.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;


bool isValidTime(const char *str) {
    if (str == nullptr || *str == '\0') {
        return false;
    }

    for (const char *current = str; *current != '\0'; ++current) {
        if (!std::isdigit(*current)) {
            return false;
        }
    }

    int number = std::stol(str);

    return (number > 0);

}

void addCronJob() {
    const char *commandToExecute = "@reboot /usr/bin/got checker";
    system(commandToExecute);

}

bool isValidFilePath(const char *filepath) {
    if (filepath == nullptr || *filepath == '\0') {
        return false;
    }
    struct stat file_stat;
    stat(filepath, &file_stat);
    if (S_ISDIR(file_stat.st_mode)) {
        printf("%s is a directory.\n", filepath);
        return false;
    }
    return true;
}

void sendFileToServer(const char *filePath, int bufferSize, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[bufferSize];

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "\n Socket creation error \n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "\nInvalid address/ Address not supported \n";
        close(sock);
        return;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "\nConnection Failed \n";
        close(sock);
        return;
    }

    // Open the file to be sent
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        perror("Failed to open file");
        close(sock);
        return;
    }

    // Read the file and send its contents to the server
    while (!inputFile.eof()) {
        inputFile.read(buffer, bufferSize);
        send(sock, buffer, inputFile.gcount(), 0);
    }

    inputFile.close();
    close(sock);
}

void sleepTillNextMinute() {
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    int seconds_until_next_minute = 60 - current_time->tm_sec;
    sleep(seconds_until_next_minute);
}
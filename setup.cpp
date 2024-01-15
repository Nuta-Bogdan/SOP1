#include <iostream>
#include <string>
#include <cstring>
#include "Utils.cpp"
#include <unistd.h>
#include "FileWriter.cpp"
#include <libgen.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
using namespace std;


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Too few arguments. Please specify a command\n";
        return 1;
    }

    if (argc == 4 && std::string(argv[1]) == "add") {
        if (!isValidFilePath(argv[2])) {
            std::cout << "Invalid file path\n";
            return 1;
        }
        if (!isValidTime(argv[3])) {
            std::cout << "Please enter a correct number of minutes\n";
            return 1;
        }

        const char *home_dir = getenv("HOME");
        const char *folder_name = "backups";

        if (access(folder_name, F_OK) != 0) {
            char command[256];
            snprintf(command, sizeof(command), "mkdir %s/%s", home_dir, folder_name);
            int result = system(command);
            if (result != 0) {
                std::cerr << "Failed to create directory\n";
                return 1;
            }
        }

        char *filename = basename(argv[2]);
        char *path = (char *) malloc(strlen(home_dir) + 1 + strlen(folder_name) + 1 + strlen(filename) + 1);
        snprintf(path, strlen(home_dir) + 1 + strlen(folder_name) + 1 + strlen(filename) + 1,
                 "%s/%s/%s", home_dir, folder_name, filename);

        if (symlink(argv[2], path) != 0) {
            std::cerr << "Failed to create symbolic link\n";
            free(path);
            return 1;
        }
        free(path);

        CsvFileManager csvFileManager;
        csvFileManager.writeRow(argv[2], argv[3], argv[3]);
        return 0;
    }

    if (argc == 3 && string(argv[1]) == "remove") {
        if (!isValidFilePath(argv[2])) {
            cout << "Invalid file path\n";
            return 1;
        }
        CsvFileManager csvFileManager;
        bool in = false;
        auto vect = csvFileManager.readAllRows();
        for (int i = 0; i < vect.size(); ++i) {
            if (vect[i][0] == argv[2]) {
                vect.erase(vect.begin() + i);
                in = true;
            }
        }
        if (in) {
            return 0;
        } else {
            cout << "Given file does not exist in job list\n";
            return 1;
        }

    }

    if (argc == 2 && string(argv[1]) == "list") {
        CsvFileManager csvFileManager;
        auto rows = csvFileManager.readAllRows();
        for (const auto &row: rows) {
            cout << "For file" << row[0] << "backup is made every " << row[1]
                 << " minutes, next backup will me made in " << row[2] << "minutes" << '\n';
        }
        return 0;
    }

    if (argc == 2 && string(argv[1]) == "checker") {
        if (open("HOME/backups/.info", O_RDONLY) == -1) {
            int fd = open("HOME/backups/.info", O_CREAT|O_WRONLY, 0666);
            unsigned long long int time = 0;
            write(fd, &time, sizeof(time));
            close(fd);
        }

        while (true)
        {
            int fd = open("HOME/backups/.info", O_RDWR);
            unsigned long long int time = 0;
            read(fd, &time, sizeof(time));
            close(fd);
            CsvFileManager csvFileManager;
            auto rows = csvFileManager.readAllRows();
            for (auto &row: rows) {
                if (stoi(row[2]) == 0) {
                    //sendFileToServer(row[0].c_str(), 1024, 8080);
                    row[2] = row[1];
                }
                else {
                    row[2] = to_string(stoi(row[2]) - 1);
                }
                time++;
                fd = open("HOME/backups/.info", O_RDWR);
                write(fd, &time, sizeof(time));
                close(fd);
                sleepTillNextMinute();
            }
        }
    }

    if (argc ==2 && string(argv[1]) == "init") {
        addCronJob();
    }
    if (argc == 3 && argv[2] == "get" && isValidFilePath(argv[1])) {
        //getLatestFromServer(argv[1], 1024, 8080);
    }

    if (argc == 4 && argv[2] == "get" && isValidFilePath(argv[1])) {
        int value = atoi(argv[3]);
        bool  ok = (value != 0);
        if (ok=false) {
            cout << "Invalid number\n";
            return 1;
        }
    //getVersionFromServer(argv[1], 1024, 8080, value);
    }

}



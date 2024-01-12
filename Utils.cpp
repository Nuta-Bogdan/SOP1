#include "Utils.h"
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

void addCronJob(const std::string &command, int intervalInMinutes) {
    std::string cronEntry = "*/" + std::to_string(intervalInMinutes) + " * * * * " + command;
    std::string installCommand = "echo \"$(crontab -l ; echo '" + cronEntry + "')\" | crontab -";
}

bool isValidFilePath(const char *filepath) {
    std::string bashCommand = "test -e \"" + std::string(filepath) + "\"";
    int result = std::system(bashCommand.c_str());
    return (result == 0);
}

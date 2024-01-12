//
// Created by nuta_ on 09/01/2024.
//

#ifndef SOP1_UTILS_H
#define SOP1_UTILS_H


class Utils {
public:
    static bool isValidTime(const char *str);

    static void addCronJob(const std::string &command, int intervalInMinutes);

    static bool isValidFilePath(const char *filepath);
};


#endif //SOP1_UTILS_H

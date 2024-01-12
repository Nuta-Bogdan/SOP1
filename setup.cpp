#include <iostream>
#include <string>
#include "FileWriter.h"
#include <cstring>
#include "Utils.h"

using namespace std;



int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Too few arguments. Please specify a command\n";
        return 1;
    }
    if (argc == 2 && string(argv[1]) == "init") {
        cout << "Initializing...\n";

        cout << "Done!\n";
        return 0;
    }

    if (argc == 4 && string(argv[1]) == "add" ) {
        if (!Utils::isValidFilePath(argv[2])) {
            cout << "Invalid file path\n";
            return 1;
        }
        if (!Utils::isValidTime(argv[3])) {
            cout << "Please enter a correct number of minutes\n";
            return 1;
        }

        cout << "Done!\n";
        return 0;
    }

    if (argc == 3 && string(argv[1]) == "remove") {
        if (!Utils::isValidFilePath(argv[2])) {
            cout << "Invalid file path\n";
            return 1;
        }

        cout << "Done!\n";
        return 0;
    }

    if (argc == 2 && string(argv[1]) == "list") {
        cout << "Done!\n";
        return 0;
    }


}



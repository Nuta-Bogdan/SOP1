#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

class CsvFileManager {
public:
    CsvFileManager(const std::string& filename) : filename_(filename) {}

    void writeRow(const std::string& element1, const std::string& element2, const std::string& element3) {
        std::ofstream file(filename_, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error opening file for writing: " << filename_ << std::endl;
            return;
        }

        file << element1 << "," << element2 << "," << element3 << "\n";
        file.close();
    }

    std::vector<std::vector<std::string>> readAllRows() {
        std::vector<std::vector<std::string>> rows;
        std::ifstream file(filename_);
        if (!file.is_open()) {
            std::cerr << "Error opening file for reading: " << filename_ << std::endl;
            return rows;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> row;
            std::istringstream iss(line);
            std::string element;
            while (std::getline(iss, element, ',')) {
                row.push_back(element);
            }
            rows.push_back(row);
        }

        file.close();
        return rows;
    }

private:
    std::string filename_ = "places.csv";
};


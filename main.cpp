/*
*     |\_/|
*    / | | \
*    \ _*_ /     _____
*    /     \    /   __\
*   / meow  \__/   /   \
*   \  za O(log(n)) ____\
*    \_  _ __  _  _/
*     \| \|  \| \|
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include "Genetic.h"

std::vector<Node> ReadFile(std::string &path) {
    std::ifstream file(path);
    std::string line;
    std::vector<std::vector<std::string>> data;
    std::vector<Node> boxes;

    if (!file.is_open()) {
        std::cerr << "Can't open file\n" << std::endl;
        return {};
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<std::string> row;

        while (std::getline(ss, value, ',')) {
            row.push_back(value);
        }

        data.push_back(row);
    }

    file.close();

    for (int i = 2; i < data.size(); ++i) {
        auto row = data[i];
        Node node;
        node.id_ = std::stoi(row[0]);
        node.count_ = std::stoi(row[1]);
        node.length_ = std::stoi(row[2]);
        node.wight_ = std::stoi(row[3]);
        node.height_ = std::stoi(row[4]);
        node.weight_ = std::stoi(row[5]);
        node.strength_ = std::stoi(row[6]);
        node.aisle_ = std::stoi(row[7]);
        node.caustic_ = std::stoi(row[8]);
        boxes.push_back(node);
    }
    return boxes;
}

int main() {
    ld sum2 = 0;
    for (int i = 1; i <= 436; ++i) {
        std::string path = "Newburgh Orders 0802 2011/"; // Change path if need
        path += std::to_string(i);
        path += ".csv";
        std::vector<Node> data = ReadFile(path);
        sum2 += Genetic(data, 800, 1200, 25, 100); // working +-5 min, res = 0.735853
        //sum2 += Genetic(data, 800, 1200, 50, 10); // working +-10 min, res = 0.74221
        //sum2 += Genetic(data, 800, 1200, 50, 250, 25, 25); // working +-14 min, res = 0.743678
        std::cout << i << "\n";
        if (i % 50 == 0) {
            std::cout << "Complete " << i << "\n";
        }
    }
    //std::cout << sum2 / 50 << "\n";
    std::cout << sum2 / 436 << "\n";
    return 0;
}


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
#include <filesystem>
#include "Genetic.h"

// Output file has format:
// box_id,count,length,wight,height,weight,strength,aisle,caustic
std::vector<Node> ReadFile(std::string &path) {
    std::ifstream file(path);
    std::string line;
    std::vector<std::vector<std::string>> data;
    std::vector<Node> boxes;

    if (!file.is_open()) {
        std::cerr << "Can't open input file\n" << std::endl;
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

// Output file has format:
// Total_H Percorellation
// box_id,x,y,z,x2,y2,z2 with x,y,z < x2,y2,z2
void WriteFile(std::string &path, std::vector<Corner> &ans) {
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Can't open output file\n" << std::endl;
        return;
    }

    int h = 0;
    for (int i = 0; i < ans.size(); ++i) {
        h = std::max(h, ans[i].h);
    }
    file << h << " " << Percorellation(ans, h, 800, 1200) << "\n";

    for (int i = 0; i < ans.size(); ++i) {
        file << ans[i].id << ',' << ans[i].w2 << ',' << ans[i].l2 << ',' << ans[i].h2 << ',' << ans[i].w << ','
             << ans[i].l << ',' << ans[i].h << '\n';
    }

    file.close();
}

//Genetic(data, 800, 1200, 25, 100); // working +-5 min, res = 0.756315
//Genetic(data, 800, 1200, 50, 10); // working +-10 min, res = 0.762408
//Genetic(data, 800, 1200, 100, 10); // working +-20 min, res = 0.763648

int main() {
    ld sum2 = 0;
    std::string dir_name = "Output"; // Dir with result Placement

    if (!std::filesystem::exists(dir_name)) {
        if (!std::filesystem::create_directory(dir_name)) {
            std::cerr << "Can't create the directory" << std::endl;
        }
    }

    for (int i = 1; i <= 436; ++i) {
        std::string input = "Input/"; // Change path if you need it
        input += std::to_string(i);
        input += ".csv";
        std::vector<Node> data = ReadFile(input);

        auto res = Genetic(data, 800, 1200, 25, 100);
        sum2 += res.first;

        std::string output = "Output/"; // Change path if you need it
        output += std::to_string(i);
        output += ".csv";
        WriteFile(output, res.second);

        if (i % 50 == 0) {
            std::cout << "Complete " << i << "\n";
        }
    }
    std::cout << sum2 / 436 << "\n";
    return 0;
}


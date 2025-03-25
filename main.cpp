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

#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>
#include <filesystem>

typedef long long ll;
typedef long double ld;

struct Node {
    //initial by params
    Node(int id, int count, int height, int length, int wight, int weight, int strength, int aisle, int caustic) : id_(
            id), count_(count), length_(length), wight_(wight), height_(height), weight_(weight), strength_(strength),
                                                                                                                   aisle_(aisle),
                                                                                                                   caustic_(
                                                                                                                           caustic) {}

    Node() = default;

    int id_ = 0;
    int count_ = 0;
    int height_ = 0;
    int wight_ = 0;
    int length_ = 0;
    int weight_ = 0; // not used
    int strength_ = 0; // not used
    int aisle_ = 0; // not used
    int caustic_ = 0; // not used
};

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

struct Corner {
    int id;
    int h; // corner closer to H W L
    int w;
    int l;
    int h2; // corner closer to 0 0 0
    int w2;
    int l2;
};

std::vector<Corner> ans;

std::unordered_map<unsigned int, std::shared_ptr<Node>> box_params_by_id;

int Dist(const Corner &corn1, const Corner &corn2) {
    Corner corn = {0, corn1.h - corn2.h, corn1.w - corn2.w, corn1.l - corn2.l};
    return corn.w * corn.w + corn.h * corn.h + corn.l * corn.l;
}

struct ComparatorForFirstCorner {
    bool operator()(const Corner &first, const Corner &second) const {
        Corner corn = {0, 0, 0, 0};
        return Dist(corn, first) < Dist(corn, second);
    }
};

struct ComparatorForSecondCorner {
    bool operator()(const Corner &first, const Corner &second) const {
        Corner corn = {0, 0, 800, 1200};
        return Dist(corn, first) < Dist(corn, second);
    }
};

std::set<Corner, ComparatorForFirstCorner> best_for_first; // Corners in Genetic
std::set<Corner, ComparatorForSecondCorner> best_for_second;

bool Intersection(int l1, int r1, int l2, int r2) {
    if (l1 > l2) {
        std::swap(l1, l2);
        std::swap(r1, r2);
    }
    return l2 < r1;
}

bool IntersectionBoxes(Corner first, Corner second) { //return true if the boxes intersect
    return Intersection(first.h2, first.h, second.h2, second.h)
           && Intersection(first.w2, first.w, second.w2, second.w)
           && Intersection(first.l2, first.l, second.l2, second.l);
}

ld Percorellation(int H, int W, int L) {
    ld V = 0;
    for (Corner &corn: ans) {
        V += static_cast<ll>(corn.w - corn.w2) * (corn.l - corn.l2) * (corn.h - corn.h2);
    }
    ll pallet = static_cast<ll>(H) * W * L;
    return V / pallet;
}

void Rotate(std::shared_ptr<Node> &node, unsigned int j) {
    if (j <= 1) {
        return;
    } else if (j == 2) {
        std::swap(node->wight_, node->length_);
    } else if (j == 3) {
        std::swap(node->height_, node->wight_);
    } else if (j == 4) {
        std::swap(node->height_, node->wight_);
        std::swap(node->wight_, node->length_);
    } else if (j == 5) {
        std::swap(node->height_, node->length_);
    } else {
        std::swap(node->height_, node->length_);
        std::swap(node->wight_, node->length_);
    }
}

void Clear() {
    ans.clear();
    best_for_second.clear();
    best_for_first.clear();
    box_params_by_id.clear();
}

struct Gen {
    unsigned int box_id;
    unsigned int rotation;
    unsigned int type;
    unsigned int index;
};

std::mt19937 rnd(1337);

void Mutation(std::vector<Gen> &data, unsigned int count1, unsigned int count2) {
    for (int i = 0; i < count1; ++i) { // random change rotation and type
        unsigned int ind = rnd() % data.size();
        data[ind].rotation = rnd() % 7;
        data[ind].type = rnd() % 2;
    }
    for (int j = 0; j < count2; ++j) {
        unsigned int a = rnd() % data.size();
        unsigned int b = rnd() % data.size();
        std::swap(data[a].index, data[b].index);
    }
}

bool ComparatorForCrossing(const Gen &first, const Gen &second) {
    return first.box_id < second.box_id;
}

bool ComparatorForByIndex(const Gen &first, const Gen &second) {
    return first.index < second.index;
}

std::vector<Gen> Crossing(std::vector<Gen> first, std::vector<Gen> second) {
    std::vector<Gen> child(first.size());
    if (first.size() != second.size()) {
        std::cout << first.size() << " " << second.size() << "\n";
        throw std::runtime_error("Bad genotypes size in Crossing\n");
    }
    std::sort(first.begin(), first.end(), ComparatorForCrossing);
    std::sort(second.begin(), second.end(), ComparatorForCrossing);
    unsigned int perm = rnd() % 2;
    for (int i = 0; i < child.size(); ++i) {
        unsigned int a;
        a = rnd() % 2;
        if (a) {
            child[i] = first[i];
        } else {
            child[i] = second[i];
        }
        if (perm) {
            child[i].index = first[i].index;
        } else {
            child[i].index = second[i].index;
        }
    }
    return child;
}

bool CompForData(Node &first, Node &second) {
    return std::max(std::max(first.height_, first.wight_), first.length_) >
           std::max(std::max(second.height_, second.wight_), second.length_);
}

bool CompForData2(Node &first, Node &second) {
    return std::tie(first.height_, first.wight_, first.length_) >
           std::tie(second.height_, second.wight_, second.length_);
}

std::vector<std::vector<Gen>> GeneratePopulation(std::vector<Node> &data, int count) {
    std::vector<std::vector<Gen>> res(count + 4, std::vector<Gen>());
    for (int i = 0; i < count; ++i) { // random genotype
        for (int j = 0; j < data.size(); ++j) {
            for (int k = 0; k < 1; ++k) {
                //for (int k = 0; k < data[j].count_; ++k) { // TODO FIX count
                res[i].emplace_back();
                res[i].back().index = res[i].size() - 1;
                res[i].back().box_id = data[j].id_;
                res[i].back().rotation = 0;
                res[i].back().type = rnd() % 2;
            }
        }
        for (int j = 0; j < 2 * data.size(); ++j) { // shuffle
            unsigned int a, b;
            a = rnd() % res[i].size();
            b = rnd() % res[i].size();
            std::swap(res[i][a].index, res[i][b].index);
        }
    }
    std::sort(data.begin(), data.end(), CompForData2);
    for (int i = 0; i < 4; ++i) { // special genotype
        for (int j = 0; j < data.size(); ++j) {
            for (int k = 0; k < 1; ++k) {
                //for (int k = 0; k < data[j].count_; ++k) { // TODO FIX count
                res[count + i].emplace_back();
                res[count + i].back().index = res[count + i].size() - 1;
                res[count + i].back().box_id = data[j].id_;
                res[count + i].back().rotation = 0;
                res[count + i].back().type = rnd() % 2;
            }
        }
        if (i == 1) {
            std::sort(data.begin(), data.end(), CompForData);
        }
    }
    return res;
}

void PlaceFirstType(Gen &gen, int W, int L, int H, bool check) {
    Node box = *box_params_by_id[gen.box_id];
    for (Corner corn: best_for_first) {
        Corner copy = corn;
        corn.id = box.id_;
        corn.w += box.wight_;
        corn.h += box.height_;
        corn.l += box.length_;
        if (corn.w > W || corn.l > L) {
            continue;
        }
        bool good = true;
        for (Corner &corn2: ans) {
            if (IntersectionBoxes(corn, corn2)) {
                good = false;
                break;
            }
            // TODO : Clear bad corners in set
        }
        if (good) {
            ans.push_back(corn);
            if (check) {
                return;
            }
            best_for_first.erase(copy); // add new corners in set
            copy.w += box.wight_;
            copy.w2 += box.wight_;
            best_for_first.insert(copy);
            copy.w -= box.wight_;
            copy.w2 -= box.wight_;
            copy.h += box.height_;
            copy.h2 += box.height_;
            best_for_first.insert(copy);
            copy.h -= box.height_;
            copy.h2 -= box.height_;
            copy.l += box.length_;
            copy.l2 += box.length_;
            best_for_first.insert(copy);
            return;
        }
    }
    Corner corn = {box.id_, H + box.height_, box.wight_, box.length_, H, 0, 0};
    ans.push_back(corn);
    if (check) {
        return;
    }
    best_for_first.insert({0, H, 0, box.length_, H, 0, box.length_});
    best_for_first.insert({0, H, box.wight_, 0, H, box.wight_, 0});
    best_for_first.insert({0, H + box.height_, 0, 0, H + box.height_, 0, 0});
}

void PlaceSecondType(Gen &gen, int W, int L, int H, bool check) {
    Node box = *box_params_by_id[gen.box_id];
    for (Corner corn: best_for_second) {
        Corner copy = corn;
        corn.id = box.id_;
        corn.h2 = corn.h;
        corn.h += box.height_;
        corn.w2 = corn.w - box.wight_;
        corn.l2 = corn.l - box.length_;
        if (corn.w2 < 0 || corn.l2 < 0) {
            continue;
        }
        bool good = true;
        for (Corner &corn2: ans) {
            if (IntersectionBoxes(corn, corn2)) {
                good = false;
                break;
            }
            // TODO : Clear bad corners in set
        }
        if (good) {
            ans.push_back(corn);
            if (check) {
                return;
            }
            best_for_second.erase(copy);
            copy.w -= box.wight_;
            best_for_second.insert(copy);
            copy.w += box.wight_;
            copy.h += box.height_;
            best_for_second.insert(copy);
            copy.h -= box.height_;
            copy.l -= box.length_;
            best_for_second.insert(copy);
            return;
        }
    }
    Corner corn = {box.id_, H + box.height_, W, L, H, W - box.wight_, L - box.length_};
    ans.push_back(corn);
    if (check) {
        return;
    }
    best_for_second.insert({0, H, W - box.wight_, L});
    best_for_second.insert({0, H, W, L - box.length_});
    best_for_second.insert({0, H + box.height_, W, L});
}

void CheckCorrectness(unsigned int size) {
    for (int a = 0; a < ans.size(); ++a) {
        if (ans[a].h2 < 0) {
            throw std::runtime_error("ERR_H\n");
        }
        if (ans[a].w2 < 0 || ans[a].w > 800) {
            throw std::runtime_error("ERR_W\n");
        }
        if (ans[a].l2 < 0 || ans[a].l > 1200) {
            throw std::runtime_error("ERR_L\n");
        }
        for (int b = a + 1; b < ans.size(); ++b) {
            if (IntersectionBoxes(ans[a], ans[b]) || ans.size() != size) {
                std::cout << ans[a].id << " " << ans[b].id << "\n";

                std::cout << ans[a].h2 << " " << ans[a].h << "\n";
                std::cout << ans[a].w2 << " " << ans[a].w << "\n";
                std::cout << ans[a].l2 << " " << ans[a].l << "\n";

                std::cout << ans[b].h2 << " " << ans[b].h << "\n";
                std::cout << ans[b].w2 << " " << ans[b].w << "\n";
                std::cout << ans[b].l2 << " " << ans[b].l << "\n";
                throw std::runtime_error("ERROR_Intersect\n");
            }
        }
    }
}

ld CalcPercGen(std::vector<Gen> genotype, std::vector<Node> &data, int W, int L) {
    Clear();
    for (int i = 0; i < data.size(); ++i) {
        box_params_by_id[data[i].id_] = std::make_shared<Node>(data[i]);
    }
    best_for_first.insert({0, 0, 0, 0, 0, 0, 0});
    best_for_second.insert({0, 0, W, L});
    std::sort(genotype.begin(), genotype.end(), ComparatorForByIndex);
    int H = 0;
    for (int i = 0; i < genotype.size(); ++i) {
        if (genotype[i].rotation == 0) {
            int bestH = H + 10000;
            int best_rot = 0;
            for (int j = 0; j < 6; ++j) {
                *box_params_by_id[data[i].id_] = data[i];
                Rotate(box_params_by_id[genotype[i].box_id], j);
                if (genotype[i].type == 0) {
                    PlaceFirstType(genotype[i], W, L, H, true);
                } else {
                    PlaceSecondType(genotype[i], W, L, H, true);
                }
                int H0 = std::max(H, ans.back().h);
                if (bestH > H0) {
                    bestH = H0;
                    best_rot = j;
                }
                ans.pop_back();
            }
            *box_params_by_id[data[i].id_] = data[i];
            Rotate(box_params_by_id[genotype[i].box_id], best_rot);
            if (genotype[i].type == 0) {
                PlaceFirstType(genotype[i], W, L, H, false);
            } else {
                PlaceSecondType(genotype[i], W, L, H, false);
            }
            H = std::max(H, ans.back().h);
            continue;
        }
        Rotate(box_params_by_id[genotype[i].box_id], genotype[i].rotation); // TODO make Uniq id for boxes :(
        if (genotype[i].type == 0) {
            PlaceFirstType(genotype[i], W, L, H, false);
        } else {
            PlaceSecondType(genotype[i], W, L, H, false);
        }
        H = std::max(H, ans.back().h);
    }
    CheckCorrectness(data.size());
    return Percorellation(H, W, L);
}

bool cmp(std::pair<ld, std::vector<Gen>> &first, std::pair<ld, std::vector<Gen>> &second) {
    return first.first > second.first;
}

ld Genetic(std::vector<Node> data, int W, int L, int generations,
           int count = 100, int cross_count = 20,
           int mutation_count = 10) { // count - genotypes count in first population
    std::vector<std::vector<Gen>> population = GeneratePopulation(data, count);
    ld best = 0;
    for (int i = 0; i < generations; ++i) {
        unsigned int sz = population.size();
        for (int j = 0; j < cross_count; ++j) {
            unsigned int a, b;
            a = rnd() % sz;
            b = rnd() % sz;
            if (a != b) {
                population.push_back(Crossing(population[a], population[b]));
            }
        }
        sz = population.size();
        if (i != 0) {
            for (int j = 0; j < mutation_count; ++j) {
                unsigned int a;
                a = rnd() % sz;
                Mutation(population[a], data.size() / 25, data.size() / 25);
            }
        }
        std::vector<std::pair<ld, std::vector<Gen>>> gen_with_perc(population.size());
        for (int j = 0; j < population.size(); ++j) {
            gen_with_perc[j] = std::make_pair(CalcPercGen(population[j], data, W, L), population[j]);
        }
        std::sort(gen_with_perc.begin(), gen_with_perc.end(), cmp);
        while (gen_with_perc.size() > 15) {
            gen_with_perc.pop_back();
        }
        population.resize(gen_with_perc.size());
        for (int j = 0; j < gen_with_perc.size(); ++j) {
            population[j] = gen_with_perc[j].second;
        }
        best = std::max(best, gen_with_perc[0].first);
    }
    return best;
}

int main() {
    ld sum2 = 0;
    for (int i = 1; i <= 436; ++i) {
        std::string path = "Newburgh Orders 0802 2011/"; // Change path if need
        path += std::to_string(i);
        path += ".csv";
        std::vector<Node> data = ReadFile(path);
        sum2 += Genetic(data, 800, 1200, 25, 100); // working 11 min, res = 0.743974
        std::cout << i << "\n";
        if (i % 50 == 0) {
            std::cout << "Complete " << i << "\n";
        }
    }
    //std::cout << sum2 / 50 << "\n";
    std::cout << sum2 / 436 << "\n";
    return 0;
}
#include "Genetic.h"

std::vector<Corner> ans;

std::unordered_map<unsigned int, std::shared_ptr<Node>> box_params_by_id;

std::set<Corner, ComparatorForFirstCorner> best_for_first; // Corners in Genetic
std::set<Corner, ComparatorForSecondCorner> best_for_second;

std::vector<Corner> global_ans;

std::mt19937 rnd;

void Clear() {
    ans.clear();
    best_for_second.clear();
    best_for_first.clear();
    box_params_by_id.clear();
}

void Mutation(std::vector<Gen> &data, unsigned int count1, unsigned int count2) {
    for (int i = 0; i < count1; ++i) { // random change rotation and type
        unsigned int ind = rnd() % data.size();
        data[ind].rotation = rnd() % 3;
        data[ind].type = rnd() % 2;
    }
    for (int j = 0; j < count2; ++j) {
        unsigned int a = rnd() % data.size();
        unsigned int b = rnd() % data.size();
        std::swap(data[a].index, data[b].index);
    }
}

std::vector<Gen> Crossing(std::vector<Gen> first, std::vector<Gen> second) {
    std::vector<Gen> child(first.size());
    if (first.size() != second.size()) {
        //std::cout << first.size() << " " << second.size() << "\n";
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

std::vector<std::vector<Gen>> GeneratePopulation(std::vector<Node> &data, int count) {
    std::vector<std::vector<Gen>> res(count + 10, std::vector<Gen>());
    for (int i = 0; i < count; ++i) { // random genotype
        for (int j = 0; j < data.size(); ++j) {
            for (int k = 0; k < data[j].count_; ++k) {
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
    for (int i = 0; i < 10; ++i) { // special genotype
        for (int j = 0; j < data.size(); ++j) {
            for (int k = 0; k < data[j].count_; ++k) {
                res[count + i].emplace_back();
                res[count + i].back().index = res[count + i].size() - 1;
                res[count + i].back().box_id = data[j].id_;
                res[count + i].back().rotation = 0;
                res[count + i].back().type = rnd() % 2;
            }
        }
        if (i == 5) {
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
    if (size != ans.size()) {
        throw std::runtime_error("Not all the boxes are packed\n");
    }
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
            if (IntersectionBoxes(ans[a], ans[b])) {
//                std::cout << ans[a].id << " " << ans[b].id << "\n";
//
//                std::cout << ans[a].h2 << " " << ans[a].h << "\n";
//                std::cout << ans[a].w2 << " " << ans[a].w << "\n";
//                std::cout << ans[a].l2 << " " << ans[a].l << "\n";
//
//                std::cout << ans[b].h2 << " " << ans[b].h << "\n";
//                std::cout << ans[b].w2 << " " << ans[b].w << "\n";
//                std::cout << ans[b].l2 << " " << ans[b].l << "\n";
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
    int ind = 0; // index box in data
    int row = 0;
    for (int i = 0; i < genotype.size(); ++i, ++row) {
        if (row == data[ind].count_) {
            ++ind;
            row = 0;
        }
        if (genotype[i].rotation == 0) {
            int bestH = H + 10000;
            int best_rot = 0;
            for (int j = 1; j <= 2; ++j) {
                *box_params_by_id[data[ind].id_] = data[ind];
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
            *box_params_by_id[data[ind].id_] = data[ind];
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
    CheckCorrectness(genotype.size());
    return Percorellation(ans, H, W, L);
}

std::pair<ld, std::vector<Corner>> Genetic(std::vector<Node> data, int W, int L, int generations,
           int count, int cross_count, int pop_limit,
           int mutation_count) { // count - genotypes count in first population
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
            if (gen_with_perc[j].first > best) {
                best = std::max(best, gen_with_perc[j].first);
                global_ans = ans;
            }
        }
        std::sort(gen_with_perc.begin(), gen_with_perc.end(), cmp);
        while (gen_with_perc.size() > pop_limit) {
            gen_with_perc.pop_back();
        }
        population.resize(gen_with_perc.size());
        for (int j = 0; j < gen_with_perc.size(); ++j) {
            population[j] = gen_with_perc[j].second;
        }
    }
    return {best, global_ans};
}
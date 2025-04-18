#pragma once
#include "FCNR.h"

std::vector<Corner> ans;

std::vector<std::vector<std::vector<Corner>>> corners_at_floor; // vector of "floor" corners futhest from 0 0 0.
// ans[i][j] - corners at i level in 3d and j level in 2d

std::vector<std::vector<std::vector<Corner>>> corners_at_ceil; // vector of "ceil" corners futhest from 0 0 0.
// ans[i][j] - corners at i level in 3d and j level in 2d

std::vector<std::vector<Corner>> corners_at_ceil3d;

std::vector<std::vector<std::set<Corner>>> places; // vector of "ceil" corners which can be used to place new box by
std::vector<std::set<Corner>> places3d; // vector of "ceil" corners which can be used to place new box by
// corner futhest from 0 0 0. ans[i][j] - corners at i level in 3d and j level in 2d

std::unordered_map<int, std::shared_ptr<Node>> box_params_by_id;

bool IntersectionBoxes2(Corner &first, Corner &second) { //return true if the boxes intersect
    std::shared_ptr<Node> box1 = box_params_by_id[first.id];
    std::shared_ptr<Node> box2 = box_params_by_id[second.id];
    return Intersection(first.h - box1->height_, first.h, second.h - box2->height_, second.h)
           && Intersection(first.w - box1->wight_, first.w, second.w - box2->wight_, second.w)
           && Intersection(first.l - box1->length_, first.l, second.l - box2->length_, second.l);
}

bool TrySetCeil2D(std::vector<Node> &data, int &start) {
    for (int i = 0; i < places.size(); ++i) {
        for (int j = 0; j < places[i].size(); ++j) {
            for (Corner corn: places[i][j]) {
                auto first = corners_at_floor[i][j][0];
                if (corn.w - data[start].wight_ < first.w - box_params_by_id[first.id]->wight_) {
                    continue;
                }
                int l = 0;
                int r = corners_at_floor[i][j].size();
                while (r - l > 1) {
                    int m = (r + l) / 2;
                    if (corners_at_floor[i][j][m].w <= corn.w - data[start].wight_) {
                        r = m;
                    } else {
                        l = m;
                    }
                }
                if (corners_at_floor[i][j][r - 1].l <= corn.l - data[start].length_) { // can set box
                    ans.push_back(corn);
                    ans.back().h += data[start].height_;
                    ans.back().id = data[start].id_;
                    bool ok = true;
                    for (Corner &corn2: corners_at_ceil[i][j]) {
                        if (IntersectionBoxes2(corn2, ans.back())) {
                            ok = false;
                            ans.pop_back();
                            break;
                        }
                    }
                    if (!ok) {
                        continue;
                    }
                    if (corners_at_ceil3d.size() > i) {
                        for (Corner &corn2: corners_at_ceil3d[i]) {
                            if (IntersectionBoxes2(corn2, ans.back())) {
                                ok = false;
                                ans.pop_back();
                                break;
                            }
                        }
                        if (!ok) {
                            continue;
                        }
                    }

                    corners_at_ceil[i][j].push_back(ans.back());
                    places[i][j].erase(corn);
                    places[i][j].insert({0, corn.h, corn.w - data[start].wight_, corn.l}); // add places (optional)
                    places[i][j].insert({0, corn.h, corn.w, corn.l - data[start].length_});
                    places[i][j].erase(corn);
                    return true;
                }
            }
        }
    }

    return false;
}

bool Fill2DLevel(std::vector<Node> &data, int W, int L, int H0, int W0, int &start) {
    corners_at_floor.back().emplace_back(); // create new level in 2d
    corners_at_ceil.back().emplace_back(); // create new level in 2d
    places.back().emplace_back(); // create new level in 2d
    int L0 = 0;
    bool use_ceil = false;
    while (start < data.size()) {
        if (W0 + data[start].wight_ > W) {
            return L0 != 0;
        }
        if (L0 + data[start].length_ > L) {
            if (TrySetCeil2D(data, start)) {
                use_ceil = true;
                --data[start].count_;
                if (!data[start].count_) {
                    ++start;
                }
                continue;
            } else {
                return L0 != 0;
            }
        }
        if (use_ceil) {
            return L0 != 0;
        }
        if (L0 == 0) {
            places.back().back().insert({0, H0, W0 + data[start].wight_, L});
        }
        corners_at_floor.back().back().push_back({data[start].id_, H0 + data[start].height_,
                                                  W0 + data[start].wight_, L0 + data[start].length_});
        ans.push_back({data[start].id_, H0 + data[start].height_,
                       W0 + data[start].wight_, L0 + data[start].length_});
        L0 += data[start].length_;
        --data[start].count_;
        if (data[start].count_ == 0) {
            ++start;
        }
    }
    return L0 != 0;
}

bool FCNR_2D(std::vector<Node> &data, int W, int L, int H0, int &start) {
    corners_at_floor.emplace_back(); // create first level in 3d
    corners_at_floor.back().emplace_back(); // create first level in 2d
    corners_at_ceil.emplace_back(); // create first level in 3d
    corners_at_ceil.back().emplace_back(); // create first level in 2d
    places.emplace_back(); // create first level in 3d
    places.back().emplace_back(); // create first level in 2d

    int W0 = 0;
    while (start < data.size()) {
        int level_w = data[start].wight_;
        if (start + 1 < data.size() && data[start + 1].wight_ > level_w) {
            level_w = data[start + 1].wight_;
        }
        if (!Fill2DLevel(data, W, L, H0, W0, start)) {
            return W0 != 0;
        }
        W0 += level_w;
    }
    return W0 != 0;
}

int FCNR_3D(std::vector<Node> &data, int W, int L) {
    int start = 0;
    int H0 = 0;
    while (start < data.size()) {
        int ind = start;
        for (int i = start + 1; i < data.size(); ++i) {
            if (data[i].height_ > data[ind].height_) {
                ind = i;
            }
        }
        Node node = data[ind];
        int level_h = node.height_;
        data.erase(std::remove(data.begin(), data.end(), node), data.end());
        data.insert(data.begin() + start, node);
        places3d.emplace_back();
        corners_at_ceil3d.emplace_back();
        FCNR_2D(data, W, L, H0, start);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (i == 0 && j == 0) continue;
                places3d.back().insert({0, H0 + level_h, (W / 2) * i, (L / 2) * j});
            }
        }
//        while (start < data.size()) {
//            if (!TrySetCeil3D(data, start, H0)) {
//                break;
//            }
//            if (ans.back().h - box_params_by_id[ans.back().id]->height_ < H0 || ans.back().h > H0 + level_h) {
//                std::cout << "error\n";
//                ans.pop_back();
//                break;
//            }
//            --data[start].count_;
//            if (!data[start].count_) {
//                ++start;
//            }
//        }
        H0 += level_h;
    }
    return H0;
}

ld Percorellation(int H, int W, int L) {
    ld V = 0;
    for (Corner &corn: ans) {
        std::shared_ptr<Node> box = box_params_by_id[corn.id];
        V += static_cast<ll>(box->wight_) * box->length_ * box->height_;
    }
    ll pallet = static_cast<ll>(H) * W * L;
    return V / pallet;
}

ld min = 1;
ld max = 0;

void Rotate(Node &node, int j) {
    if (j == 0) {
        return;
    } else if (j == 1) {
        std::swap(node.wight_, node.length_);
    }
}

void ClearFCNR() {
    ans.clear();
    box_params_by_id.clear();
    corners_at_floor.clear();
    corners_at_ceil.clear(); // clear all
    corners_at_ceil3d.clear();
    places.clear();
    places3d.clear();
}

ld SolveFCNR(std::vector<Node> data, int W, int L) {
    ld best = 0;
    std::vector<Node> data2 = data;
    for (int i = 0; i < data.size(); ++i) {
        if (data2[i].height_ < data2[i].wight_) {
            std::swap(data2[i].height_, data2[i].wight_);
        }
        if (data2[i].height_ < data2[i].length_) {
            std::swap(data2[i].height_, data2[i].length_);
        }
        if (data2[i].wight_ < data2[i].length_) {
            std::swap(data2[i].wight_, data2[i].length_);
        }
    }
    for (int j = 0; j < 2; ++j) {
        data = data2;
        for (int i = 0; i < data.size(); ++i) {
            Rotate(data[i], j);
        }
        std::sort(data.begin(), data.end(), cmp_FCNR);
        ClearFCNR();
        for (int i = 0; i < data.size(); ++i) {
            box_params_by_id[data[i].id_] = std::make_shared<Node>(data[i]);
        }
        int H = FCNR_3D(data, W, L);
        best = std::max(best, Percorellation(H, W, L));
    }
    min = std::min(min, best);
    max = std::max(max, best);
    return best;
}
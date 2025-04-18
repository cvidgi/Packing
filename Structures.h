#pragma once
struct Node {
    //initial by params
    Node(int id, int count, int height, int length, int wight, int weight, int strength, int aisle, int caustic) : id_(
            id), count_(count), length_(length), wight_(wight), height_(height), weight_(weight), strength_(strength),
                                                                                                                   aisle_(aisle),
                                                                                                                   caustic_(
                                                                                                                           caustic) {}

    Node() = default;

    bool operator==(const Node &other) const = default;

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

struct Corner {
    int id;
    int h; // corner closer to H W L
    int w;
    int l;
    int h2; // corner closer to 0 0 0
    int w2;
    int l2;

    bool operator<(const Corner &c) const {
        return std::tie(h, w, l) <
               std::tie(c.h, c.w, c.l);
    }
};

struct Gen {
    unsigned int box_id;
    unsigned int rotation;
    unsigned int type;
    unsigned int index;

    bool operator==(const Gen &other) const {
        return (rotation == other.rotation) && (type == other.type);
    }
};
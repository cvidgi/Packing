#include <algorithm>
#include <vector>
#include "Structures.h"

typedef long long ll;
typedef long double ld;

inline int Dist(const Corner &corn1, const Corner &corn2) {
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

inline bool ComparatorForCrossing(const Gen &first, const Gen &second) {
    return first.box_id < second.box_id;
}

inline bool ComparatorForByIndex(const Gen &first, const Gen &second) {
    return first.index < second.index;
}

inline bool CompForData(Node &first, Node &second) {
    return std::max(std::max(first.height_, first.wight_), first.length_) >
           std::max(std::max(second.height_, second.wight_), second.length_);
}

inline bool CompForData2(Node &first, Node &second) {
    return std::tie(first.height_, first.wight_, first.length_) >
           std::tie(second.height_, second.wight_, second.length_);
}

inline bool cmp(std::pair<ld, std::vector<Gen>> &first, std::pair<ld, std::vector<Gen>> &second) {
    return first.first > second.first;
}
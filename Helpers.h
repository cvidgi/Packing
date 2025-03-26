#include <memory>
#include "Comparators.h"

inline bool Intersection(int l1, int r1, int l2, int r2) {
    if (l1 > l2) {
        return l1 < r2;
    }
    return l2 < r1;
}

inline bool IntersectionBoxes(Corner &first, Corner &second) { // return true if the boxes intersect
    return Intersection(first.h2, first.h, second.h2, second.h)
           && Intersection(first.w2, first.w, second.w2, second.w)
           && Intersection(first.l2, first.l, second.l2, second.l);
}

ld Percorellation(std::vector<Corner> &ans, int H, int W, int L);

void Rotate(std::shared_ptr<Node> &node, unsigned int j);
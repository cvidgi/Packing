#pragma once
#include "Helpers.h"

ld Percorellation(std::vector<Corner> &ans, int H, int W, int L) {
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
    } else {
        std::swap(node->wight_, node->length_);
    }
}
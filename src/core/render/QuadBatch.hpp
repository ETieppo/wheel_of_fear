#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "core/render/QuadProps.hpp"

namespace wheel_of_fear {

class QuadBatch {
public:
    explicit QuadBatch(std::size_t capacity) : capacity_(capacity) { items_.reserve(capacity); }

    void push(const QuadProps& quad) {
        if (items_.size() >= capacity_) return;
        items_.push_back(quad);
    }

    void clear() { items_.clear(); }
    std::span<const QuadProps> used() const { return items_; }
    std::size_t count() const { return items_.size(); }
    std::size_t capacity() const { return capacity_; }

private:
    std::size_t capacity_;
    std::vector<QuadProps> items_;
};

}

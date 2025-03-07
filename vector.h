#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <span>

class Vector {
   public:
    class Iterator {
       public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = int*;
        using reference = int&;

        Iterator() : it_(nullptr) {
        }

        explicit Iterator(pointer it) : it_(it) {
        }

        reference operator*() const {
            return *it_;
        }

        reference operator*() {
            return *it_;
        }

        pointer operator->() const {
            return it_;
        }

        auto operator<=>(const Iterator& x) const = default;

        Iterator& operator++() {
            it_++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(it_);
            it_++;
            return tmp;
        }

        Iterator& operator--() {
            it_--;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp(it_);
            it_--;
            return tmp;
        }

        Iterator& operator+=(const difference_type& n) {
            if (n >= 0) {
                it_ += n;
            } else {
                it_ -= -n;
            }
            return *this;
        }

        Iterator operator+(const difference_type& n) const {
            Iterator tmp(it_);
            tmp += n;
            return tmp;
        }

        friend Iterator operator+(const difference_type& n, const Iterator& x) {
            Iterator tmp(x);
            tmp += n;
            return tmp;
        }

        Iterator& operator-=(const difference_type& n) {
            *this += -n;
            return *this;
        }

        Iterator operator-(const difference_type& n) const {
            Iterator tmp(it_);
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const Iterator& x) const {
            return it_ - x.it_;
        }

        reference operator[](const difference_type& n) const {
            return *(it_ + n);
        }

        reference operator[](const difference_type& n) {
            return *(it_ + n);
        }

       private:
        pointer it_;
    };

    Vector() : begin_(nullptr), size_(0), capacity_(0) {
    }

    explicit Vector(size_t n)
        : begin_(new int[n]), size_(n), capacity_(n) {  // NOLINT(cppcoreguidelines-owning-memory)
        for (size_t i = 0; i < size_; i++) {
            std::span(begin_, capacity_)[i] = 0;
        }
    }

    Vector(std::initializer_list<int> data)
        : begin_(new int[data.size()])  // NOLINT(cppcoreguidelines-owning-memory)
        , size_(data.size())
        , capacity_(data.size()) {
        size_t i = 0;
        for (auto j : data) {
            std::span(begin_, capacity_)[i] = j;
            i++;
        }
    }

    Vector(const Vector& data)
        : begin_(new int[data.capacity_])  // NOLINT(cppcoreguidelines-owning-memory)
        , size_(data.size_)
        , capacity_(data.capacity_) {
        for (size_t i = 0; i < data.size_; i++) {
            std::span(begin_, capacity_)[i] = std::span(data.begin_, data.capacity_)[i];
        }
    }

    Vector(Vector&& data) noexcept
        : begin_(data.begin_), size_(data.size_), capacity_(data.capacity_) {
        data.begin_ = nullptr;
    }

    Vector& operator=(const Vector& data) {
        if (&data != this) {
            delete[] begin_;
            begin_ = new int[data.capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
            size_ = data.size_;
            capacity_ = data.capacity_;
            for (size_t i = 0; i < data.size_; i++) {
                std::span(begin_, capacity_)[i] = std::span(data.begin_, data.capacity_)[i];
            }
        }
        return *this;
    }

    Vector& operator=(Vector&& data) noexcept {
        if (&data != this) {
            delete[] begin_;
            begin_ = data.begin_;
            size_ = data.size_;
            capacity_ = data.capacity_;
            data.begin_ = nullptr;
        }
        return *this;
    }

    ~Vector() {
        delete[] begin_;
    }

    int& operator[](const size_t& i) const {
        return std::span(begin_, capacity_)[i];
    }

    int& operator[](const size_t& i) {
        return std::span(begin_, capacity_)[i];
    }

    [[nodiscard]] Iterator begin() const {
        Iterator tmp(begin_);
        return tmp;
    }

    [[nodiscard]] Iterator end() const {
        Iterator tmp(&std::span(begin_, capacity_)[size_]);
        return tmp;
    }

    Vector Swap(Vector& data) {
        const size_t capacity1 = capacity_;
        const size_t capacity2 = data.capacity_;
        const Vector tmp = *this;
        *this = data;
        data = tmp;
        Reserve(std::max(capacity1, size_));
        data.Reserve(std::max(capacity2, data.size_));
        return *this;
    }

    [[nodiscard]] size_t Size() const {
        return size_;
    }

    [[nodiscard]] size_t Capacity() const {
        return capacity_;
    }

    Vector& PushBack(const int& x) {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                Reserve(1);
            } else {
                Reserve(capacity_ * 2);
            }
        }
        std::span(begin_, capacity_)[size_] = x;
        size_++;
        return *this;
    }

    Vector& PopBack() {
        size_--;
        return *this;
    }

    Vector& Clear() {
        size_ = 0;
        return *this;
    }

    void Reserve(const size_t& capacity) {
        if (capacity > capacity_) {
            int* new_begin = new int[capacity];  // NOLINT(cppcoreguidelines-owning-memory)
            for (size_t i = 0; i < size_; i++) {
                std::span(new_begin, capacity_)[i] = std::span(begin_, capacity_)[i];
            }
            delete[] begin_;
            begin_ = new_begin;
            new_begin = nullptr;
            capacity_ = capacity;
        }
    }

   private:
    int* begin_;
    size_t size_;
    size_t capacity_;
};

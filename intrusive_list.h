#pragma once

#include <cstddef>
#include <iterator>

class ListHook {
   public:
    [[nodiscard]] bool IsLinked() const {
        return next_ != this;
    }

    void Unlink() {
        prev_->next_ = next_;
        next_->prev_ = prev_;
        prev_ = this;
        next_ = this;
    }

    void LinkAfter(ListHook* other) {
        other->Unlink();
        other->prev_ = this;
        other->next_ = next_;
        next_->prev_ = other;
        next_ = other;
    }

    ListHook(const ListHook&) = delete;
    ListHook& operator=(const ListHook&) = delete;

    ListHook(ListHook&& other) = delete;
    ListHook& operator=(ListHook&&) = delete;

   private:
    ListHook* prev_ = this;
    ListHook* next_ = this;

   protected:
    ListHook() = default;

    ~ListHook() {
        Unlink();
    }

    template <class T>
    friend class List;
};

template <class T>
class List {
   public:
    class Iterator {
       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() : it_(nullptr) {
        }

        explicit Iterator(ListHook* it) : it_(it) {
        }

        Iterator(const Iterator& other) = default;

        Iterator(Iterator&& other) noexcept : it_(other.it_) {
            other.it_ = nullptr;
        }

        Iterator& operator=(const Iterator& other) {
            if (this == &other) {
                return *this;
            }
            it_ = other.it_;
            return *this;
        }

        Iterator& operator=(Iterator&& other) noexcept {
            if (this == &other) {
                return *this;
            }
            it_ = other.it_;
            other.it_ = nullptr;
            return *this;
        }

        ~Iterator() {
            it_ = nullptr;
        }

        Iterator& operator++() {
            it_ = it_->next_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            it_ = it_->prev_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        reference operator*() const {
            return *static_cast<T*>(it_);
        }

        pointer operator->() const {
            return static_cast<T*>(it_);
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

       private:
        ListHook* it_;
    };

    List() = default;

    List(const List&) = delete;

    List(List&& other) noexcept {
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_ = other.dummy_.next_;
        other.dummy_.prev_->next_ = &dummy_;
        other.dummy_.next_->prev_ = &dummy_;
        other.dummy_.prev_ = &other.dummy_;
        other.dummy_.next_ = &other.dummy_;
    }

    ~List() {
        while (dummy_.next_ != &dummy_) {
            dummy_.next_->Unlink();
        }
    }

    List& operator=(const List&) = delete;

    List& operator=(List&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        while (dummy_.next_ != &dummy_) {
            dummy_.next_->Unlink();
        }
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_ = other.dummy_.next_;
        other.dummy_.prev_->next_ = &dummy_;
        other.dummy_.next_->prev_ = &dummy_;
        other.dummy_.prev_ = &other.dummy_;
        other.dummy_.next_ = &other.dummy_;
        return *this;
    }

    [[nodiscard]] bool IsEmpty() const {
        return !dummy_.IsLinked();
    }

    [[nodiscard]] size_t Size() const {
        const auto* k = dummy_.next_;
        size_t count = 0;
        while (k != &dummy_) {
            k = k->next_;
            count++;
        }
        return count;
    }

    void PushBack(T* elem) {
        dummy_.prev_->LinkAfter(elem);
    }

    void PushFront(T* elem) {
        dummy_.LinkAfter(elem);
    }

    T& Front() {
        return *(static_cast<T*>(dummy_.next_));
    }

    [[nodiscard]] const T& Front() const {
        return *(static_cast<T*>(dummy_.next_));
    }

    T& Back() {
        return *(static_cast<T*>(dummy_.prev_));
    }

    [[nodiscard]] const T& Back() const {
        return *(static_cast<T*>(dummy_.prev_));
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }

    void PopFront() {
        dummy_.next_->Unlink();
    }

    Iterator Begin() {
        Iterator tmp(dummy_.next_);
        return tmp;
    }

    Iterator End() {
        Iterator tmp(&dummy_);
        return tmp;
    }

    Iterator IteratorTo(T* element) {
        Iterator tmp(static_cast<ListHook*>(element));
        return tmp;
    }

   private:
    ListHook dummy_;
};

template <class T>
List<T>::Iterator begin(List<T>& list) {
    return list.Begin();
}

template <class T>
List<T>::Iterator end(List<T>& list) {
    return list.End();
}

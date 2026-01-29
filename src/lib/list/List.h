#ifndef KITSUNEENGINE_LIST_H
#define KITSUNEENGINE_LIST_H

#include <cstddef>

template<typename T>
class List {
public:
    List() : data(nullptr), count(0), capacity(0) {}

    ~List() {
        delete[] data;
    }

    void Add(T item) {
        if (count >= capacity)
            Grow();
        data[count++] = item;
    }

    void RemoveAt(size_t index) {
        if (index >= count) return;
        for (size_t i = index; i + 1 < count; ++i)
            data[i] = data[i + 1];
        --count;
    }

    void Clear() {
        for (size_t i = 0; i < count; ++i)
            data[i] = nullptr;
        count = 0;
    }

    // Add specific for Events
    void ClearSubscriptions() {
        for (size_t i = 0; i < count; ++i)
            data[i] = T{};
        count = 0;
    }

    // ✅ Correct indexing
    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    size_t Size() const {
        return count;
    }

    T Get(int x) {
        return data[x];
    }

    const T& Get(int x) const {
        return data[x];
    }

    void PopBack() { if (count > 0) --count; }

    void Set(size_t index, const T& item) {
        if (index >= count) return;
        data[index] = item;
    }

private:
    T* data;
    size_t count;
    size_t capacity;

    void Grow() {
        capacity = capacity == 0 ? 4 : capacity * 2;
        T* newData = new T[capacity];
        for (size_t i = 0; i < count; ++i)
            newData[i] = data[i];
        delete[] data;
        data = newData;
    }
};

#endif

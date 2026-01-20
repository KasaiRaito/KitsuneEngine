#include "List.h"
#include <cstdlib>   // malloc, free
#include <cstring>   // memcpy

static constexpr size_t INITIAL_CAPACITY = 8;

List::List()
    : data(nullptr), count(0), capacity(0) {
    Grow();
}

List::~List() {
    free(data);
}

void List::Grow() {
    size_t newCapacity = (capacity == 0) ? INITIAL_CAPACITY : capacity * 2;

    void** newData = (void**)malloc(sizeof(void*) * newCapacity);

    if (data) {
        memcpy(newData, data, sizeof(void*) * count);
        free(data);
    }

    data = newData;
    capacity = newCapacity;
}

void List::Add(void* item) {
    if (count >= capacity) {
        Grow();
    }

    data[count++] = item;
}

void List::Remove(void* item) {
    for (size_t i = 0; i < count; ++i) {
        if (data[i] == item) {
            data[i] = data[count - 1]; // swap-remove
            count--;
            return;
        }
    }
}

void List::Clear() {
    count = 0;
}

void* List::Get(size_t index) const {
    if (index >= count) return nullptr;
    return data[index];
}

size_t List::Size() const {
    return count;
}

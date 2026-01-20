#ifndef KITSUNEENGINE_LIST_H
#define KITSUNEENGINE_LIST_H

#include <cstddef> // size_t

class List {
public:
    List();
    ~List();

    // Disable copy (engine containers usually shouldn’t copy)
    List(const List&) = delete;
    List& operator=(const List&) = delete;

    // Core operations
    void Add(void* item);
    void Remove(void* item);
    void Clear();

    void* Get(size_t index) const;
    size_t Size() const;

private:
    void** data;        // raw pointer array
    size_t count;
    size_t capacity;

    void Grow();
};

#endif // KITSUNEENGINE_LIST_H

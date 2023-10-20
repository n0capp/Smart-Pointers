# SmartPointers
Implementation of smart pointers in efficient way. Detailed description:

# Unique Pointer
Templated unique pointer that stores Compressed Pair inside. Compressed Pair implements [empty base optimization]([myLib/README.md](https://en.cppreference.com/w/cpp/language/ebo)https://en.cppreference.com/w/cpp/language/ebo) in a way not to store deleter or empty object (deleter usually doesn`t have fields, by default it's Slug, that calls operator delete on a heap)
# Intrusive Pointer
A pointer similar to Shared Pointer by its logic, we want to permit several pointers store the same object, and intrusive ptr obligates stored object to has functions that increment and decrement counter, counter is stored right inside the user tip.
# Shared Pointer and Weak Pointer
Implemented shared pointer with 1 allocation per MakeShared, it's reached by storing two control blocks (states that store object and counter). Also if user tip is derived from EnableSharedFromThis class, you don't have to build another shared from this shared, it's enough to build it from raw pointer.

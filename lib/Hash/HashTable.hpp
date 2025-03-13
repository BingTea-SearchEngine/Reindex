#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;
using std::cout;
using std::endl;

// You may add additional members or helper functions.

// Compare C-strings, return true if they are the same.
bool CompareEqual(const char* L, const char* R);

template <typename Key, typename Value>
class Tuple {
   public:
    Key key;
    Value value;

    Tuple(const Key& k, const Value v) : key(k), value(v) {}
};

template <typename Key, typename Value>
class Bucket {
   public:
    Bucket* next;
    uint32_t hashValue;
    Tuple<Key, Value> tuple;

    Bucket(const Key& k, uint32_t h, const Value v)
        : tuple(k, v), next(nullptr), hashValue(h) {}
};

template <typename Key, typename Value>
class HashTable {
   private:
    Bucket<Key, Value>** buckets;
    size_t numBuckets;
    size_t numNodes;

    friend class Iterator;
    friend class HashBlob;

   public:
    Tuple<Key, Value>* Find(const Key k, const Value initialValue) {
        // Search for the key k and return a pointer to the
        // ( key, value ) entry.  If the key is not already
        // in the hash, add it with the initial value.
        Tuple<Key, Value>* node = Find(k);

        if (node != nullptr) {
            return node;
        }

        //not found, add to hashmap,
        size_t hashedValue = HashTable::fnvHash(k, sizeof(k));
        size_t index = hashedValue % numBuckets;

        // Need to dynamically allocate because for a cstring we need it to live throughout the life time of Hashtable
        char* key = new char[strlen(k) + 1];
        strcpy(key, k);

        auto* newNode = new Bucket<Key, Value>(key, hashedValue, initialValue);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        ++numNodes;

        // load balance if necessary

        return &(newNode->tuple);
    }

    Tuple<Key, Value>* Find(const Key k) const {
        // Search for the key k and return a pointer to the
        // ( key, value ) entry.  If the key is not already
        // in the hash, return nullptr.
        size_t hashedValue = HashTable::fnvHash(k, sizeof(k));
        size_t index = hashedValue % numBuckets;

        // Look for key
        Bucket<Key, Value>* curr = buckets[index];
        while (curr) {
            if (strcmp(curr->tuple.key, k) == 0) {
                return &(curr->tuple);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    void Optimize(double loading = 1.5) { return; }

    // Your constructor may take as many default arguments
    // as you like.

    HashTable() {
        numNodes = 0;
        numBuckets = 8192;
        buckets = new Bucket<Key, Value>*[numBuckets + 1];
        for (size_t i = 0; i < numBuckets; ++i) {
            buckets[i] = nullptr;
        }
        buckets[numBuckets] =
            new Bucket<Key, Value>(Key(), numBuckets, Value());
    }

    ~HashTable() {
        for (size_t i = 0; i < numBuckets + 1; ++i) {
            auto node = buckets[i];
            while (node) {
                auto* temp = node;
                node = node->next;
                // Delete dynamically allocated key
                delete[] temp->tuple.key;
                // Delete dynamicall allocated bucket
                delete temp;
            }
        }
        delete[] buckets;
    }

    class Iterator {
       private:
        friend class HashTable;

        Bucket<Key, Value>* _ptr;
        const HashTable<Key, Value>* _table;
        size_t _currBucket;

        Iterator(const HashTable* table, size_t bucket, Bucket<Key, Value>* b) {
            _table = table;
            _currBucket = bucket;
            _ptr = b;
        }

       public:
        Iterator() : Iterator(nullptr, 0, nullptr) {}

        ~Iterator() {}

        Tuple<Key, Value>& operator*() { return _ptr->tuple; }

        // Compiler changes it->first to (it.operator->())->first;
        Tuple<Key, Value>* operator->() const { return &(_ptr->tuple); }

        Bucket<Key, Value> const* getBucket() { return _ptr; }

        // Prefix ++
        Iterator& operator++() {
            if (_ptr && _ptr->next) {
                _ptr = _ptr->next;
            } else {
                // End of bucket
                while (_table->buckets[++_currBucket] == nullptr) {}
                _ptr = _table->buckets[_currBucket];
            }
            return *this;
        }

        // Postfix ++
        Iterator operator++(int) {
            Iterator temp = *this;
            if (_ptr && _ptr->next) {
                _ptr = _ptr->next;
            } else {
                // End of bucket
                while (_table->buckets[++_currBucket] == nullptr) {}
                _ptr = _table->buckets[_currBucket];
            }
            return temp;
        }

        bool operator==(const Iterator& rhs) const { return _ptr == rhs._ptr; }

        bool operator!=(const Iterator& rhs) const { return _ptr != rhs._ptr; }
    };

    Iterator begin() const {
        auto it = Iterator(this, 0, buckets[0]);
        if (it._ptr == nullptr)
            ++it;
        return it;
    }

    Iterator end() const {
        return Iterator(this, numBuckets, buckets[numBuckets]);
    }

   private:
    // Requires
    //      data: pass in data by bytes
    //      length: length in bytes
    // Returns
    //      hash as size_t
    size_t static fnvHash(const char* data, size_t length) {
        static const size_t FnvOffsetBasis = 146959810393466560;
        static const size_t FnvPrime = 1099511628211ul;
        size_t hash = FnvOffsetBasis;
        for (size_t i = 0; *(data + i) != '\0'; ++i) {
            hash *= FnvPrime;
            hash ^= data[i];
        }
        return hash;
    }
};

// key -> T1
// hash_root(key) -> T2
// value -> T3
// Root stores a linked list of (key, value) pairs.
// Each level stores a hash table (vector of buckets),
// where each bucket is a vector of (key, value) pairs.

#include <iostream>
#include <vector>

using namespace std;

const int TOMBSTONE = -1;

class LinkedListNode {
public:
    int key;
    int value;
    LinkedListNode* next;

    LinkedListNode(int k, int v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

class LevelNode {
    vector<pair<int, int>> entries;

public:
    void myInsert(int key, int value) {
        entries.push_back({key, value});
    }

    int myLookup(int key) {
        for (auto &entry : entries) {
            if (entry.first == key)
                return entry.second;
        }
        return -1;
    }
};

class Level {
    vector<LevelNode> buckets;
    Level* nextLevel;
    int maxSize;
    int currSize;

    int myHash(int key) {
        return key % buckets.size();
    }

public:
    Level(int numBuckets, int maxEntries) {
        buckets.resize(numBuckets);
        maxSize = maxEntries;
        currSize = 0;
        nextLevel = nullptr;
    }

    void myInsert(int key, int value) {
        int bucket = myHash(key);
        buckets[bucket].myInsert(key, value);
        currSize++;
    }

    int myLookup(int key) {
        int bucket = myHash(key);
        return buckets[bucket].myLookup(key);
    }
};

class Root {
    LinkedListNode* head;
    int numNodes;
    int maxSize;
    Level* nextLevel;

    int myHash(int key) {
        return key % 8;
    }

public:
    Root(int maxNodes) {
        head = nullptr;
        numNodes = 0;
        maxSize = maxNodes;
        nextLevel = nullptr;
    }

    void setNextLevel(Level* level) {
        nextLevel = level;
    }

    void myInsert(int key, int value) {
        LinkedListNode* newNode = new LinkedListNode(key, value);

        newNode->next = head;
        head = newNode;

        numNodes++;

        if (numNodes >= maxSize)
            myFlush();
    }

    void myDelete(int key) {
        myInsert(key, TOMBSTONE);
    }

    int myLookup(int key) {
        LinkedListNode* curr = head;

        while (curr != nullptr) {
            if (curr->key == key)
                return curr->value;
            curr = curr->next;
        }

        if (nextLevel != nullptr)
            return nextLevel->myLookup(key);

        return -1;
    }

    void myFlush() {
        if (nextLevel == nullptr)
            return;

        LinkedListNode* curr = head;

        while (curr != nullptr) {
            nextLevel->myInsert(curr->key, curr->value);

            LinkedListNode* temp = curr;
            curr = curr->next;
            delete temp;
        }

        head = nullptr;
        numNodes = 0;
    }
};

class LSM {
    Root* treeRoot;

public:
    LSM() {
        treeRoot = new Root(4);

        Level* level1 = new Level(8, 100);
        treeRoot->setNextLevel(level1);
    }

    void myInsert(int key, int value) {
        treeRoot->myInsert(key, value);
    }

    void myDelete(int key) {
        treeRoot->myDelete(key);
    }

    int myLookup(int key) {
        return treeRoot->myLookup(key);
    }
};

int main() {
    LSM db;

    db.myInsert(1, 100);
    db.myInsert(2, 200);
    db.myInsert(3, 300);
    db.myInsert(4, 400);   // Triggers flush to Level 1

    cout << db.myLookup(2) << endl;   // 200

    db.myDelete(2);

    cout << db.myLookup(2) << endl;   // -1 (TOMBSTONE)

    return 0;
}
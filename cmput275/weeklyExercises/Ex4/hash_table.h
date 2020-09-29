#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "linked_list.h"
#include <cassert>

// forward declaration of HashTable class
template <typename T>
class HashTable;


template <typename T>
class HashTableIterator {
public:
  // constructor, need to know a pointer to the hash table we are iterating over
  // this initialize this iterator to the first entry of the table (if any)
  HashTableIterator(const HashTable<T>* hashTable);

  // no need for a destructor, we don't allocate dynamic memory here

  // signals if we are at the end
  bool atEnd() const;

  // post increment operator for the iterator, look it up on the
  // page on operator overloading linked from the lecture slides to see
  // why we use this specific syntax
  HashTableIterator<T> operator++(int);

  // get the item the iterator is looking at
  const T& operator*() const;

private:
  // the bucket we are currently examining, we are at the end of iteration
  // if and only if bucket == the number of buckets held by *tablePtr
  unsigned int bucket;

  // the table itself that we are iterating over
  const HashTable<T>* tablePtr;

  // the node in the list we are examining
  ListNode<T>* node;

  // advance the iterator
  void advance();
};

template <typename T>
HashTableIterator<T>::HashTableIterator(const HashTable<T>* hashTable) {
  bucket = 0;
  tablePtr = hashTable;
  node = tablePtr->table[0].getFirst();
  if (node == NULL) {
    // if the first bucket was empty, advance to the next item in the table
    advance();
  }
}

template <typename T>
bool HashTableIterator<T>::atEnd() const {
  return bucket == tablePtr->tableSize;
}

template <typename T>
HashTableIterator<T> HashTableIterator<T>::operator++(int) {
  HashTableIterator<T> copy = *this; // create a copy
  advance();
  return copy;
}

template <typename T>
const T& HashTableIterator<T>::operator*() const {
  assert(!atEnd());
  return node->item;
}

template <typename T>
void HashTableIterator<T>::advance() {
  assert(!atEnd());
  if (node != NULL)
    node = node->next;
  // if we advanced past the end of the list in this bucket,
  // then scan for the next nonempty bucket
  while (node == NULL && bucket < tablePtr->tableSize) {
    ++bucket;
    node = tablePtr->table[bucket].getFirst();
  }
}

/*
  A hash table for storing items. It is assumed the type T of the item
  being stored has a hash method, eg. you can call item.hash(), which
  returns an unsigned integer.

  Also assumes the != operator is implemented for the item being stored,
  so we could check if two items are different.

  If you just want store integers int for the key, wrap it up in a struct
  with a .hash() method and both == and != operator.
*/

template <typename T>
class HashTable {
  friend class HashTableIterator<T>; // now we can access the private variables of HashTable<T>

public:
  // creates an empty hash table with the given number of buckets. Default number of buckets: 10
  HashTable(unsigned int tableSize = 10);
  ~HashTable();

  // Check if the item already appears in the table.
  bool contains(const T& item) const;

  // Insert the item, do nothing if it is already in the table.
  // Returns true iff the insertion was successful (i.e. the item was not there).
  bool insert(const T& item);

  // Removes the item after checking, via assert, that the item was in the table.
  void remove(const T& item);

  unsigned int size() const;

private:
  LinkedList<T> *table; // start of the array of linked lists (buckets)
  unsigned int numItems; // # of items in the table
  unsigned int tableSize; // # of buckets

  // Computes the hash table bucket that the item maps into
  // by calling it's .hash() method.
  unsigned int getBucket(const T& item) const;
  unsigned int getBucket(const T& item, unsigned int newSize) const;

  // resizes current hash table
  void resize(unsigned int tableSize);
};


template <typename T>
HashTable<T>::HashTable(unsigned int tableSize) {
  // make sure there is at least one bucket
  assert(tableSize > 0);

  // calls the constructor for each linked list
  // so each is initialized properly as an empty list
  table = new LinkedList<T>[tableSize];

  // we are not storing anything
  numItems = 0;
  this->tableSize = tableSize;
}

template <typename T>
HashTable<T>::~HashTable() {
  // this will call the destructor for each linked
  // list before actually deleting this table from
  // the heap
  delete[] table;
}

template <typename T>
bool HashTable<T>::contains(const T& item) const {
  unsigned int bucket = getBucket(item);

  return table[bucket].find(item) != NULL;
}


// resizes current hash table
template <typename T>
void HashTable<T>::resize(unsigned int newSize) {
  // makes sure new size is at least 10
  if (newSize < 10) {
    newSize = 10;
  }

  // creates new hash table
  LinkedList<T> *newTable = new LinkedList<T>[newSize];

  // puts old hash table items into new hash table
  for (HashTableIterator<T> item(this); !item.atEnd(); item++) {
    unsigned int newBucket = (*item).hash() % newSize;
    newTable[newBucket].insertFront(*item);
  }

  // deletes old hash table and reassigns new hash table to the old one
  delete[] table;
  this->table = newTable;
  this->tableSize = newSize;
}


template <typename T>
bool HashTable<T>::insert(const T& item) {
  // if the item is here, return false
  if (contains(item)) {
    return false;
  }
  else {
    // otherwise, insert it into the front of the list
    // in this bucket and return true
    unsigned int bucket = getBucket(item);
    table[bucket].insertFront(item);
    ++numItems;

    // will resize hash table when there are more items than buckets
    // resizes hash table to half twice size
    if (numItems > tableSize) {
      resize(tableSize*2);
    }

    return true;
  }
}

template <typename T>
void HashTable<T>::remove(const T& item) {
  unsigned int bucket = getBucket(item);

  ListNode<T>* link = table[bucket].find(item);

  // make sure the item was in the list
  assert(link != NULL);

  table[bucket].removeNode(link);

  --numItems;

  // will resize hash table when there are 4 times more buckets than items
  // resizes hash table to half its size with min 10 buckets
  if (tableSize > 10 && tableSize > 4*numItems) {
    resize(tableSize/2);
  }
}

template <typename T>
unsigned int HashTable<T>::size() const {
  return numItems;
}


template <typename T>
unsigned int HashTable<T>::getBucket(const T& item) const {
  return item.hash() % tableSize;
}

#endif

#ifndef HASHTABLE_H

  #define HASHTABLE_H
  #define DEFAULT_HASH_TABLE_CAPACITY 512

  #include <stdlib.h>
  #include <string.h>

  typedef struct {

    void *value;
    size_t size;

  } Data;

  typedef struct Entry {

    Data *key;
    Data *value;
    struct Entry *next;

  } Entry;

  typedef struct {

    Entry **buckets;
    int size;
    int capacity;

  } HashTable; 

  HashTable *initialize_hash_table(int capacity);
  void destroy_hash_table(HashTable *hash_table);
  int expand_hash_table(HashTable *hash_table);
  int add_hash_table_entry(HashTable *hash_table, void *key, size_t key_size, void *value, size_t value_size);
  int remove_hash_table_entry(HashTable *hash_table, void *key, size_t key_size);
  int set_hash_table_entry(HashTable *hash_table, Entry *entry);
  Data *get_hash_table_entry(HashTable *hash_table, void *key, size_t key_size);

#endif

#include "hashtable.h"
#include <stdio.h>

unsigned long calc_hash(void *k, unsigned long size) {

  unsigned char *bytes = (unsigned char *) k;
  unsigned long result = 1;

  for (int i = 0; i < size; i++) {
    result = 31 * result + (char) bytes[i];
  }

  return result;
}

int equals(void *k1, unsigned long k1_size, void *k2, unsigned long k2_size) {

  if (k1_size != k2_size)
    return 0;

  return memcmp(k1, k2, k1_size) == 0;
}

Data *initialize_data(void *value, unsigned long size) {

  Data *data = malloc(sizeof(Data));

  if (data == NULL)
    return NULL;

  data->value = malloc(size);

  if (data->value == NULL) {
    free(data);
    return NULL;
  }

  memcpy(data->value, value, size);
  data->size = size;
  
  return data;
}

HashTable *initialize_hash_table(int capacity) {

  if (capacity <= 0)
    return NULL;

  HashTable *hash_table = malloc(sizeof(HashTable));

  if (hash_table == NULL)
    return NULL;

  Entry **buckets = calloc(capacity, sizeof(Entry *));

  if (buckets == NULL) {
    free(hash_table);
    return NULL;
  }

  hash_table->buckets = buckets;
  hash_table->capacity = capacity;
  hash_table->size = 0;

  return hash_table;
}

void destroy_hash_table(HashTable *hash_table) {

  if (hash_table == NULL)
    return;

  for (int i = 0; i < hash_table->capacity; i++) {

    Entry *entry = hash_table->buckets[i];

    while(entry != NULL) {

      Entry *next = entry->next;

      free(entry->key->value);
      free(entry->key);
      free(entry->value->value);
      free(entry->value);
      free(entry);
      
      entry = next;
    }

  }

  free(hash_table->buckets);
  free(hash_table);
}

int expand_hash_table(HashTable *hash_table) {

  if (hash_table == NULL)
    return -1;

  Entry **buckets = calloc(hash_table->capacity * 2, sizeof(Entry *));

  if (buckets == NULL)
    return -1;

  int old_capacity = hash_table->capacity;
  Entry **old_buckets = hash_table->buckets;
  hash_table->capacity *= 2;
  hash_table->buckets = buckets;

  for (int i = 0; i < old_capacity; i++) {

    Entry *entry = old_buckets[i];

    while (entry != NULL) {

      Entry *next = entry->next;

      set_hash_table_entry(hash_table, entry); 

      entry = next;
    }

  }

  free(old_buckets);
  return 0;
}

int remove_hash_table_entry(HashTable *hash_table, void *key, unsigned long key_size) {

  if (hash_table == NULL || key == NULL || key_size <= 0)
    return -1;

  unsigned long hash = calc_hash(key, key_size);
  int index = hash % hash_table->capacity;
  Entry *old_entry = NULL;
  Entry *entry = hash_table->buckets[index];

  while (entry != NULL) {
    
    if (equals(key, key_size, entry->key->value, entry->key->size)) {

      if (old_entry == NULL) {
        hash_table->buckets[index] = entry->next;
      }else {
        old_entry->next = entry->next;
      }

      free(entry->key->value);
      free(entry->key);
      free(entry->value->value);
      free(entry->value);
      free(entry);
      hash_table->size--;
      return 0;
    }

    old_entry = entry;
    entry = entry->next;
  }

  return -1;
}

int add_hash_table_entry(HashTable *hash_table, void *key, unsigned long key_size, void *value, unsigned long value_size) {

  if (hash_table == NULL || key == NULL || value == NULL || key_size <= 0 || value_size <= 0)
    return -1;

  Entry *entry = malloc(sizeof(Entry));

  if (entry == NULL)
    return -1;

  Data *key_data = initialize_data(key, key_size);

  if (key_data == NULL) {
    free(entry);
    return -1;
  }

  Data *value_data = initialize_data(value, value_size);

  if (value_data == NULL) {
    free(key_data);
    free(entry);
    return -1;
  }

  entry->key = key_data;
  entry->value = value_data;

  return set_hash_table_entry(hash_table, entry);
}

int set_hash_table_entry(HashTable *hash_table, Entry *entry) {

  entry->next = NULL;

  if (hash_table == NULL || entry == NULL)
    return -1;

  if (hash_table->size >= hash_table->capacity)
    expand_hash_table(hash_table);

  unsigned long hash = calc_hash(entry->key->value, entry->key->size);
  int index = hash % hash_table->capacity;

  Entry *entry_in_bucket = hash_table->buckets[index];

  if (entry_in_bucket == NULL) {
    hash_table->buckets[index] = entry;
  }else {

    while (entry_in_bucket->next != NULL) {
      entry_in_bucket = entry_in_bucket->next;
    }

    entry_in_bucket->next = entry;
  }

  hash_table->size++;
  return 0;
}

Data *get_hash_table_entry(HashTable *hash_table, void *key, unsigned long key_size) {

  if (hash_table == NULL || key == NULL || key_size <= 0)
    return NULL;

  unsigned long hash = calc_hash(key, key_size);
  int index = hash % hash_table->capacity;

  Entry *entry = hash_table->buckets[index];

  while (entry != NULL) {
    if (equals(key, key_size, entry->key->value, entry->key->size)) {
      return entry->value;
    }
  }

  return NULL;
}

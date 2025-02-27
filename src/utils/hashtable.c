#include "hashtable.h"

int calc_hash(void *k, int size) {

  unsigned char *bytes = (unsigned char *) k;
  unsigned int result = 1;

  for (int i = 0; i < size; i++) {
    result = 31 * result + bytes[i];
  }

  return result;
}

int equals(void *k1, int k1Size, void *k2, int k2Size) {

  if (k1Size != k2Size)
    return -1;

  unsigned char *k1Bytes = (unsigned char *) k1;
  unsigned char *k2Bytes = (unsigned char *) k2;

  for (int i = 0; i < k1Size; i++) {
    if (k1Bytes[i] != k2Bytes[i])
      return -1;
  }

  return 0;
}

HashTable *initialize_hash_table(int capacity) {

  HashTable *hashTable = malloc(sizeof(HashTable));

  if (hashTable == NULL)
    return NULL;

  Entry **buckets = calloc(capacity, sizeof(Entry *));

  if (buckets == NULL) {
    free(hashTable);
    return NULL;
  }

  hashTable->buckets = buckets;
  hashTable->capacity = capacity;
  hashTable->size = 0;

  return hashTable;
}

void destroy_hash_table(HashTable *hashTable) {

  if (hashTable == NULL)
    return;

  for (int i = 0; i < hashTable->capacity; i++) {

    Entry *entry = hashTable->buckets[i];

    while(entry != NULL) {

      Entry *next = entry->next;
      free(entry);
      
      entry = next;
    }

  }

  free(hashTable->buckets);
  free(hashTable);
}

int expand_hash_table(HashTable *hashTable) {

  if (hashTable == NULL)
    return -1;

  Entry **buckets = calloc(hashTable->capacity * 2, sizeof(Entry *));

  if (buckets == NULL)
    return -1;

  int oldCapacity = hashTable->capacity;
  Entry **oldBuckets = hashTable->buckets;
  hashTable->capacity *= 2;
  hashTable->buckets = buckets;

  for (int i = 0; i < oldCapacity; i++) {

    Entry *entry = oldBuckets[i];

    while (entry != NULL) {

      Entry *next = entry->next;
      entry->next = NULL;

      set_hash_table_entry(hashTable, entry); 

      entry = next;
    }

  }

  free(oldBuckets);
  return 0;
}

int remove_hash_table_entry(HashTable *hashTable, void *key, int keySize) {

  if (hashTable == NULL)
    return -1;

  int hash = calc_hash(key, keySize);
  int index = hash % hashTable->capacity;
  Entry *oldEntry = NULL;
  Entry *entry = hashTable->buckets[index];

  while (entry != NULL) {
    
    if (equals(key, keySize, entry->key, entry->keySize)) {

      if (oldEntry == NULL) {
        hashTable->buckets[index] = entry->next;
      }else {
        oldEntry->next = entry->next;
      }

      free(entry);
      return 0;
    }

    oldEntry = entry;
    entry = entry->next;
  }

  return -1;
}

int add_hash_table_entry(HashTable *hashTable, void *key, int keySize, void *value) {

  if (hashTable == NULL)
    return -1;

  Entry *entry = malloc(sizeof(Entry));

  if (entry == NULL)
    return -1;

  entry->key = key;
  entry->keySize = keySize;
  entry->value = value;
  entry->next = NULL;

  set_hash_table_entry(hashTable, entry);

  return 0;
}

int set_hash_table_entry(HashTable *hashTable, Entry *entry) {

  if (hashTable == NULL || entry == NULL)
    return -1;

  if (hashTable->size >= hashTable->capacity)
    expand_hash_table(hashTable);

  int hash = calc_hash(entry->key, entry->keySize);
  int index = hash % hashTable->capacity;

  Entry *entryInBucket = hashTable->buckets[index];

  if (entryInBucket == NULL) {
    hashTable->buckets[index] = entry;
  }else {

    while (entryInBucket->next != NULL) {
      entryInBucket = entryInBucket->next;
    }

    entryInBucket->next = entry;
  }

  return 0;
}

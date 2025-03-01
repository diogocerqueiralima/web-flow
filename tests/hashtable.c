#include "../src/utils/hashtable.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void initialize_and_destroy_hash_table_should_succeed() {

  int capacity = 16;
  HashTable *hashTable = initialize_hash_table(capacity);

  assert(hashTable != NULL);

  destroy_hash_table(hashTable);

  printf("✔ initialize and destroy hash table\n");
}

void add_entry_to_hash_table_should_succeed() {

  HashTable *hashTable = initialize_hash_table(DEFAULT_HASH_TABLE_CAPACITY);

  char *key = "name";
  char *value = "Diogo";

  int success = add_hash_table_entry(hashTable, key, strlen(key), value, strlen(value));

  assert(success == 0);

  destroy_hash_table(hashTable);

  printf("✔ add entry to hash table\n");
}

void get_entry_from_hash_table_should_succeed() {

  HashTable *hashTable = initialize_hash_table(DEFAULT_HASH_TABLE_CAPACITY);

  char *key = "name";
  char *value = "Diogo";

  add_hash_table_entry(hashTable, key, strlen(key), value, strlen(value));
  Data *data = get_hash_table_entry(hashTable, key, strlen(key));

  assert(memcmp(value, data->value, strlen(value)) == 0);

  destroy_hash_table(hashTable);

  printf("✔ get entry from hash table\n");
}

int main() {

  initialize_and_destroy_hash_table_should_succeed();
  add_entry_to_hash_table_should_succeed();
  get_entry_from_hash_table_should_succeed();

  return 0;
}

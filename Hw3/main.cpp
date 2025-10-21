#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

using namespace std;

struct Slot {
  uint32_t tag;
  bool valid, dirty;
  uint32_t load_ts, access_ts;
};

struct Set {
  vector<Slot> slots;
};

struct Cache {
  vector<Set> sets;
};

uint32_t global_time = 0;

//Helper Functions
int valid_parameters(int num_sets, int num_blocks, int num_bytes, string write_allocate_action, string write_through_or_back) {

  if (num_bytes < 4 || num_sets <= 0 || num_blocks <= 0) {
    std::cerr << "The block size is less than 4 or non-postive values for either number of sets or number of blocks were given" << endl;
    return 1;
  }

  if ((num_sets & (num_sets - 1)) != 0 || (num_blocks & (num_blocks - 1) != 0) || (num_bytes & (num_bytes - 1) != 0)) {
    std::cerr << "The number of sets in the cache or the number of blocks in each set or the number bytes in each block is not a power of 2" << endl;
    return 1;
  }

  if (write_allocate_action == "no-write-allocate" && write_through_or_back == "write-back") {
    std::cerr << "Won't be able to actually write to the cache for the store" << endl;
    return 1;
  }

  return 0;
}

void check_combinations(int num_sets, int num_blocks) {
  if (num_blocks == 1) {
    //direct-mapped
  } else if (num_sets == 1) {
    //fully associative
  } else {
    //set-associative
  }
}

/*goal of the function: process each of the access or loads, calculate and return process cycles to 
perform operation.
*/
int process_trace(Cache &cache, char operation, uint32_t address, int num_sets, int num_blocks, 
  int num_bytes, string write_allocate_action, string write_through_or_back, string eviction) {

  int block_offset_size = log2(num_bytes); // the lowest n bits 
  int index_size = log2(num_sets); // the next n bits

  // extract the lowest n bits
  uint32_t block_offset_isolate = (1 << block_offset_size) - 1; // Isolate the bottom n bits
  uint32_t block_offset = address & block_offset_isolate;

  uint32_t index_isolate = (1 << index_size) - 1; // Isolate the next n bits
  uint32_t index = (address >> block_offset_size) & index_isolate;

  uint32_t tag = address >> (block_offset_size + index_size);

  Set &set = cache.sets[index];
  int i = 0; 
  bool found = false;
  while (i < set.slots.size()) {
    if (set.slots[i].tag == tag) {
      found = true;
      break;
    }
    i++;
  }

  if (found) {
    /*
    found for load: perform tag updates 
    */
  } else {
    /*if not found, then different things to do depending on operation:
    if not found for a load then it'll be 100 p cycles for every 4 bytes for main memeroy access,
    then load into cache. break into subfunctions. function for loading into cache taking parameters 
    (cache, set index, slot index). so we can just call it rather than putting in this body

    if not found for a store then.. 
      if no write allcoate then just don't modify anything and it falls to the wayside
      if write allocate then access memory into the cache and then store (100 + 1) per 4 bytes store operation
    
    */
  }
  //default value for processor cycles for now
  return 0;
}

Cache create_cache(int num_sets, int num_blocks) {
  Cache cache;
  cache.sets.resize(num_sets); // creates num_sets sets

  for (int i = 0; i < num_sets; i++) {
    cache.sets[i].slots.resize(num_blocks); // each set has num_blocls blocks/slots
    for (int j = 0; j < num_blocks; j++) {
      cache.sets[i].slots[j].valid = true;
      cache.sets[i].slots[j].dirty = false;
      cache.sets[i].slots[j].tag = 0;
      cache.sets[i].slots[j].load_ts = 0;
      cache.sets[i].slots[j].access_ts = true;
    }
  }
  return cache;
}

int main( int argc, char **argv ) {
  // TODO: implement

  if(argc != 7) { 
    return 1;
  }

  int num_sets = stoi(argv[1]); // Extract the number of sets in the cache
  int num_blocks = stoi(argv[2]); // Extract the number of blocks in the cache
  int num_bytes = stoi(argv[3]); //Extract the number of bytes in each block

  string write_allocate_action = argv[4];
  string write_through_or_back = argv[5];
  string eviction = argv[6];

  // checking parameters
  int check_parameters = valid_parameters(num_sets, num_blocks, num_bytes, write_allocate_action, write_through_or_back);
  if (check_parameters == 1) return 1;

  // Checking combinations
  check_combinations(num_sets, num_blocks);

  // Create cache
  Cache cache = create_cache(num_sets, num_blocks);

  // Process memory access trace

  char operation;
  uint32_t address;
  int process_cycles = 0;
  // Extract the 'l' or 's' operation and the address
  while (cin >> operation >> hex >> address) {
    process_cycles += process_trace(cache, operation, address, num_sets, num_blocks, num_bytes, 
                  write_allocate_action, write_through_or_back, eviction);
  }

  return 0;
}


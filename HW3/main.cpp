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
uint32_t total_loads = 0;
uint32_t total_stores = 0;
uint32_t load_hits = 0;
uint32_t load_misses = 0;
uint32_t store_hits = 0;
uint32_t store_misses = 0;
uint32_t total_cycles = 0;


//Helper Functions
int valid_parameters(int num_sets, int num_blocks, int num_bytes, string write_allocate_action, string write_through_or_back) {

  if (num_bytes < 4 || num_sets <= 0 || num_blocks <= 0) {
    std::cerr << "The block size is less than 4 or non-postive values for either number of sets or number of blocks were given" << endl;
    return 1;
  }

  if ((num_sets & (num_sets - 1)) != 0 || (num_blocks & (num_blocks - 1)) != 0 || (num_bytes & (num_bytes - 1)) != 0) {
    std::cerr << "The number of sets in the cache or the number of blocks in each set or the number bytes in each block is not a power of 2" << endl;
    return 1;
  }

  if (write_allocate_action == "no-write-allocate" && write_through_or_back == "write-back") {
    std::cerr << "Won't be able to actually write to the cache for the store" << endl;
    return 1;
  }

  return 0;
}

int find_lru(Set &set) {
  int index = 0;
  uint32_t current_oldest = set.slots[0].access_ts;
  for (uint32_t i = 1; i < set.slots.size(); i++ ) {
    if(set.slots[i].access_ts < current_oldest) {
      current_oldest = set.slots[i].access_ts;
      index = i;
    }
  }
  return index;
}

int find_fifo(Set &set) {
  // the block that was loaded in the first is the one with the largest load_ts
  int index = 0;
  uint32_t current_oldest = set.slots[0].load_ts;
  for (uint32_t i = 1; i < set.slots.size(); i++ ) {
    if(set.slots[i].load_ts < current_oldest) {
      current_oldest = set.slots[i].load_ts;
      index = i;
    }
  }
  return index;
}

void load_found(Set &set, uint32_t slot_index) {
  set.slots[slot_index].access_ts = global_time;
  total_cycles++;
}

void load_miss(Set &set, uint32_t tag, int num_bytes, string wt_or_wb, string eviction) {
  //evictions
  //total_cycles += (num_bytes / 4) * 100; 


  int empty_slot = -1;
  for (uint32_t i = 0; i < set.slots.size(); i++) {
    if (!set.slots[i].valid) {
      empty_slot = i;
      break;
    }
  }

  if (empty_slot == -1) {
    // Have to evict
    if (eviction == "fifo") {
      empty_slot = find_fifo(set);
    } else {
      empty_slot = find_lru(set);
    }
    Slot &victim = set.slots[empty_slot];

    if (victim.valid && victim.dirty && wt_or_wb == "write-back") {
      total_cycles += (num_bytes / 4) * 100;
    }
  }

  total_cycles += (num_bytes / 4) * 100;

  Slot &slot = set.slots[empty_slot];

  slot.tag = tag;
  slot.valid = true;
  slot.dirty = false;
  slot.load_ts = global_time;
  slot.access_ts = global_time;
  total_cycles++; /*final cache access*/
}

void store_found(Set &set, uint32_t slot_index, string wt_or_wb) {
  Slot &slot = set.slots[slot_index];
  slot.access_ts = global_time;
  total_cycles++;
  if (wt_or_wb == "write-through") /*write-through*/ {
    total_cycles += 100;
    slot.dirty = false;
  } else {
    slot.dirty = true;
  }
}

void store_miss(Set &set, int num_bytes, string nw_or_wa, string wt_or_wb, uint32_t tag, string eviction) {
  if (nw_or_wa == "no-write-allocate") {
    total_cycles += 100;
  } else {
    int slot_to_replace = -1;

    for (uint32_t i = 0; i < set.slots.size(); i++) {
      if (!set.slots[i].valid) {
        slot_to_replace = i;
        break;
      }
    }

    if (slot_to_replace == -1) {
      if (eviction == "fifo") {
         slot_to_replace = find_fifo(set);
      } else {
        slot_to_replace = find_lru(set);
      }
     
      Slot &pop = set.slots[slot_to_replace];

      if (pop.valid && pop.dirty && wt_or_wb == "write-back") {
        total_cycles += (num_bytes / 4) * 100;
      }
    }

    total_cycles += (num_bytes / 4) * 100;

    Slot &n_slot = set.slots[slot_to_replace];
    n_slot.tag = tag;
    n_slot.valid = true;
    n_slot.load_ts = global_time;
    n_slot.access_ts = global_time;

    total_cycles++;

    if (wt_or_wb == "write-through") {
      total_cycles += 100;
      n_slot.dirty = false;
    } else {
      n_slot.dirty = true;
    }
  }
}


void update_counters(Set &set, char operation, int hit_slot, string write_through_or_back, string write_allocate_action, uint32_t tag, int num_bytes, string eviction) {
  if (operation == 'l') { 
    total_loads++;
    if (hit_slot != -1) {
      load_hits++;
      load_found(set, hit_slot);
    } else {
      load_misses++;
      load_miss(set, tag, num_bytes, write_through_or_back, eviction);    
    }
  } else if (operation == 's') {
    total_stores++;
    if (hit_slot != -1) {
      store_hits++;
      store_found(set, hit_slot, write_through_or_back);
    } else {
      store_misses++;
      store_miss(set, num_bytes, write_allocate_action, write_through_or_back, tag, eviction);
    }
  } 
}

/*goal of the function: process each of the access or loads, calculate and return process cycles to 
perform operation.
*/
int process_trace(Cache &cache, char operation, uint32_t address, int num_sets, 
  int num_bytes, string write_allocate_action, string write_through_or_back, string eviction) {

  int block_offset_size = log2(num_bytes); // the lowest n bits 
  int index_size = log2(num_sets); // the next n bits

  // extract the lowest n bits
  //uint32_t block_offset_isolate = (1 << block_offset_size) - 1; // Isolate the bottom n bits
  //uint32_t block_offset = address & block_offset_isolate;

  uint32_t index_isolate = (1 << index_size) - 1; // Isolate the next n bits
  uint32_t index = (address >> block_offset_size) & index_isolate;

  uint32_t tag = address >> (block_offset_size + index_size);

  Set &set = cache.sets[index];
  uint32_t i = 0; int hit_slot = -1;
  while (i < set.slots.size()) {
    if (set.slots[i].valid && set.slots[i].tag == tag) {
      hit_slot = i;
      break;
    }
    i++;
  }

  global_time++;

  // Tobi this handles the scenrios you were describing with if (found), some of the functions you outlined for the different scenarios are being called here
  update_counters(set, operation, hit_slot, write_through_or_back, write_allocate_action, tag, num_bytes, eviction);

  return 0;
}

Cache create_cache(int num_sets, int num_blocks) {
  Cache cache;
  cache.sets.resize(num_sets); // creates num_sets sets

  for (int i = 0; i < num_sets; i++) {
    cache.sets[i].slots.resize(num_blocks); // each set has num_blocls blocks/slots
    for (int j = 0; j < num_blocks; j++) {
      cache.sets[i].slots[j].valid = false; //cache starts empty so every block should be invalid?
      cache.sets[i].slots[j].dirty = false; 
      cache.sets[i].slots[j].tag = 0;
      cache.sets[i].slots[j].load_ts = 0;
      cache.sets[i].slots[j].access_ts = 0;
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

  // Create cache
  Cache cache = create_cache(num_sets, num_blocks);

  // Process memory access trace

  char operation;
  uint32_t address;
  string unused;
  //int process_cycles = 0;
  // Extract the 'l' or 's' operation and the address
  while (cin >> operation >> hex >> address >> unused) {
    process_trace(cache, operation, address, num_sets, num_bytes, 
                  write_allocate_action, write_through_or_back, eviction);
  }

  printf("Total Loads: %d\n", total_loads);
  printf("Total Stores: %d\n", total_stores);
  printf("Load Hits: %d\n", load_hits);
  printf("Load Misses: %d\n", load_misses);
  printf("Store Hits: %d\n", store_hits);
  printf("Store Misses: %d\n", store_misses);
  printf("Total Cycles: %d\n", total_cycles);

  return 0;
}
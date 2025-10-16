#include <iostream>

#include <string>
using namespace std;


//Helper Functions
int valid_parameters(int num_sets, int num_blocks, int num_bytes, string write_allocate_action, string write_through_or_back) {

  if (num_bytes < 4 || num_sets <= 0 || num_blocks <= 0) {
    std::cerr << "The block size is less than 4 or non-postive values for either number of sets or number of blocks were given";
    return 1;
  }

  if ((num_sets & (num_sets - 1)) != 0 || (num_blocks & (num_blocks - 1) != 0) || (num_bytes & (num_bytes - 1) != 0)) {
    std::cerr << "The number of sets in the cache or the number of blocks in each set or the number bytes in each block is not a power of 2";
    return 1;
  }

  if (write_allocate_action == "no-write-allocare" && write_through_or_back == "write-back") {
    std::cerr << "Won't be able to actually write to the cache for the store";
    return 1;
  }

  return 0;
}

void check_combinations(int num_sets, int num_blocks) {
  if (num_blocks = 1) {
    //direct-mapped
  } else if (num_sets == 1) {
    //ully associative
  } else {
    //set-associative
  }
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




  return 0;
}


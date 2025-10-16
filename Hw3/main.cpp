#include <iostream>

#include <string>
using namespace std;

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

  // Checking combinations
  check_combinations(num_sets, num_blocks);




  return 0;
}

int valid_parameters(int num_sets, int num_blocks, int num_bytes, string write_allocate_action, string write_through_or_back, string eviction) {

  if (num_bytes < 4 || num_sets < 0 || num_blocks < 0) {
    std::cerr << "The block size is less than 4 or non-postive values for either number of sets or number of blocks";
    return 1;

    
  }
  
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

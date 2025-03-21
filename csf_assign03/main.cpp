#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include <limits>

using namespace std;

//structure for cache block
struct Block {
  uint32_t tag;
  bool valid, dirty;
  uint32_t load_ts, access_ts; //can either be LRU or FIFO
};

struct Set {
  vector<Block> blocks;
};

class Cache {
public:
  vector<Set> sets;
  int num_sets;
  int num_blocks;
  int block_size;
  bool write_allocate; // true: write-allocate, false: no-write-allocate
  bool write_through; // true: write-through, false: write-back
  bool lru_eviction; // true: LRU, false: FIFO
  uint32_t timestamp; //global simulated timestamp


  //create an empty cache with the given specifications
  Cache(int num_sets, int num_blocks, int block_size,
    bool write_allocate, bool write_through, bool lru_eviction)
  : num_sets(num_sets), num_blocks(num_blocks), block_size(block_size),
    write_allocate(write_allocate), write_through(write_through),
    lru_eviction(lru_eviction), timestamp(0)
  {
    sets.resize(num_sets);
    for (int i = 0; i < num_sets; i++){
      //Initialize each set with a vector of blocks
      sets[i].blocks.resize(num_blocks);
      for (int j = 0; j < num_blocks; j++) {
        sets[i].blocks[j].valid = false;
        sets[i].blocks[j].dirty = false;
        sets[i].blocks[j].tag = 0;
        sets[i].blocks[j].load_ts = 0;
        sets[i].blocks[j].access_ts = 0;
      }
    }
  }

  //access a single memory address
  void access(char op, uint32_t address, int &hit_count, int &miss_count, int &eviction_count) {
    //increment timestamp for every access
    timestamp++;

    int b = log2(block_size);
    int s = log2(num_sets);
    uint32_t tag = address >> (s + b); //isolate the tag part of memory address
    uint32_t set_index = (address >> b) & ((1 << s) - 1);

    Set &set = sets[set_index];
    bool hit = false;
    int hit_index = -1;
    int empty_index = -1;

    //search set for a hit
    for (int i = 0; i < num_blocks; ++i) {
      Block &block = set.blocks[i];
      if (block.valid && block.tag == tag) {
          hit = true;
          hit_index = i;
          break;
      }
      if (!block.valid && empty_index == -1) {
        empty_index = i;
      }
    }

    if (hit) {
      hit_count++;
      //for LRU, update access timestamp on a hit
      if (lru_eviction) {
        set.blocks[hit_index].access_ts = timestamp;
      }
      //on a store, mark block dirty if using write-back
      if (op == 's' && !write_through) {
        set.blocks[hit_index].dirty = true;
      }
    } else {
      miss_count++;
      int target_index;
      //use empty block if there is one
      if (empty_index != -1) {
        target_index = empty_index;
      } else {
        //if no empty block, evict a block
        target_index = 0;
        uint32_t candidate_ts = numeric_limits<uint32_t>::max();
        //if FIFO, use load_ts; if LRU, use access_ts
        for (int i = 0; i < num_blocks; ++i){
          Block &block = set.blocks[i];
          uint32_t time_val = lru_eviction ? block.load_ts : block.access_ts;
          if (time_val < candidate_ts) {
            candidate_ts = time_val;
            target_index = i;
          }
        }
        eviction_count++;
        //TODO: handle dirty eviction here for write-back caches
      }

      //load new block into cache
      Block &target = set.blocks[target_index];
      target.valid = true;
      target.tag = tag;
      target.dirty = false; //assume clean on load
      target.load_ts = timestamp;
      target.access_ts = timestamp;
      if (op == 's' && !write_through) {
        target.dirty = true;
      }
    }
  }

private:
  //Helper function compute integer log base 2 of x
  int log2(int x) {
    int result = 0;
    while (x >>= 1) {
      result++;
    }
    return result;
  }
};

int main( int argc, char **argv ) {
  if (argc != 7) {
    cerr << "Usage: " << argv[0]
         << " <num_sets> <num_blocks> <block_size> <write_allocate (0/1)> "
            "<write_through (0/1)> <eviction_policy (lru/fifo)>" << endl;
    return EXIT_FAILURE;
  }

  int num_sets = atoi(argv[1]);       // Should be a power of 2.
  int num_blocks = atoi(argv[2]);   // Should be a power of 2.
  int block_size = atoi(argv[3]);       // Power of 2, at least 4.
  bool write_allocate = (atoi(argv[4]) == 1);
  bool write_through = (atoi(argv[5]) == 1);
  bool lru_eviction = false;         // true for LRU, false for FIFO.
  
  string policy(argv[6]);
  if (policy == "lru") {
    lru_eviction = true;
  } else if (policy == "fifo") {
    lru_eviction = false;
  } else {
    cerr << "Invalid eviction policy. Use 'lru' or 'fifo'." << endl;
    return EXIT_FAILURE;
  }
  
  Cache cache(num_sets, num_blocks, block_size, write_allocate, write_through, lru_eviction);
  int hit_count = 0, miss_count = 0, eviction_count = 0;
  char op;
  uint32_t address;
  int val;

  while (std::cin >> op >> std::hex >> address >> std::dec >> val) {
    cache.access(op, address, hit_count, miss_count, eviction_count);
  }

  std::cout << "Hit count: " << hit_count << "\n Miss count: " << miss_count << "\n Eviction count: " << eviction_count << std::endl;
  
  return 0;
}

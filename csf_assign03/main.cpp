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

//structure for cache set
struct Set {
  vector<Block> blocks;
};

//structure for cache
class Cache {
public:
  vector<Set> sets;
  int num_sets;         
  int num_blocks;       
  int block_size;       
  bool write_allocate;  
  bool write_through;   
  bool lru_eviction;    
  uint32_t timestamp;   //global simulated timestamp


  /**
   * @brief        Initialize a new cache instance
   * 
   * @param[in]    num_sets       Number of sets in this cache instance
   * @param[in]    num_blocks     Number of blocks in each set
   * @param[in]    block_size     Number of bytes in each cache block
   * @param[in]    write_allocate True: write-allocate, False: no-write-allocate
   * @param[in]    write_through  True: write-through, False: write-back
   * @param[in]    lru_eviction   True: LRU, False: FIFO
   * 
   * @return       Description of what the function returns.
   * 
   * @note         timestamp is automatically initialized to 0.
   * @warning      num_sets, num_blocks, block_size should all be positive powers-of-2, block size is no smaller than 4
   */
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

  /**
   * @brief        Simulate a load request to the cache
   * 
   * @param[in]    address        address that's to be used to find tag and index
   * @param[in]    load_hits      number of load hits upon function call
   * @param[in]    load_misses    number of load misses upon function call
   * @param[in]    store_hits     number of store hits upon function call
   * @param[in]    store_misses   number of store misses upon function call
   * @param[in]    total_cycles   number of cycles upon function call
   * 
   * @note         timestamp is unconditionally incremented by 1.
   */ 
  void load(uint32_t address, int &load_hits, int &load_misses, int &store_hits, int &store_misses, int &total_cycles){
    timestamp++;
    //isolate the index of the set and the tag
    int b = log2(block_size);
    int s = log2(num_sets);
    uint32_t tag = address >> (s + b); //isolate the tag part of memory address
    uint32_t set_index = (address >> b) & ((1 << s) - 1);

    //initialize values to search for a hit
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

    if(hit){
      //yay we hit lmfao
      load_hits++;      
      if (lru_eviction) {
          set.blocks[hit_index].access_ts = timestamp; // Update access timestamp for LRU
      }
    } else { //rip we missed
      total_cycles += 25 * block_size; 
      load_misses++;
      // There isn't empty index space that exists
      if (empty_index == -1) {
            int lru_index = 0;
            uint32_t lru_ts = set.blocks[0].access_ts; 

            // Find the block that LRU
            for (int i = 1; i < num_blocks; ++i) {
                if (set.blocks[i].access_ts < lru_ts) {
                    lru_index = i;
                    lru_ts = set.blocks[i].access_ts;
                }
            }

            // Evict the LRU block
            Block &evicted_block = set.blocks[lru_index];
            if (evicted_block.dirty) {
                // Write back to memory if the block is dirty (write-back policy)
                total_cycles += 25*block_size; // Assume 100 cycles to write back to memory
            }

            // Replace the evicted block with the new block
            evicted_block.valid = true;
            evicted_block.tag = tag;
            evicted_block.dirty = false; // New block is clean on load
            evicted_block.load_ts = timestamp;
            evicted_block.access_ts = timestamp;
        } else {
            // Use the empty block that exists alreayd
            Block &target = set.blocks[empty_index];
            target.valid = true;
            target.tag = tag;
            target.dirty = false; 
            target.load_ts = timestamp;
            target.access_ts = timestamp;

        }
    }
    total_cycles++; 



  }


  /**
   * @brief        Simulate a store request to the cache
   * 
   * @param[in]    address        address that's to be used to find tag and index
   * @param[in]    load_hits      number of load hits upon function call
   * @param[in]    load_misses    number of load misses upon function call
   * @param[in]    store_hits     number of store hits upon function call
   * @param[in]    store_misses   number of store misses upon function call
   * @param[in]    total_cycles   number of cycles upon function call
   * 
   * @note         timestamp is unconditionally incremented by 1.
   */ 
  void store(uint32_t address, int &load_hits, int &load_misses, int &store_hits, int &store_misses, int &total_cycles){
    timestamp++;
    //isolate the index of the set and the tag
    int b = log2(block_size);
    int s = log2(num_sets);
    uint32_t tag = address >> (s + b); //isolate the tag part of memory address
    uint32_t set_index = (address >> b) & ((1 << s) - 1);

    //initialize values to search for a hit
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

    if(hit){
      store_hits++;
      if (lru_eviction) {
          set.blocks[hit_index].access_ts = timestamp; // Update access timestamp for LRU
      }
      total_cycles++;
      //if write back make the block dirty
      if(!write_through){
        set.blocks[hit_index].dirty = true;
      }
      else{
        //increment by 100 if we are using write back as 
        total_cycles+= 100;
      }

    }
    //OOPS I MISSED
    else {
      // Cache miss
        store_misses++;
        if (write_allocate) {
            if (empty_index == -1) {
                // No empty block, perform eviction (LRU)
                int lru_index = 0;
                uint32_t lru_ts = set.blocks[0].access_ts; // Initialize with the first block's timestamp

                // Find the block with the least recently used timestamp
                for (int i = 1; i < num_blocks; ++i) {
                    if (set.blocks[i].access_ts < lru_ts) {
                        lru_index = i;
                        lru_ts = set.blocks[i].access_ts;
                    }
                }

                // Evict the LRU block
                Block &evicted_block = set.blocks[lru_index];
                if (evicted_block.dirty) {
                    // Write back to memory if the block is dirty (write-back policy)
                    total_cycles += 25 * block_size; ; // Assume 100 cycles to write back to memory
                }
                

                // Replace the evicted block with the new block
                evicted_block.valid = true;
                evicted_block.tag = tag;
                evicted_block.dirty = !write_through; // Dirty if write-back, clean if write-through
                evicted_block.load_ts = timestamp;
                evicted_block.access_ts = timestamp;
                total_cycles += 25 * block_size; // Assume 25 cycles per byte to load from memory
            } else {
                // Use the empty block
                Block &target = set.blocks[empty_index];
                target.valid = true;
                target.tag = tag;
                target.dirty = !write_through; // Dirty if write-back, clean if write-through
                target.load_ts = timestamp;
                target.access_ts = timestamp;
                total_cycles += 25 * block_size; // Assume 25 cycles per byte to load from memory
            }

            if(write_through){
              total_cycles+=100;
            }
        } else {
            // No-write-allocate: write directly to memory
            total_cycles += 100; // Assume 100 cycles to write to memory
        }
    }
    total_cycles++; // Increment cycle count for the access


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



bool isPowerOfTwo(int x) {
  return x && ((x & (x - 1)) == 0);
}


int main( int argc, char **argv ) {

  int num_sets = atoi(argv[1]); 
  // Check if number of sets is a power of 2.
  if (!isPowerOfTwo(num_sets)) {
    std::cerr << "Error: Number of sets must be a power of 2." << std::endl;
    return EXIT_FAILURE;
  }
  
  int num_blocks = atoi(argv[2]);  


  int block_size = atoi(argv[3]);   
  // Check if block size is a power of 2 and is at least 4.
  if (!isPowerOfTwo(block_size) || block_size < 4) {
    std::cerr << "Error: Block size must be a power of 2." << std::endl;
    return EXIT_FAILURE;
  }
  
  
  bool write_allocate;
  string writemiss(argv[4]);
  if (writemiss == "write-allocate"){
    write_allocate = true;
  } else if (writemiss == "no-write-allocate"){
    write_allocate = false;
  } else {
    cerr << "Invalid write-miss policy. Use 'write-allocate' or 'no-write-allocate'." << endl;
    return EXIT_FAILURE;
  }

  bool write_through;
  string writehit(argv[5]);
  if (writehit == "write-back"){
    write_through = false;
  } else if (writehit == "write-through"){
    write_through = true;
  } else {
    cerr << "Invalid write-hit policy. Use 'write-through' or 'write-back'." << endl;
    return EXIT_FAILURE;
  }

  bool lru_eviction;         // true for LRU, false for FIFO.
  string policy(argv[6]);
  if (policy == "lru") {
    lru_eviction = true;
  } else if (policy == "fifo") {
    lru_eviction = false;
  } else {
    cerr << "Invalid eviction policy. Use 'lru' or 'fifo'." << endl;
    return EXIT_FAILURE;
  }






  // Check that write-back and no-write-allocate are not both specified.
  if (!write_allocate && !write_through) {
    std::cerr << "Error: Write-back and no-write-allocate cannot both be specified." << std::endl;
    return EXIT_FAILURE;
  }

  
  Cache cache(num_sets, num_blocks, block_size, write_allocate, write_through, lru_eviction);
  int load_hits = 0, load_misses = 0, store_hits = 0, store_misses = 0, total_loads = 0, total_stores = 0, total_cycles = 0;

  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    char op;
    uint32_t address;
    int val = 0; // Default value if the third value is missing

    iss >> op >> std::hex >> address;
    iss >> std::dec >> val; // Try to read the third value, but it's okay if it fails

    if (op == 'l') {
        total_loads++;
        cache.load(address, load_hits, load_misses, store_hits, store_misses, total_cycles);
    } else if (op == 's') {
        total_stores++;
        cache.store(address, load_hits, load_misses, store_hits, store_misses, total_cycles);
    }
  }

  std::cout << "Total loads: " << total_loads 
            << "\nTotal stores: " << total_stores
            << "\nLoad hits: " << load_hits
            << "\nLoad misses: " << load_misses
            << "\nStore hits: " << store_hits
            << "\nStore misses: " << store_misses
            << "\nTotal cycles: " << total_cycles << std::endl;
  
  return 0;
}

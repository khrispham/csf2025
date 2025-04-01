TODO: names of team members and their contributions to the project
Khris: wrote the code skeleton for the cache structure and main() code
John: implemented load() and store() functions within cache with write-through/write-back and write-alloc/no-write-alloc
TODO (for MS3): best cache report
Summary: There are 12 total trials, with 3 different variables
- Different cache format (but same total size)
    - 256 4 16: 4-way assiciative
    - 1024 1 16: Directly mapped
- Different write policy
    - write-allocate    write-back
    - no-write-allocate write-through
    - write-allocate    write-through
- Different eviction policy
    - LRU
    - FIFO


TRIAL 1: 256 4 16 write-allocate write-back fifo
                gcc.trace:      swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      314171          218357          503023              428926
Load misses:    4026            2311            9693                25815
Store hits:     188047          71787           78757               20403
Store misses:   9439            10738           2313                24844
Total cycles:   10033330        9727380         6340943             30521591

TRIAL 2: 256 4 16 write-allocate write-back lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      314798          219507          503992              429522
Load misses:    3399            1161            8724                25219
Store hits:     188250          71956           79207               20453
Store misses:   9236            10569           1863                24794
Total cycles:   9532733         9081549         5589393             30243241

TRIAL 3: 256 4 16 no-write-allocate write-through fifo
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      311017          217968          503709              429224
Load misses:    7180            2700            9007                25517
Store hits:     163705          57994           0                   0
Store misses:   33781           24531           81070               45247
Total cycles:   23299988        9693687         12303586            15231488

TRIAL 4: 256 4 16 no-write-allocate write-through lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      311613          218072          504737              429522
Load misses:    6584            2596            7979                25219
Store hits:     164819          58030           0                   0
Store misses:   32667           24495           81070               45247
Total cycles:   23062702        9652123         11892386            15112288

TRIAL 5: 256 4 16 write-allocate write-through fifo
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      314171          218357          503023              428926
Load misses:    4026            2311            9693                25815
Store hits:     188047          71787           78757               20403
Store misses:   9439            10738           2313                24844
Total cycles:   25838330        13847080        13581943            25308691

TRIAL 6: 256 4 16 write-allocate write-through lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      314798          219507          503992              429522
Load misses:    3399            1161            8724                25219
Store hits:     188250          71956           79207               20453
Store misses:   9236            10569           1863                24794
Total cycles:   25506533        13319649        13014793            25050341

TRIAL 7: 1024 1 16 write-allocate write-back fifo
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      312238          217735          501149              428934
Load misses:    5959            2933            11567               25807
Store hits:     187502          71538           77949               20407
Store misses:   9984            10987           3121                24840
Total cycles:   26828985        14195231        14653935            25303895

TRIAL 8: 1024 1 16 write-allocate write-back lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      312238          217735          501149              428934
Load misses:    5959            2933            11567               25807
Store hits:     187502          71538           77949               20407
Store misses:   9984            10987           3121                24840
Total cycles:   26828985        14195231        14653935            25303895

TRIAL 9: 1024 1 16 no-write-allocate write-through fifo

INITIAL SANITY CHECK: consider total loads and total stores for any trace file, which
remain the same on all instances.

LRU vs FIFO:
Compare trial 1 vs 2, 3 vs 4, 5 vs 6
Consider load hits/misses, store hits/misses, and total cycles, lru performs better in
ALL aspects.
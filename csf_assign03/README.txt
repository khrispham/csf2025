TODO: names of team members and their contributions to the project
Khris: wrote the code skeleton for the cache structure and main() code
John: implemented load() and store() functions within cache with write-through/write-back and write-alloc/no-write-alloc
TODO (for MS3): best cache report
Summary: There are 12 total trials, with 3 different variables
- Different cache format (but same total size)
    - 256 sets, 4 blocks/set, 16 byte block size: 4-way associative
    - 1024 sets, 1 block/set, 16 byte block size: Directly mapped
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
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      309108          217402          502194              429231
Load misses:    9089            3266            10522               25510
Store hits:     162581          57696           0                   0
Store misses:   34905           24829           81070               45247
Total cycles:   24062464        9919789         12909586            15228688

TRIAL 10: 1024 1 16 no-write-allocate write-through lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      309108          217402          502194              429231
Load misses:    9089            3266            10522               25510
Store hits:     162581          57696           0                   0
Store misses:   34905           24829           81070               45247
Total cycles:   24062464        9919789         12909586            15228688

TRIAL 11: 1024 1 16 write-allocate write-through fifo
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      312238          217735          501149              428934
Load misses:    5959            2933            11567               25807
Store hits:     187502          71538           77949               20407
Store misses:   9984            10987           3121                24840
Total cycles:   26828985        14195231        14653935            25303895

TRIAL 12: 1024 1 16 write-allocate write-through lru
                gcc.trace       swim.trace      test_cache.trace    test_cache2.trace
Total loads:    318197          220668          512716              454741
Total stores:   197486          82525           81070               45247
Load hits:      312238          217735          501149              428934
Load misses:    5959            2933            11567               25807
Store hits:     187502          71538           77949               20407
Store misses:   9984            10987           3121                24840
Total cycles:   26828985        14195231        14653935            25303895

ANALYSIS:
- SANITY CHECK: consider total loads and total stores for any trace file, which
remain the same on all trials.

- LRU vs FIFO:
Compare between trial 1 vs 2, 3 vs 4, 5 vs 6
Consider load hits/misses, store hits/misses, and total cycles, lru performs better in
ALL aspects.
Compare between trial 6 vs 7, 8 vs 9, 10 vs 11
Each pair are identical, since the difference between each pair is only in eviction 
policy, which does not matter for a direct mapped cache.

- Write policy
Compare between trial 1 vs 3 vs 5, 2 vs 4 vs 6, 7 vs 9 vs 11, 8 vs 10 vs 12
The performance of difference write policies seem to depend on whether the cache has
direct mapping or associative mapping. 
For direct mapping, no-write-allocate and write-through performs the best whereas the
other two write policies produce the same total cycles. This can be concluded that 
no-write-allocate is generally better for direct mapped caches.
For associative mapping, write-allocate and write-back performs the best, followed by
no-write-allocate and write-through, with write-allocate and write-through coming in
last. This pattern does not stay true for test_cache2.trace, where no-write-allocate
and write-through actually performs better. 

- Direct mapping and associative mapping
Compare between 1 vs 7, 2 vs 8, 3 vs 9, 4 vs 10, 5 vs 11, 6 vs 12
As expected, even just a 4-way associative mapping enhances the performance across all
trial pairs. Looking at the total cycles, the biggest difference is when we use a 
write-allocate and write-back policy. For example, the total cycles of trial 1 and 7 
for gcc.trace are 10.0 mil and 26.8 mil respectively, and for trial 2 and 8 they are 
9.5 mil and 26.8 mil. Other pairs have a difference of around 1 mil in favor of 
associative mapping.

- Conclusion:
We want to use associate mapping, which write-allocate and write-back are generally
suitable for. Furthermore, we want to use LRU eviction policy which will almost always
result in better performance
This combination is of trial 2, which generally has the lowest total cycles, only except
for test_cache2.trace.
TODO: names of team members and their contributions to the project
Dayana Matamoros Ramos: Created the process_trace, update_counters, create_cache, update_counters, find_lru, valid_parameters, and load_miss functions. 

Tobi Ijiyemi: Created the load_found, store_found, and store_miss functions. Also worked on the update_counters function. 

Both coded in main. 

TODO (for MS3): best cache report

Shorthand: write-allocate (WA), write-back (WB), no_write-allocate (NWA), write-through (WT), least-recently-used (LRU), first in first out (fifo)

Goal of the experiment is to find the best cache configuration. We can guide the experiment by tracking on
two key outputs: total cycles (that tests performance of the cache) and total cache size (cost of the cache).
When we can find the best balance of performance and cost of size, right before we reach diminishing returns, we would have found our 
best cache. 

The variables we can mutate are the number of sets, number of blocks per set, block sizes in bytes,
write association policy, write policy, and the eviction policy; then we'll measure the total loads, stores,
load hits and misses, store hits and misses, and finaly the total cycles. Performance is total cycles and cost is the total cache size which equals the
(number of sets times the number of blocks per set times block size).

For testing and evaluation we're going to use gcc.trace and swim.trace because these are traces from real programs.

First and foremost to see which policies are the best we're going to fix a specific cache setup with 256 sets, 4 blocks, and 16 bytes
and then vary the policies, to see which ones result in the lowest total cycles. 

gcc.trace, WA + WB + LRU

Total Loads: 318197
Total Stores: 197486
Load Hits: 314798
Load Misses: 3399
Store Hits: 188250
Store Misses: 9236
Total Cycles: 9344483

swim.trace, WA + WB + LRU

Total Loads: 220668
Total Stores: 82525
Load Hits: 219507
Load Misses: 1161
Store Hits: 71956
Store Misses: 10569
Total Cycles: 9009593

gcc.trace, WA + WB + fifo

Total Loads: 318197
Total Stores: 197486
Load Hits: 314171
Load Misses: 4026
Store Hits: 188047
Store Misses: 9439
Total Cycles: 9845283

swim.trace, WA + WB + fifo

Total Loads: 220668
Total Stores: 82525
Load Hits: 218357
Load Misses: 2311
Store Hits: 71787
Store Misses: 10738
Total Cycles: 9655593

gcc.trace, WA + WT + LRU

Total Loads: 318197
Total Stores: 197486
Load Hits: 314798
Load Misses: 3399
Store Hits: 188250
Store Misses: 9236
Total Cycles: 25318283

swim.trace, WA + WT + LRU

Total Loads: 220668
Total Stores: 82525
Load Hits: 219507
Load Misses: 1161
Store Hits: 71956
Store Misses: 10569
Total Cycles: 13247693

gcc.trace, WA + WT + fifo

Total Loads: 318197
Total Stores: 197486
Load Hits: 314171
Load Misses: 4026
Store Hits: 188047
Store Misses: 9439
Total Cycles: 25650283

swim.trace, WA + WT + fifo

Total Loads: 220668
Total Stores: 82525
Load Hits: 218357
Load Misses: 2311
Store Hits: 71787
Store Misses: 10738
Total Cycles: 13775293

gcc.trace, NWA + WT + LRU

Total Loads: 318197
Total Stores: 197486
Load Hits: 311613
Load Misses: 6584
Store Hits: 164819
Store Misses: 32667
Total Cycles: 22865216

swim.trace, NWA + WT + LRU

Total Loads: 220668
Total Stores: 82525
Load Hits: 218072
Load Misses: 2596
Store Hits: 58030
Store Misses: 24495
Total Cycles: 9569598

gcc.trace, NWA + WT + fifo

Total Loads: 318197
Total Stores: 197486
Load Hits: 311017
Load Misses: 7180
Store Hits: 163705
Store Misses: 33781
Total Cycles: 23102502

swim.trace, NWA + WT + fifo

Total Loads: 220668
Total Stores: 82525
Load Hits: 217968
Load Misses: 2700
Store Hits: 57994
Store Misses: 24531
Total Cycles: 9611162

Analysis from varying the policies of the cache:

From the data we've produced above, it seems like the best overall cahce policy is 
write-allocate (WA) + write-back (WB) + least-recently-used (LRU) as an evicition policy.

First of all write-through vs write-back: 
when using write-through the cache must write to memory on every single store, even on a store
hit with each of those writes costing 100 cycles; while the write-back policy writes to memory only
when the block is evicted we can see this in the more than two times difference in total cycles between 
WA + WB + LRU and WA + WT + LRU (for gcc) and the 4M cycle difference between WB and WT. 
So Write-back is better than write-through.

Next the eviction policy of LRU vs FIFO:
conceptualy LRU works better than fifo because lru evicts the block that hasn't been used in the longest time
while fifo simply evicts the block that's been in the cache the longest, even if it's been frequently accessed. 
While for WA + WB (gcc) LRU and FIFO had similar total cycles but LRU had fewer load misses 3399 versus 4026; 
swm.trace (WA + WB) LRU had 1161 load misses compares to 2311. Load misses correlate to fewer 100 cycle lookups, 
which lowers total cycles.

Finally allocation policy of Write-allocate vs no-write-allocate:
Surprisingly NWA beats WA but only because we're forced to used WT rather than the better WB policy (we can't combine WB and NWA).
We see this with the difference in cycles with WT + LRU (gcc) with WA producing 25.31M cycles and NWA producing 22.86 cycles; for swim:
13.24M cycles for WA and 9.57M cycles for NWA.

In conclusion: the best policies to use are WA + WB + LRU.
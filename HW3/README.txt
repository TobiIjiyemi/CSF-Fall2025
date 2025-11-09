TODO: names of team members and their contributions to the project
Dayana Matamoros Ramos: Created the process_trace, update_counters, create_cache, update_counters, find_lru, valid_parameters, and load_miss functions. 

For ms3: Created the find_fifo function and then integrated the 6th argument (eviction policy) into the rest of the code. Compared our results to those on
courselore. 

Tobi Ijiyemi: Created the load_found, store_found, and store_miss functions. Also worked on the update_counters function. 
For ms3: Completed the analysis portion of the assignment. Set up the experiment and analyzed the results of that experiment in the README.  

Both coded in main. 

TODO (for MS3): best cache report

Shorthand: write-allocate (WA), write-back (WB), no_write-allocate (NWA), write-through (WT), least-recently-used (LRU), first in first out (fifo)
Conclusion (also at the very bottom): Compiling all of this analysis together, it seems like the best cache (from our tests) is a 32kb, 4-byte, 4-way (or 8-way)
write allocate, write-back, and least recently used eviction policy cache configuration.

*A significant portion of the README data is just test-data*


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

From the data we've produced above, it seems like the best overall cache policy is 
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

Now for the next part of the experiment we're going to use write-allocate, write-back, and LRU as the definitive policies and focus on
varying the number of sets, blocks, and block sizes to see if we can reduce costs and maximize performance. The plan is to test specific
total cache sizes and within each cache size test the different configurations of the cache (from direct-mapping to set associativitiy to n, m 
set associativity and then block size as well) and see how that affects performance, and to see how far we can minimize the memory costs of the cache.

Okay we're going to break the testing up into 3 different groups (4kb cache, 8kb cache,and then 32 kb cache) varying the block sizes as well (4 byte, 8 bytes, and 16 bytes)
and vary the set associativity (1-way, 4-way, and 8-way). Once again we'll be testing on the two trace gcc.trace and swim.trace:

total size (kb); block size (bytes); associativity (e.g. 2-way); # of sets

4KB PORTION:

gcc 4 4 1 1024:

Total Loads: 318197
Total Stores: 197486
Load Hits: 298686
Load Misses: 19511
Store Hits: 165874
Store Misses: 31612
Total Cycles: 9233383

swim 4 4 1 1024:

Total Loads: 220668
Total Stores: 82525
Load Hits: 209991
Load Misses: 10677
Store Hits: 59635
Store Misses: 22890
Total Cycles: 6057693

gcc 4 4 4 256:

Total Loads: 318197
Total Stores: 197486
Load Hits: 310247
Load Misses: 7950
Store Hits: 168688
Store Misses: 28798
Total Cycles: 7187283

swim 4 4 4 256:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214159
Load Misses: 6509
Store Hits: 60907
Store Misses: 21618
Total Cycles: 5304493

gcc 4 4 8 128:

Total Loads: 318197
Total Stores: 197486
Load Hits: 310361
Load Misses: 7836
Store Hits: 168757
Store Misses: 28729
Total Cycles: 7159183

swim 4 4 8 128:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214169
Load Misses: 6499
Store Hits: 61043
Store Misses: 21482
Total Cycles: 5269993

gcc 4 8 1 512:

Total Loads: 318197
Total Stores: 197486
Load Hits: 298786
Load Misses: 19411
Store Hits: 175801
Store Misses: 21685
Total Cycles: 14013683

swim 4 8 1 512:

Total Loads: 220668
Total Stores: 82525
Load Hits: 209864
Load Misses: 10804
Store Hits: 61856
Store Misses: 20669
Total Cycles: 10998993

gcc 4 8 4 128:

Total Loads: 318197
Total Stores: 197486
Load Hits: 310895
Load Misses: 7302
Store Hits: 178685
Store Misses: 18801
Total Cycles: 9738283

swim 4 8 4 128:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214224
Load Misses: 6444
Store Hits: 63171
Store Misses: 19354
Total Cycles: 9412393

gcc 4 8 8 64:

Total Loads: 318197
Total Stores: 197486
Load Hits: 311066
Load Misses: 7131
Store Hits: 178746
Store Misses: 18740
Total Cycles: 9674483

swim 4 8 8 64:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214259
Load Misses: 6409
Store Hits: 63335
Store Misses: 19190
Total Cycles: 9327393

gcc 4 16 1 256:

Total Loads: 318197
Total Stores: 197486
Load Hits: 298863
Load Misses: 19334
Store Hits: 185202
Store Misses: 12284
Total Cycles: 20312483

swim 4 16 1 256:

Total Loads: 220668
Total Stores: 82525
Load Hits: 211533
Load Misses: 9135
Store Hits: 70212
Store Misses: 12313
Total Cycles: 14381193

gcc 4 16 4 64:

Total Loads: 318197
Total Stores: 197486
Load Hits: 312099
Load Misses: 6098
Store Hits: 187526
Store Misses: 9960
Total Cycles: 11414883

swim 4 16 4 64:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214923
Load Misses: 5745
Store Hits: 71353
Store Misses: 11172
Total Cycles: 11733193

gcc 4 16 8 32:

Total Loads: 318197
Total Stores: 197486
Load Hits: 312665
Load Misses: 5532
Store Hits: 187617
Store Misses: 9869
Total Cycles: 11068483

swim 4 16 8 32:

Total Loads: 220668
Total Stores: 82525
Load Hits: 215231
Load Misses: 5437
Store Hits: 71451
Store Misses: 11074
Total Cycles: 11497193

8KB PORTION:

gcc 8 4 1 2048:

Total Loads: 318197
Total Stores: 197486
Load Hits: 306254
Load Misses: 11943
Store Hits: 167302
Store Misses: 30184
Total Cycles: 7892583

swim 8 4 1 2048:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214855
Load Misses: 5813
Store Hits: 60665
Store Misses: 21860
Total Cycles: 5247093

gcc 8 4 4 512:

Total Loads: 318197
Total Stores: 197486
Load Hits: 311802
Load Misses: 6395
Store Hits: 169394
Store Misses: 28092
Total Cycles: 6788183

swim 8 4 4 512:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218395
Load Misses: 2273
Store Hits: 61469
Store Misses: 21056
Total Cycles: 4689093

gcc 8 4 8 256:

Total Loads: 318197
Total Stores: 197486
Load Hits: 311972
Load Misses: 6225
Store Hits: 169443
Store Misses: 28043
Total Cycles: 6761283

swim 8 4 8 256:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218361
Load Misses: 2307
Store Hits: 61477
Store Misses: 21048
Total Cycles: 4689393

gcc 8 8 1 1024:

Total Loads: 318197
Total Stores: 197486
Load Hits: 306753
Load Misses: 11444
Store Hits: 177224
Store Misses: 20262
Total Cycles: 11319283

swim 8 8 1 1024:

Total Loads: 220668
Total Stores: 82525
Load Hits: 214850
Load Misses: 5818
Store Hits: 62918
Store Misses: 19607
Total Cycles: 9353993

gcc 8 8 4 256:

Total Loads: 318197
Total Stores: 197486
Load Hits: 312586
Load Misses: 5611
Store Hits: 179374
Store Misses: 18112
Total Cycles: 9015083

swim 8 8 4 256:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218561
Load Misses: 2107
Store Hits: 63800
Store Misses: 18725
Total Cycles: 8156593

gcc 8 8 8 128:

Total Loads: 318197
Total Stores: 197486
Load Hits: 312750
Load Misses: 5447
Store Hits: 179419
Store Misses: 18067
Total Cycles: 8960883

swim 8 8 8 128:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218518
Load Misses: 2150
Store Hits: 63813
Store Misses: 18712
Total Cycles: 8157193

gcc 8 16 1 512:

Total Loads: 318197
Total Stores: 197486
Load Hits: 308018
Load Misses: 10179
Store Hits: 186206
Store Misses: 11280
Total Cycles: 14469283

swim 8 16 1 512:

Total Loads: 220668
Total Stores: 82525
Load Hits: 216046
Load Misses: 4622
Store Hits: 71132
Store Misses: 11393
Total Cycles: 11479593

gcc 8 16 4 128:

Total Loads: 318197
Total Stores: 197486
Load Hits: 314007
Load Misses: 4190
Store Hits: 188015
Store Misses: 9471
Total Cycles: 10071683

swim 8 16 4 128:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218920
Load Misses: 1748
Store Hits: 71854
Store Misses: 10671
Total Cycles: 9549993

gcc 8 16 8 64:

Total Loads: 318197
Total Stores: 197486
Load Hits: 314221
Load Misses: 3976
Store Hits: 188067
Store Misses: 9419
Total Cycles: 9927283

swim 8 16 8 64:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218922
Load Misses: 1746
Store Hits: 71867
Store Misses: 10658
Total Cycles: 9534393

32 KB PORTION:

gcc 32 4 1 8192:

Total Loads: 318197
Total Stores: 197486
Load Hits: 311740
Load Misses: 6457
Store Hits: 169399
Store Misses: 28087
Total Cycles: 6214483

swim 32 4 1 8192:

Total Loads: 220668
Total Stores: 82525
Load Hits: 217965
Load Misses: 2703
Store Hits: 61361
Store Misses: 21164
Total Cycles: 4381693

gcc 32 4 4 2048:

Total Loads: 318197
Total Stores: 197486
Load Hits: 313013
Load Misses: 5184
Store Hits: 169929
Store Misses: 27557
Total Cycles: 5954983

swim 32 4 4 2048:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218987
Load Misses: 1681
Store Hits: 61633
Store Misses: 20892
Total Cycles: 4209693

gcc 32 4 8 1024:

Total Loads: 318197
Total Stores: 197486
Load Hits: 313054
Load Misses: 5143
Store Hits: 169998
Store Misses: 27488
Total Cycles: 5945283

swim 32 4 8 1024:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218983
Load Misses: 1685
Store Hits: 61644
Store Misses: 20881
Total Cycles: 4208893

gcc 32 8 1 4096:

Total Loads: 318197
Total Stores: 197486
Load Hits: 312370
Load Misses: 5827
Store Hits: 179384
Store Misses: 18102
Total Cycles: 8504683

swim 32 8 1 4096:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218077
Load Misses: 2591
Store Hits: 63661
Store Misses: 18864
Total Cycles: 7725993

gcc 32 8 4 1024:

Total Loads: 318197
Total Stores: 197486
Load Hits: 313804
Load Misses: 4393
Store Hits: 179909
Store Misses: 17577
Total Cycles: 7955483

swim 32 8 4 1024:

Total Loads: 220668
Total Stores: 82525
Load Hits: 219185
Load Misses: 1483
Store Hits: 63980
Store Misses: 18545
Total Cycles: 7337393

gcc 32 8 8 512:

Total Loads: 318197
Total Stores: 197486
Load Hits: 313851
Load Misses: 4346
Store Hits: 179977
Store Misses: 17509
Total Cycles: 7933483

swim 32 8 8 512:

Total Loads: 220668
Total Stores: 82525
Load Hits: 219180
Load Misses: 1488
Store Hits: 63991
Store Misses: 18534
Total Cycles: 7335993

gcc 32 16 1 2048:

Total Loads: 318197
Total Stores: 197486
Load Hits: 313662
Load Misses: 4535
Store Hits: 188043
Store Misses: 9443
Total Cycles: 9684083

swim 32 16 1 2048:

Total Loads: 220668
Total Stores: 82525
Load Hits: 218655
Load Misses: 2013
Store Hits: 71725
Store Misses: 10800
Total Cycles: 9191193

gcc 32 16 4 512:

Total Loads: 318197
Total Stores: 197486
Load Hits: 315211
Load Misses: 2986
Store Hits: 188468
Store Misses: 9018
Total Cycles: 8607683

swim 32 16 4 512:

Total Loads: 220668
Total Stores: 82525
Load Hits: 219604
Load Misses: 1064
Store Hits: 71993
Store Misses: 10532
Total Cycles: 8521593

gcc 32 16 8 256:

Total Loads: 318197
Total Stores: 197486
Load Hits: 315261
Load Misses: 2936
Store Hits: 188513
Store Misses: 8973
Total Cycles: 8562483

swim 32 16 8 256:

Total Loads: 220668
Total Stores: 82525
Load Hits: 219607
Load Misses: 1061
Store Hits: 72010
Store Misses: 10515
Total Cycles: 8509993

First we'll analyze the impact of the block size. If we take the averages of the number of cycles from a
4-byte block, 8-byte block, and 16-byte block we get 5.9M cycles, 9.1M cycles, and 11.0M cycles respectively.
The 4-byte block resulted in the best peformance.

Next is the impact of the total cache size: There was significant decrease in the number of cycles as we moved from
4kb (10.2M avg cycles) cache to 8kb (8.6M avg cycles) cache and once again when we moved from 8kb to 32kb (7.2M avg cycles) cache. Since we didn't hit the point of diminishing returns
on the decrease in the cache size, we can conclude that (out of the cache sizes we tested) a 32kb cache has the best performance.

And finally imact of the associativity: Direct-mapped associativity resulted in the high avg of cycles (10.0M)
while 4-way and 8-way associativity resulted in around 8M average cycles. 

Compiling all of this analysis together, it seems like the best cache (from our tests) is a 32kb, 4-byte, 4-way (or 8-way)
write allocate, write-back, and least recently used eviction policy cache configuration.


CONTRIBUTIONS

TODO: write a brief summary of how each team member contributed to
the project.

Tobi - Task 1, Task 2
Dayana - Task 3
Both: Analysis

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.

Test run with threshold 2097152

real    0m0.409s
user    0m0.393s
sys     0m0.014s
Test run with threshold 1048576

real    0m0.246s
user    0m0.425s
sys     0m0.026s
Test run with threshold 524288

real    0m0.149s
user    0m0.418s
sys     0m0.034s
Test run with threshold 262144

real    0m0.121s
user    0m0.452s
sys     0m0.046s
Test run with threshold 131072

real    0m0.111s
user    0m0.444s
sys     0m0.059s

**** Point of Diminishing Returns was reached here ***
Test run with threshold 65536

real    0m0.113s
user    0m0.466s
sys     0m0.073s
Test run with threshold 32768

real    0m0.113s
user    0m0.492s
sys     0m0.109s
Test run with threshold 16384

real    0m0.123s
user    0m0.524s
sys     0m0.151s

*** SECOND RUN *** 
Test run with threshold 2097152

real    0m0.374s
user    0m0.363s
sys     0m0.009s
Test run with threshold 1048576

real    0m0.214s
user    0m0.380s
sys     0m0.018s
Test run with threshold 524288

real    0m0.170s
user    0m0.417s
sys     0m0.046s
Test run with threshold 262144

real    0m0.124s
user    0m0.461s
sys     0m0.042s
Test run with threshold 131072

real    0m0.113s
user    0m0.439s
sys     0m0.063s
Test run with threshold 65536

real    0m0.107s
user    0m0.459s
sys     0m0.079s

*** Diminishing Returns Here ***
Test run with threshold 32768

real    0m0.109s
user    0m0.506s
sys     0m0.098s
Test run with threshold 16384

real    0m0.131s
user    0m0.536s
sys     0m0.158s

*** Third Run ***
Test run with threshold 2097152

real    0m0.374s
user    0m0.363s
sys     0m0.009s
Test run with threshold 1048576

real    0m0.213s
user    0m0.374s
sys     0m0.023s
Test run with threshold 524288

real    0m0.155s
user    0m0.418s
sys     0m0.038s
Test run with threshold 262144

real    0m0.127s
user    0m0.449s
sys     0m0.049s
Test run with threshold 131072

real    0m0.119s
user    0m0.452s
sys     0m0.048s
Test run with threshold 65536

real    0m0.112s
user    0m0.467s
sys     0m0.077s

*** Diminishing Returns Here ***
Test run with threshold 32768

real    0m0.115s
user    0m0.507s
sys     0m0.093s
Test run with threshold 16384

real    0m0.145s
user    0m0.557s
sys     0m0.145s


 For arrays of size < 32768 elements, the time it takes to fork() for the left and right sub-arrays and then have the parent wait for each sub-process to complete
 before continuing can be greater than the time it would take to sort the array in one process. 
 
 Each child process requires its own virtual address space, which the OS must map to physical memory through page tables. This increases overhead space and also 
 increases the potential of page faults, resulting in longer execution time (as OS has to handle each page fault that occurs). 

 However, for arrays of size > 32,768, the cost of creating and managing child processes is smaller in comparison to the cost of performing quick sort. Dividing the 
 array among multiple processes running in parallel, despite the increase in overhead, ultimately decreases execution time because multiple sub-arrays are being sorted
 at once.

 The threshold is where the expensiveness of creating child processes is outweighed by the expedited parallel sorting. 




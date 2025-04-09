CONTRIBUTIONS

TODO: write a brief summary of how each team member contributed to
the project.
Khris: Wrote the report
John: Implemented parsort.c

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.
Test run with threshold 2097152

real    0m0.737s
user    0m0.695s
sys     0m0.035s
Test run with threshold 1048576

real    0m0.403s
user    0m0.677s
sys     0m0.055s
Test run with threshold 524288

real    0m0.275s
user    0m0.677s
sys     0m0.086s
Test run with threshold 262144

real    0m0.226s
user    0m0.685s
sys     0m0.116s
Test run with threshold 131072

real    0m0.207s
user    0m0.724s
sys     0m0.118s
Test run with threshold 65536

real    0m0.191s
user    0m0.686s
sys     0m0.186s
Test run with threshold 32768

real    0m0.197s
user    0m0.734s
sys     0m0.263s
Test run with threshold 16384

real    0m0.191s
user    0m0.754s
sys     0m0.487s

Real runtime - Threshhold:
When the threshhold decreases, more child processes are needed for the same number of elements 
needed to be sorted. By having more child processes, we are able to utilize more cores of the CPU
that run concurrently with each other, reducing the amount of time needed to sort the same number
of elements. Thus, as the threshhold decreases, the runtime also decreases by virtue of having
multiple instances of quicksort happening at the same time in different parts of the sequence.

User runtime:
Notice that the user runtime do not change much with different thresholds. This is because the
user runtime adds up CPU time across all cores, so no matter how many cores are being utilized, 
the CPU still has to sort the entire sequence. The user runtime is more dependent on the number
of elements needed to be sorted, which do not change across the test runs. The small variations
between the test runs come from the randomness of the sequence needed to be sorted, with some
slightly more sorted than others.

System runtime - Threshold:
Notice that system runtimes have a negative relationship with the threshold. That is, as the
threshhold decreases, the system runtime increases (and vice versa). We have established that
more child processes are created with lower threshholds, which requires the CPU to be in kernel
mode for longer to appropriately set up those processes. There is an overhead cost for each new
process being made, so more child processes result in higher system runtime.

Diminishing return:
Using more CPU cores to sort the sequence generally lowers the real runtime, but it hits a plateau
on the lower end, because as much as per-core runtime is reduced with more child processes, the
overhead from having to create one in the first place starts to add up as well, resulting in
diminishing returns up to a certain point.

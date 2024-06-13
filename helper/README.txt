BSEARCH
-------
To help you to check your program correctness, a sample search program called "bsearch" that produces 
the same search results required by this assignment is provided. It does NOT read a RLB and requires 
the original TXT file, but you can still use it to verify your search results. To use it, simply use
the TXT file and the search term as input arguments, e.g.,:

cs9319@vx05:~$ ~cs9319/a2/helper/bsearch ~cs9319/a2/dummy.txt "in"
[8]Computers in industry
[11]Big data indexing
cs9319@vx05:~$ 


SAMPLE C FILES & MAKEFILE
-------------------------
There are two versions of the same program - to print a binary RLB file to stdout in a human readable way.

Why two versions (sample1 and sample2)?
One uses bit operations and the other one doesn't (choose one that suits your programming preference).

Please read them first (sample1.c and sample2.c). Feel free to modify them to C++ if you like.

A sample, minimal makefile is included so that you can use it as a starting template for your assignment 2 
(of course, you will need to MODIFY it).

To test the sample program, first type the command:

make

Then you can do something like:

./sample1 abcde.rlb

./sample1 a20k.rlb

The program only works for single-byte counts for now. But if you observe the multi-byte count 
files (such as a20k.rlb), it is not hard for you to figure and extend it to multi-byte counts.

Good luck!

RW 22/6/23


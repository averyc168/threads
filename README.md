# Sorting Integers using threads from pthread library

### Purpose:
Takes an input file of integers with the first line being the size of the input, and
first partitions the array into sublists using threads. These threads then sort their sublist
of the array by bubble sort and insertion sort. Afterward, the threads are joined,
and the sublists are merged to create one vast sorted array. The sorted arrays are then
written onto a file. The threads and sorting are also timed to see if there is a correlation
between the number of threads and the time to sort.

### Input: 
The user enters an input text file with the first line as the number of integers on the
file and an integer to represent the number of threads to perform the sort.

### Output:
The program will output eight text files, 2 for each thread number (1,2,4 or 8 threads)
and within the two files, one file will be of bubble sort and the other will be insertion sort.
The program also outputs messages of the times it took for both the partition made by the threads
and the time it took for insertion sort and bubble sort to sort the input text of integers in milliseconds.


### To compile and run the program, here is what you should do:
1. gcc main.c -o a.out
2. ./a.out *(filename) (number of threads)*


### Analysis of threads and sorting algorithms (used 100000 ints to be sorted):
Average time for 1 thread in bubble sort: 25211.8 milliseconds
<br />Average time for 1 thread in insertion sort: 4970.5 milliseconds

Average time for 2 threads in bubble sort: 6218.9 milliseconds
<br />Average time for 2 threads in insertion sort: 1356.1 milliseconds

Average time for 4 threads in bubble sort: 1860.1 milliseconds
<br />Average time for 4 threads in insertion sort: 435.9 milliseconds

Average time for 8 threads in bubble sort: 678 milliseconds
<br />Average time for 8 threads in insertion sort: 194 milliseconds


Based on these average times, there is a clear and negative correlation between the number of threads and the time it takes to sort the inputted array. When using a constant of 100000 integers, more threads significantly lower the time to sort. In other words, multi-threading performance is a lot faster than single-threading performance. This is evident for both bubble sort and insertion sort. For bubble sort, the times were 25211.8 milliseconds for one thread, 6218.9 milliseconds for two threads, 1860.1 milliseconds for four threads, and 678 milliseconds for eight threads. For insertion sort, the times were 4970.5 milliseconds for one thread, 1356.1 milliseconds for two threads, 435.9 milliseconds for four threads, and 194 milliseconds for eight threads. The relationship is not linear, but more exponential since the times drop by a large factor with more threads. Even with the threads, bubble sort is a lot slower than insertion sort.


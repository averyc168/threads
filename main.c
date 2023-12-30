/*
 * Name: Avery Chan
 * Date: 10/8/23
 *
 * Program: Project 1
 *
 * Purpose: Takes an input file of integers with the first line is the size of the input, and
 * first partitions the array into sublists using threads. These threads then sort their sublist
 * of the array by bubble sort and insertion sort. After that, the threads are joined together,
 * and the sublists are merged to create one fully large sorted array. The sorted arrays are then
 * written onto a file. The threads and sorting are also timed to see if there is a correlation
 * between number of threads and time to sort.
 *
 * Input: The user enters an input text file with the first line as the number of integers on the
 * file and an integer to represent the number of threads to perform the sort.
 *
 * Output: The program will output eight text files, 2 for each thread number (1,2,4 or 8 threads)
 * and within the two files, one file will be of bubble sort and the other is insertion sort.
 * The program also outputs messages of the times it took for both the partition made by the threads
 * and time it took for insertion sort and bubble sort to sort the input text of integers in milliseconds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

struct thread_data
{
    int	thread_id;  // the thread number
    int start;      // the starting index in the array of the thread
    int end;        // the ending index in the array of the thread
    int *array;     // the base address of the main array
};

//================================================================================================================
/**
 * reads the file and puts the values into the main array
 * @param infile - file pointer to the file that is being read
 * @param array  - array that will be filled with values from the file
 * @param length - size of the array
 * there is no return value but the array should be filled with ints from the file
 */
void readFile(FILE* infile, int array[], int length) {
    if (infile == NULL) {       // checks to see if the file is read in
        printf("FILE ERROR!");
        exit(1);
    }

    int i = 0;      // array iterator
    int x;          // int variable that is holding the int from the file
    while (i < length) {        // while the array is not filled
        fscanf(infile, "%d", &x);       // reads file line by line
        array[i] = x;
        i++;
    }

    fclose(infile);     // closes the file
}

/**
 * copies the elements from the arrays one element at a time onto the text file line by line
 * @param fptr - file pointer that has the address of the text file to be written on
 * @param array - the dynamically allocated array with unsorted or sorted elements inside
 * @param length - the number of elements in the array
 * there is no return value but the files should be written on line by line of elements from the array
 */
void writeFile(FILE *fptr, int array[], int length) {
    if (fptr == NULL) {     // checks to see if the file pointer exists
        printf("Error!\n");
        exit(-2);
    }
    for(int i = 0; i < length; i++) {       // for each line, write the int onto a file
        fprintf(fptr, "%d\n", array[i]);
    }
    fclose(fptr);       // closes the file
}

// bubbleToTop function
// iterates through the array and compares the two neighboring elements in the array, swaps them
// if one is greater than the other
// Precondition: the subarray and the low and high index values of that subarray
// Postcondition: no return value, but larger numbers should be at the end of the subarray
void bubbleToTop(int A[], int low, int high) {
    for (int i = low; i <= high - 1; i++) {
        if(A[i] > A[i+1]) {
            int temp = A[i];    // temp variable to hold value and swap
            A[i] = A[i + 1];
            A[i + 1] = temp;
        }
    }
}

// bubbleSort function
// saves the struct thread_data into a new variable, and it iterates through the array and
// calls the bubbleToTop function
// Precondition: thread_array struct array, holding the info of the threads
// Postcondition: no return value, but the array should be sorted at the end
void *bubbleSort(void *threadArg) {
    struct thread_data *bubble_data;    // new variable that holds the struct data from the one that was passed in to the function
    bubble_data = (struct thread_data *) threadArg;
    int *PtrtobubbleArray, start, end, id;
    id = bubble_data->thread_id;                // the thread id
    start = bubble_data->start;                 // the starting index
    end = bubble_data->end;                     // the ending index
    PtrtobubbleArray = bubble_data->array;      // pointer to the array

    for (int i = end; i >= start + 1; i--) {
        bubbleToTop(PtrtobubbleArray, start, i);
    }

    PtrtobubbleArray = NULL;        // sets the pointer to NULL
    return(NULL);
}

/**
 * insert function
 * iterates through the array and inserts the elements in the right sorted position
 * @param A = array that contains randomly generated integers
 * @param n = the specific index of the array
 * @param start = the starting index of the thread in the subarray
 * There is no return, but the element under consideration should be in the
 * correct spot within the array
 */
void insert(int A[], int n, int start) {
    int elt = A[n];     // saves the nth element of the array
    int freepos = n;    // saves the freed element position
    while (freepos > start && A[freepos - 1] > elt) {
        A[freepos] = A[freepos-1];
        freepos--;
    } // endwhile
    A[freepos] = elt;
}

/**
 * insertionSort function
 * copies the thread_data struct into new struct thread_data variable and
 * goes through each element in the array and sorts it in place
 * @param threadArg - the struct that has the specific thread_data
 * There is no return, but the array should be sorted afterwards via insertion sort
 */
void *insertionSort(void *threadArg) {
    struct thread_data *insertion_data;     // struct thread_data variable that holds the data from the struct that is passed into the function
    insertion_data = (struct thread_data *) threadArg;
    int *PtrtoInsertionArray, start, end, id;
    id = insertion_data->thread_id;         // id of the thread from the struct
    start = insertion_data->start;          // the starting index of the subarray of the thread
    end = insertion_data->end;              // the ending index of the subarray of the thread
    PtrtoInsertionArray = insertion_data->array;       // the pointer to the base address of the array from the thread_data struct

    for (int i = start + 1; i <= end; i++) {
       insert(PtrtoInsertionArray, i, start);
    } //endfor

    PtrtoInsertionArray = NULL;
    return(NULL);
}

/**
 * merge function
 * takes the array that is sorted in each partition, and merges them all to create a whole
 * sorted array
 * @param array - array address that will hold all the sorted values
 * @param numOfThreads - number of threads specified by the user input
 * @param length - the size of the main array
 * @param data - the array of thread_data structs that has the data like starting and ending index
 * @return - returns an int pointer to the base address of the fully sorted main array
 */
int* merge(int array[], int numOfThreads, int length, struct thread_data data[]) {
    int startIndexArray[numOfThreads];  // array to keep track of the start values
    int endIndexArray[numOfThreads];    // array to keep track of the end values
    for (int i = 0; i < numOfThreads; i++) {    // for loop to copy start and end values from the struct
        startIndexArray[i] = data[i].start;
        endIndexArray[i] = data[i].end;
    }
    int iterator = numOfThreads - 1; // keeps track of the startIndexArray info size that we want to see
    int sortedArrayIterator = 0;    // index value for the merged sorted array
    int *sortedArray;               // pointer to the fully sorted array
    sortedArray = (int *) malloc(sizeof(int) * length);

    // while the size of thread_data is still 0 or above or the array iterator is still within array bounds
    while (iterator >= 0 || sortedArrayIterator < length) {
        int smallestVal = array[startIndexArray[0]];        // smallest value of all starting threads
        int smallValIndex = 0;                              // index of thread_data struct that had smallest value
        for (int i = 0; i <= iterator; i++) {               // for loop through the starting index array
            if (array[startIndexArray[i]] <= smallestVal) { // checks to see smallest value
                smallestVal = array[startIndexArray[i]];
                smallValIndex = i;
            }
        }
        sortedArray[sortedArrayIterator] = smallestVal;     // puts the smallest value into main array
        sortedArrayIterator++;      // iterates the index of main array
        // if the starting index of this struct thread_data is the same as the end index, we swap with where the
        // last available thread_struct and lower the size of the struct array by one
        if (startIndexArray[smallValIndex] == endIndexArray[smallValIndex]) {
            int startTemp = startIndexArray[smallValIndex]; // temporary starting index variable for the swap
            startIndexArray[smallValIndex] = startIndexArray[iterator];
            startIndexArray[iterator] = startTemp;
            int endTemp = endIndexArray[smallValIndex]; // temporary ending index variable for the swap
            endIndexArray[smallValIndex] = endIndexArray[iterator];
            endIndexArray[iterator] = endTemp;
            iterator--;
        }
        else {
            startIndexArray[smallValIndex]++;   // if start != end, increase the starting index
        }
    }

    return sortedArray;     // returns address to the main sorted array
}

//==========================================================================================
int main (int argc, char *argv[]) {
    struct timeval startTime;	/* starting time */
    struct timeval endTime;	    /* ending time */
    unsigned long timePassed;	/* elapsed milliseconds */

    char* filename = argv[1];   // filename from command line user input
    int numOfThreads = atoi(argv[2]); // thread count from command line user input
    // checks to see if the number of threads user inputted is valid
    if (!(numOfThreads == 1 || numOfThreads == 2 || numOfThreads == 4 || numOfThreads == 8)) {
        printf("ERROR: INVALID NUMBER OF THREADS\n");
        exit(-1);
    }

    FILE* infile, *insertionSortFile, *bubbleSortFile;    // file pointers to create files

    int *array, *dupArray;      // int array pointers to hold sorted subarrays

    // checks to see if the file is loaded in properly
    if ((infile = fopen(filename, "r")) == NULL) {
        printf("FILE ERROR!");
        exit(1);
    }
    int length;     // size of the array
    fscanf(infile, "%d", &length);     // reads in the first line of file to get length
    array = (int *) malloc(sizeof(int) * length);   // allocates an int array of size length
    dupArray = (int *) malloc(sizeof(int) * length); // allocates a second int array of size length
    readFile(infile, array, length);    // calls readFile function to read file and put values into array
    memcpy(dupArray, array, length * sizeof(int));  // copies the values from first array into dupArray


    pthread_t bubbleThreads[numOfThreads];  // array for the threads
    struct thread_data bubbleThread_array[numOfThreads]; // an array of structs to hold thread_data
    int rc; // int to check if thread is created
    // thread id: 1,2,3,4,5,6,7,8
    // start = N/M * (thread_id - 1)
    // end   = N/M * thread_id - 1
    gettimeofday(&startTime, 0);	/* mark the start time */
    for (int t = 0; t < numOfThreads; t++) {    // for each number of threads
        bubbleThread_array[t].thread_id = t+1;    // thread id always iterating
        bubbleThread_array[t].start = length / numOfThreads * (bubbleThread_array[t].thread_id - 1);
        if (length % numOfThreads != 0) {         // checks to see if there is a remainder
            if (t == numOfThreads - 1) { // if it is the last thread, then this is the end
                bubbleThread_array[t].end = length - 1;
            }
            else {  // if not then this is the end
                bubbleThread_array[t].end = bubbleThread_array[t].start + (length / numOfThreads) - 1;
            }
        }
        else {   // if there is no remainder
            bubbleThread_array[t].end = length / numOfThreads * bubbleThread_array[t].thread_id - 1;
        }
        bubbleThread_array[t].array = array;    // passes the address of the base array

        // creates the threads
        rc = pthread_create(&bubbleThreads[t], NULL, bubbleSort, (void *) &bubbleThread_array[t]);
        if (rc) {   // checks to see if the threads were created
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    } //endfor

    // joins the threads
    for (int j = 0; j < numOfThreads; j++) {
        pthread_join(bubbleThreads[j], NULL);
    }

    int *bubbleSortedArrayMerged, *insertionSortedArrayMerged;  // pointers to int arrays that will be fully sorted
    bubbleSortedArrayMerged = merge(array, numOfThreads, length, bubbleThread_array);   // calls the merge function for bubble sort
    gettimeofday(&endTime, 0);		/* mark the end time */
    timePassed = ((endTime.tv_sec * 1000) + endTime.tv_usec / 1000) - ((startTime.tv_sec * 1000) + startTime.tv_usec / 1000);
    printf("\ntime it took for bubble sort %d milliseconds\n", timePassed);

    char filename_bubble[30];   // filename for bubble sort txt
    sprintf(filename_bubble, "[%d]threads_sorted_list1.txt", numOfThreads);  // filename for insertion sort file
    bubbleSortFile = fopen(filename_bubble, "w"); // opening the file so it can be written on
    writeFile(bubbleSortFile, bubbleSortedArrayMerged, length-1); // function to call to write elements of array onto file


    pthread_t insertionThreads[numOfThreads];   // array for the threads
    struct thread_data insertionThread_array[numOfThreads]; // array of struct thread_data to hold the data
    int rc2; // int to check if thread is created
    gettimeofday(&startTime, 0);	/* mark the start time */
    for (int t = 0; t < numOfThreads; t++) {
        insertionThread_array[t].thread_id = t+1;    // thread id always iterating
        insertionThread_array[t].start = length / numOfThreads * (insertionThread_array[t].thread_id - 1);
        if (length % numOfThreads != 0) {         // checks to see if there is a remainder
            if (t == numOfThreads - 1) { // if it is the last thread, then this is the end
                insertionThread_array[t].end = length - 1;
            }
            else {  // if not then this is the end
                insertionThread_array[t].end = insertionThread_array[t].start + (length / numOfThreads) - 1;
            }
        }
        else {   // if there is no remainder
            insertionThread_array[t].end = length / numOfThreads * insertionThread_array[t].thread_id - 1;
        }
        insertionThread_array[t].array = dupArray;  // copies the address of the main array

        // creating the threads
        rc2 = pthread_create(&insertionThreads[t], NULL, insertionSort, (void *) &insertionThread_array[t]);
        if (rc2) {  // checks to see if the threads are created
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            exit(-1);
        }
    } //endfor

    // joins the threads
    for (int j = 0; j < numOfThreads; j++) {
        pthread_join(insertionThreads[j], NULL);
    }

    // calls the merge function so the array is fully sorted
    insertionSortedArrayMerged = merge(dupArray, numOfThreads, length, insertionThread_array);
    gettimeofday(&endTime, 0);		/* mark the end time */
    timePassed = ((endTime.tv_sec * 1000) + endTime.tv_usec / 1000) - ((startTime.tv_sec * 1000) + startTime.tv_usec / 1000);
    printf("\ntime it took for insertion sort %d milliseconds\n", timePassed);

    char filename_insertion[30];    // file name variable for insertion sort txt
    sprintf(filename_insertion, "[%d]threads_sorted_list2.txt", numOfThreads);  // filename for insertion sort file
    insertionSortFile = fopen(filename_insertion, "w"); // opening the file so it can be written on
    writeFile(insertionSortFile, insertionSortedArrayMerged, length-1); // function to call to write elements of array onto file

    // frees the data from the dynamically allocated arrays and sets the pointers all to null
    free(array);
    free(dupArray);
    free(bubbleSortedArrayMerged);
    free(insertionSortedArrayMerged);
    array = NULL;
    dupArray = NULL;
    bubbleSortedArrayMerged = NULL;
    insertionSortedArrayMerged = NULL;

    return 0;
}
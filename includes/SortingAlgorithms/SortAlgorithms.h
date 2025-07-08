#ifndef SORT_AlGORITHMS_H
#define SORT_AlGORITHMS_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

//swapping two items
template <class T>
void Swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

//defining sorting function names
enum func_names {BUBBLE_SORT, INSERTION_SORT, QUICK_SORT, MERGE_SORT, HEAP_SORT, SORT_F_COUNT};

//defining function pointer types for sort functions and function return types
typedef void (*sort_f_ptr)(int *, unsigned int);
typedef sort_f_ptr (*get_sort_f) (unsigned int);

//implementing bubble sort algorithm
template <class T>
void bubble_sort(T a[], unsigned int size)
{
    int i, j;
    bool swapped;
    for (i = 0; i < size - 1; i++)
    {
        swapped = false;
        for (j = 0; j < size - i - 1; j++)
        {
            if (a[j] > a[j + 1])
            {
                Swap(a[j], a[j + 1]);
                swapped = true;
            }
        }

        //breaking loop if no swaps occurred
        if (swapped == false)
            break;
    }
}

//implementing insertion sort algorithm
template <class T>
void inseration_sort(T a[], unsigned int size)
{
    int i, key, j;
    for (i = 1; i < size; i++) {
        key = a[i];
        j = i - 1;

        //shifting elements greater than key
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = key;
    }
}

//merging two subarrays for merge sort
template <class T>
void merge(T a[], unsigned int s1, unsigned int s2){

    int *temp;
    int copied = 0;
    int copied_1 = 0;
    int copied_2 = 0;
    int i;

    //allocating memory for temporary array
    temp = new int[s1 + s2];

    //merging elements from both subarrays
    while(copied_1 < s1 && copied_2 < s2){
        if(a[copied_1] < (a + s1)[copied_2]){
            temp[copied] = a[copied_1];
            copied++;
            copied_1++;
        }
        else{
            temp[copied] = (a + s1)[copied_2];
            copied++;
            copied_2++;
        }
    }

    //handling remaining elements from first subarray
    while(copied_1 < s1){
        temp[copied] = a[copied_1];
        copied++;
        copied_1++;
    }

    //handling remaining elements from second subarray
    while(copied_2 < s2){
        temp[copied] = (a + s1)[copied_2];
        copied++;
        copied_2++;
    }

    //copying merged elements back to original array
    for(i = 0; i < s1 + s2; i++){
        a[i] = temp[i];
    }

    delete [] temp;
}

int partition(int arr[],int low,int high);

//generating random pivot and partitioning
int partition_r(int arr[], int low, int high);

//implementing recursive merge sort
template <class T>
void mergeSort(T a[], unsigned int const begin, unsigned int const end)
{
    if(begin < end)
    {
        int mid = begin + (end - begin) / 2;
        mergeSort(a, begin, mid);
        mergeSort(a, mid + 1, end);
        merge(a, begin, mid, end);
    }
}

//implementing recursive quick sort
template<class T>
void quickSort(T arr[],int low,int high)
{
  const bool debug = false;
  if(debug)
    cout<<"quickSort() fired.\n";
  if(low<high)
  {
    int pi= partition(arr,low,high);
    quickSort(arr,low,pi-1);
    quickSort(arr,pi+1,high);
  }
}

void heapify(int arr[], int N, int i);

//implementing iterative merge sort
template <class T>
void merge_sort(T a[], unsigned int size){
    int size_1, size_2;

    if(size > 1){

        size_1 = size / 2;
        size_2 = size - size_1;
        merge_sort(a, size_1);
        merge_sort((a + size_1), size_2);

        merge(a, size_1, size_2);
    }
}

//implementing quick sort with recursion
template <class T>
void quick_sort(T a[], unsigned int size)
{
    quickSort(a, 0, size - 1);
}

//implementing heap sort algorithm
template <class T>
void heap_sort(T a[], unsigned int size)
{
    for (int i = size / 2 - 1; i >= 0; i--)
        heapify(a, size, i);

    for (int i = size - 1; i > 0; i--) {

        Swap(a[0], a[i]);

        heapify(a, i, 0);
    }
}
sort_f_ptr get_sort_func_ptr(int func_name);
string print_func_name(int f_name);
int get_sort_func_name(sort_f_ptr f);

#endif //SORT_AlGORITHMS_H

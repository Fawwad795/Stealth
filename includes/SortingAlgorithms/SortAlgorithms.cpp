#ifndef SORT_AlGORITHMS_CPP
#define SORT_AlGORITHMS_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "SortAlgorithms.h"
using namespace std;

//getting the sort function name based on function pointer
int get_sort_func_name(sort_f_ptr f)
{
    //matching the function pointer with predefined sort functions
    if(f == static_cast<sort_f_ptr>(bubble_sort))
        return BUBBLE_SORT;
    if(f == static_cast<sort_f_ptr>(inseration_sort))
        return INSERTION_SORT;
    if(f == static_cast<sort_f_ptr>(quick_sort))
        return QUICK_SORT;
    if(f == static_cast<sort_f_ptr>(merge_sort))
        return MERGE_SORT;
    if(f == static_cast<sort_f_ptr>(heap_sort))
        return HEAP_SORT;

    return -1;
}

//printing the sort function name as a string
string print_func_name(int f_name)
{
    switch (f_name)
    {
    case BUBBLE_SORT:
        return "bubbleSort: ";
    case INSERTION_SORT:
        return "insertionSort: ";
    case MERGE_SORT:
        return "mergeSort: ";
    case QUICK_SORT:
        return "quickSort: ";
    case HEAP_SORT:
        return "heapSort: ";
    default:
        cout<<"default: get_sort_func_ptr\n";
        break;
    }
}

//returning the function pointer for the specified function name
sort_f_ptr get_sort_func_ptr(int func_name)
{
    switch (func_name)
    {
    case BUBBLE_SORT:
        return bubble_sort;
    case INSERTION_SORT:
        return inseration_sort;
    case MERGE_SORT:
        return merge_sort;
    case QUICK_SORT:
        return quick_sort;
    case HEAP_SORT:
        return heap_sort;
    default:
        cout<<"default: get_sort_func_ptr\n";
        break;
    }
}

//partitioning the array for quick sort
int partition(int arr[],int low,int high)
{
  int pivot=arr[high];
  int i=(low-1);

  for(int j=low;j<=high;j++)
  {
    if(arr[j] < pivot)
    {
      i++;
      Swap(arr[i],arr[j]);
    }
  }
  Swap(arr[i+1],arr[high]);
  return (i+1);
}

//partitioning with a random pivot
int partition_r(int arr[], int low, int high)
{
    int random = low + rand() % (high - low);
    Swap(arr[random], arr[high]);
    return partition(arr, low, high);
}

//heapifying the array for heap sort
void heapify(int arr[], int N, int i)
{
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < N && arr[l] > arr[largest])
        largest = l;

    if (r < N && arr[r] > arr[largest])
        largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, N, largest);
    }
}

#endif //SORT_AlGORITHMS_CPP

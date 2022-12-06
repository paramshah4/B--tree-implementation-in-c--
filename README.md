# CS539 - Database System

Programming Project 1 - B+Tree

## Project Description

This programming project is to implement an index for fast data retrieval without having to search through every row in a database table.

I implemented the B+Tree dynamic index structure. It is a balanced tree in which the internal nodes direct the search and leaf nodes contains record pointers to actual data entries. Since the tree structures grows and shrink dynamically, I have implemented the logic of split and merge.


Assuming the keys are unique integers. We simulate the disk pages using two node types and the MAX_FANOUT parameter.

## Build

run the following commands to build the system:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```


## Testing

You can test the individual components of this assignment using our testing file in the (`test/b_plus_tree_test.cpp`), you can modify this file to test and debug the functionality.

We just provide simple sample test cases in the testing file.

```
$ cd build
$ make bplustree-test
$ ./bplustree-test
```

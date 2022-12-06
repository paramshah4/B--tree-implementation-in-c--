# CS539 - Database System

Programming Project 1 - B+Tree

## Project Description

This programming project is to implement an index for fast data retrieval without having to search through every row in a database table.

You will need to implement the B+Tree dynamic index structure. It is a balanced tree in which the internal nodes direct the search and leaf nodes contains record pointers to actual data entries. Since the tree structures grows and shrink dynamically, you are required to handle the logic of split and merge.

We are providing you the *BPlusTree* classes that contain the API that you need to implement. If a class already contains certain member variables, you should **not** remove them. But you are free to add helper functions/member variables to these classes in order to correctly realize the functionality.

You are only allowed to modify the header file (`include/b_plus_tree.h`) and its corresponding source file (`src/b_plus_tree.cpp`).

We assume the keys are unique integers. We simulate the disk pages using two node types and the MAX_FANOUT parameter.

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

We just provide simple sample test cases in the testing file. We will use more complex test cases for grading.

```
$ cd build
$ make bplustree-test
$ ./bplustree-test
```

## Grading Rubric

Each project submission will be graded based on the following criteria:

Does the submission successfully execute all of the test cases and produce the correct answer?
Does the tree nodes satisfy the B+ tree definition? i.e., are they at least half full after insertion and deletion?
The query latency? Are the insertion and deletion very slow?
Tree balance and tree height, etc.

Note that we will use additional test cases that are more complex and go beyond the sample test cases that we provide you.

**WARNING: All of the code for this project must be your own. You may not copy source code from other students or other sources that you find on the web. Plagiarism will not be tolerated. See [Rutgers University Academic Integrity Policy](http://nbacademicintegrity.rutgers.edu/) for additional information.**

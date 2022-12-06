//===----------------------------------------------------------------------===//
//
//                         Rutgers CS539 - Database System
//                         ***DO NO SHARE PUBLICLY***
//
// Identification:   include/b_plus_tree.h
//
// Copyright (c) 2022, Rutgers University
//
//===----------------------------------------------------------------------===//
#pragma once

#include <queue>
#include <string>
#include <vector>
#include "para.h"

using namespace std;

// Value structure we insert into BPlusTree
struct RecordPointer {
  int page_id;
  int record_id;
  RecordPointer() : page_id(0), record_id(0){};
  RecordPointer(int page, int record) : page_id(page), record_id(record){};
};

// BPlusTree Node
class Node {
public:
  Node(bool leaf) : key_num(0), is_leaf(leaf) {
    this->parent = NULL;
  };
  bool is_leaf;
  int key_num;
  KeyType keys[MAX_FANOUT - 1];
  Node* parent; // to store parent of the node
};

// internal b+ tree node
class InternalNode : public Node {
public:
  InternalNode() : Node(false) {};
  Node * children[MAX_FANOUT];
};

class LeafNode : public Node {
public:
  LeafNode() : Node(true) {};
  RecordPointer pointers[MAX_FANOUT - 1];
  // pointer to the next/prev leaf node
  LeafNode *next_leaf = NULL;
  LeafNode *prev_leaf = NULL;
};


/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain record pointers
 * (1) We only support (and test) UNIQUE key
 * (2) Support insert & remove
 * (3) Support range scan, return multiple values.
 * (4) The structure should shrink and grow dynamically
 */

class BPlusTree {
 public:
  BPlusTree(){
    this->root = NULL;
  };

  // Returns true if this B+ tree has no keys and values
  bool IsEmpty() const;

  // Insert a key-value pair into this B+ tree.
  bool Insert(const KeyType &key, const RecordPointer &value);

  // Remove a key and its value from this B+ tree.
  void Remove(const KeyType &key);

  // return the value associated with a given key
  bool GetValue(const KeyType &key, RecordPointer &result);

  // return the values within a key range [key_start, key_end) not included key_end
  void RangeScan(const KeyType &key_start, const KeyType &key_end,
                 std::vector<RecordPointer> &result);
  
  //Helper functions

  Node* findLeafNode(Node* node, KeyType key);
  
  void splitInternalNode(InternalNode *parent_node, InternalNode *new_leaf, const KeyType &key);

  void updateParentInternalNode(Node* remove_node,int index, InternalNode* parent_node);

 private:
  // pointer to the root node.
  Node *root;
};

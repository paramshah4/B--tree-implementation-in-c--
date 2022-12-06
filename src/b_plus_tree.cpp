#include <iostream>
#include <cmath>
using namespace std;
#include "include/b_plus_tree.h"

/*
 * Function to decide whether current bplus tree is empty or not
 */
bool BPlusTree::IsEmpty() const {
    if(root == NULL)
		return true;
	return false; 
}

/* Fucntion to find the leaf node where the given key should be present */
Node* BPlusTree::findLeafNode(Node* node, KeyType key){

    if(node == NULL){
        Node* null_node = NULL;
        return null_node;
    }

    InternalNode* iter = (InternalNode*) node; // cursor finding key

    while(!iter->is_leaf){ // loop till we reach the leaf node where we can insert
        for(int i = 0; i< iter->key_num; i++){ //Loop through keys in the internal node
            if(key < iter->keys[i]){ // go to left children of the key if value is less
                iter = (InternalNode*) iter->children[i];
                break; 
            }
            if(i == (iter->key_num)-1){ // if key is greater than all the keys in the node go to right children of rightmost key
                iter = (InternalNode*) iter->children[i+1];
                break;
            }
        }
    }
    return (Node*) iter;
}

/* Fucntion to insert key to a leaf node */
void insertKeyNode(LeafNode* leaf, const KeyType &key, const RecordPointer &value){
    int i;
    for (i = leaf->key_num-1; (i >= 0 && leaf->keys[i] > key); i--){
        leaf->keys[i + 1] = leaf->keys[i];
        leaf->pointers[i + 1] =  leaf->pointers[i];
    }
    leaf->keys[i + 1] = key;
    leaf->pointers[i + 1] = value;
    leaf->key_num++;
}

/* Binary search */
int binarySearch(KeyType arr[], int low, int high, const KeyType &key)
{
    if (high < low)
        return -1;
    int mid = (low + high) / 2; /*low + (high - low)/2;*/
    if (key == arr[mid])
        return mid;
    if (key > arr[mid])
        return binarySearch(arr, (mid + 1), high, key);
    return binarySearch(arr, low, (mid - 1), key);
}
/* Function to remove the node pointer given node */
void removeNodePointer(Node* node){
    if(node != NULL){
        if(node->is_leaf){
            LeafNode* leaf_node = (LeafNode*) node;
            leaf_node->key_num = 0;
            leaf_node->next_leaf = NULL;
            leaf_node->prev_leaf = NULL;
        }
        else{
            InternalNode* internal_node = (InternalNode*) node;
            for(int i=0; i<internal_node->key_num+1; i++){
                internal_node->children[i] = NULL;
            }
            internal_node->key_num = 0;
        }
    }
}

/* Function to restructure parent internal node while removing a key */
void BPlusTree::updateParentInternalNode(Node* remove_node,int index, InternalNode* parent_node){

    // Case when parent node only has one key so we compress the tree and make the child as root
    if(((Node*) parent_node == root) && (parent_node->key_num == 1)){
        if(remove_node == parent_node->children[0]){
            root = (Node* ) parent_node->children[1];
            parent_node->children[1]->parent = NULL;
            return;
        }
        if(remove_node == parent_node->children[1]){
            root = (Node*) parent_node->children[0];
            parent_node->children[0]->parent = NULL;
            return;
        }
    }

    // remove key if it exists in the parent node pointer 
    for(int i = index; i<parent_node->key_num-1; i++){
        parent_node->keys[i] = parent_node->keys[i+1];
    }

    int remove_node_index = -1;
    for(int i = 0; i<parent_node->key_num+1; i++){
        if(remove_node == parent_node->children[i]){
            remove_node_index = i;
        }
    }
    if(remove_node_index == -1){
        return;
    }
    for(int i = remove_node_index; i<parent_node->key_num; i++){
        parent_node->children[i] = parent_node->children[i+1];
    }
    parent_node->children[parent_node->key_num] = NULL;
    parent_node->key_num--;
    
    // if parent_node is root then return as there will no situation of minimum number of keys
    if(parent_node == (InternalNode*) root){
        return;
    }

    // checking after deleting the key, if parent node has minimum keys
    if (parent_node->key_num + 1 < MAX_FANOUT/2){

        int parent_child_index = -1;
        InternalNode* parent_parent_node = (InternalNode*) parent_node->parent;
        if (parent_parent_node != NULL){
            for(int i=0; i < parent_parent_node->key_num+1; i++){
                if(parent_node == parent_parent_node->children[i]){
                    parent_child_index = i;
                }
            }
        }
        int left_parent_sibling_index = parent_child_index - 1;
        int right_parent_sibling_index = parent_child_index + 1;
        
         /* Checking for MERGING condition if possible with left and right sibling internal nodes  */

         // Checking if left sibling exists 
        if(left_parent_sibling_index >= 0){
            
            InternalNode* left_sibling_parent = (InternalNode*) parent_parent_node->children[left_parent_sibling_index];
            
            // checking if left sibling has keys to merge
            if (left_sibling_parent->key_num + parent_node->key_num + 1 < MAX_FANOUT){
            
            // assigning the parent's key[left_parent_sibling_index] to left_sibling_parent keys array
            left_sibling_parent->keys[left_sibling_parent->key_num] = parent_parent_node->keys[left_parent_sibling_index];
            

            for(int i=0; i<parent_node->key_num; i++){
                left_sibling_parent->keys[left_sibling_parent->key_num+i+1] = parent_node->keys[i];
            }

            // merging the left sibling with current internal node
            for(int i=0; i<parent_node->key_num+1; i++){
                left_sibling_parent->children[left_sibling_parent->key_num+i+1] = parent_node->children[i];
                parent_node->children[i]->parent = left_sibling_parent;
            }

            //removing the children from parent node
            for(int i=0; i<parent_node->key_num+1; i++){
                parent_node->children[i] = NULL;
            }
            // update the left sibling key num after merge
            left_sibling_parent->key_num = left_sibling_parent->key_num + parent_node->key_num + 1;

            updateParentInternalNode(parent_node, left_parent_sibling_index, parent_parent_node);

            return;
            }
        }
        
        if(right_parent_sibling_index <= parent_parent_node->key_num){

            InternalNode* right_sibling_parent = (InternalNode*)parent_parent_node->children[right_parent_sibling_index];
            // checking if right sibling has keys to merge
            if (right_sibling_parent->key_num + parent_node->key_num + 1 < MAX_FANOUT){
            
        // same logic as above for merging left sibling. Only difference is we merge the key with minimum value.
            parent_node->keys[parent_node->key_num] = parent_parent_node->keys[right_parent_sibling_index-1];
            for(int i=0; i<right_sibling_parent->key_num; i++){
                parent_node->keys[parent_node->key_num+1+i] = right_sibling_parent->keys[i];
            }

            for(int i=0; i<right_sibling_parent->key_num+1; i++){
                parent_node->children[parent_node->key_num+i+1] = right_sibling_parent->children[i];
                right_sibling_parent->children[i]->parent = parent_node;
            }
            for(int i=0; i<right_sibling_parent->key_num+1; i++){
                right_sibling_parent->children[i] = NULL;
            }
            parent_node->key_num = right_sibling_parent->key_num+parent_node->key_num+1;

            updateParentInternalNode(right_sibling_parent, right_parent_sibling_index-1, parent_parent_node);

            return;
            }
        }

        /* Checking for LENDING condition if possible with left and right sibling internal nodes  */

        // Checking if left sibling exists 
        if(left_parent_sibling_index >= 0){

            InternalNode* left_sibling_parent = (InternalNode*) parent_parent_node->children[left_parent_sibling_index];

            // checking if left sibling has keys to lend current internal node
            if(left_sibling_parent->key_num > MAX_FANOUT/2){
                
                KeyType keyToInsert = parent_parent_node->keys[left_parent_sibling_index];

                int i,j;
                for (i = parent_node->key_num-1; (i >= 0 && parent_node->keys[i] > keyToInsert); i--){
                    parent_node->keys[i + 1] = parent_node->keys[i];
                }
                parent_node->keys[i + 1] = keyToInsert;
                parent_parent_node->keys[left_parent_sibling_index] = left_sibling_parent->keys[left_sibling_parent->key_num-1];

                Node* child=left_sibling_parent->children[left_sibling_parent->key_num];
                for(j = parent_node->key_num+1; j > 0; j--){
                    parent_node->children[j]=parent_node->children[j-1];
                    parent_node->children[j]->parent = parent_node;
                }
                parent_node->children[j + 1]=child;
                parent_node->children[j + 1]->parent = parent_node;

                parent_node->key_num++;
                left_sibling_parent->key_num--;
                return;
            }
        }
        if (right_parent_sibling_index <= parent_parent_node->key_num){            
            InternalNode* right_sibling_parent = (InternalNode*)parent_parent_node->children[right_parent_sibling_index];
            
            if(right_sibling_parent->key_num > MAX_FANOUT/2){
                
                KeyType keyToInsert = parent_parent_node->keys[parent_child_index];


                int i,j;
                for (i = parent_node->key_num-1; (i >= 0 && parent_node->keys[i] > keyToInsert); i--){
                    parent_node->keys[i + 1] = parent_node->keys[i];
                }
                parent_node->keys[i + 1] = keyToInsert;

                parent_parent_node->keys[parent_child_index] = right_sibling_parent->keys[0];
                parent_node->children[parent_node->key_num+1] = right_sibling_parent->children[0];
                parent_node->children[parent_node->key_num+1]->parent = parent_node;
                for(int i=0; i<right_sibling_parent->key_num;i++){
                    right_sibling_parent->children[i] = right_sibling_parent->children[i+1];
                    right_sibling_parent->keys[i] = right_sibling_parent->keys[i+1];
                }
                right_sibling_parent->children[right_sibling_parent->key_num] = NULL;

                parent_node->key_num++;
                right_sibling_parent->key_num--;
                return;
            }
        }
        
    }
    return;
}

/* Function to split internal node while inserting a key */
 void BPlusTree::splitInternalNode(InternalNode *parent_node, InternalNode *new_leaf, const KeyType &key){

    // checking if parent internal node has space to insert key
	if(parent_node->key_num < MAX_FANOUT - 1){
        int i;
        // shifting the keys and the children
        for (i = parent_node->key_num-1; (i >= 0 && parent_node->keys[i] > key); i--){
            parent_node->keys[i + 1] = parent_node->keys[i];
            parent_node->children[i+2] = (InternalNode*)parent_node->children[i+1];
        }
        // inserting the new key and setting children pointer for that key
		parent_node->keys[i+1] = key;
		parent_node->key_num++;

		parent_node->children[i+2] = (InternalNode*) new_leaf;
        parent_node->children[i+2]->parent = parent_node;
	}
	else{
        // if internal node is full we need to split the internal node as well
        // new internal node after splitting the current internal parent node
		InternalNode *internal_node = new InternalNode();
        internal_node->parent = parent_node->parent; // we are splitting the parent_node so the parent of new internal node will be same as parent of parent_node
        internal_node->is_leaf = false;


		KeyType curr_keys[MAX_FANOUT]; //to store keys of current internal parent node
		InternalNode *curr_child[MAX_FANOUT+1]; //to store children nodes of current internal node key

		for(int i=0; i<MAX_FANOUT-1;i++){
			curr_keys[i] = parent_node->keys[i]; 
		}
		for(int i=0; i<MAX_FANOUT;i++){
            curr_child[i] = (InternalNode*) parent_node->children[i]; 
        }

        int i,j;
        for (i = MAX_FANOUT-2; (i >= 0 && curr_keys[i] > key); i--){
            curr_keys[i + 1] = curr_keys[i];
            curr_child[i+2] = curr_child[i+1];
        }
        curr_keys[i + 1] = key;
        curr_child[i + 2] = (InternalNode*) new_leaf;

		internal_node->key_num = MAX_FANOUT - 1 - ceil(MAX_FANOUT / 2.0);  // setting the size of new internal node after splitting
        parent_node->key_num = ceil(MAX_FANOUT / 2.0); // setting the size of current internal node after splitting
		

         // Storing the new keys and assigning child pointers after splitting in current internal parent node
        for(int i=0;i<parent_node->key_num;i++){ 
            parent_node->keys[i] = curr_keys[i];
            parent_node->children[i] = (InternalNode*)curr_child[i];
            parent_node->children[i]->parent = parent_node;
        }
        parent_node->children[parent_node->key_num] = curr_child[parent_node->key_num];
        parent_node->children[parent_node->key_num]->parent = parent_node;

        // Storing the new keys and assigning child pointers after splitting in new internal parent node
		for(i = 0, j = parent_node->key_num+1; i<internal_node->key_num;i++, j++){
			internal_node->keys[i] = curr_keys[j]; 
            internal_node->children[i]=(InternalNode*)curr_child[j];
            internal_node->children[i]->parent = internal_node;
        } 
        internal_node->children[internal_node->key_num] = (InternalNode*)curr_child[parent_node->key_num + internal_node->key_num + 1];
        internal_node->children[internal_node->key_num]->parent = internal_node;

        // Key at which the internal node is splitted
        KeyType split_key = curr_keys[parent_node->key_num];

        // checking if current internal node has parent 
        // if not , then create a parent internal node for current internal node and set that as root.
        // if present then we again call the split internal function to insert key in internal node
		if((parent_node-> parent == NULL) || (parent_node->parent == nullptr) || (!parent_node->parent)){

            InternalNode *parent_internal_node = new InternalNode;
            parent_internal_node->keys[0] = split_key;
            parent_internal_node->key_num = 1;
            parent_internal_node->is_leaf = false;
            
            parent_internal_node->children[0] = parent_node;
            parent_internal_node->children[1] = internal_node;

            parent_node->parent = parent_internal_node;
            internal_node->parent = parent_internal_node;

            root = (Node*)parent_internal_node;
		}
		else{
			splitInternalNode((InternalNode*) parent_node->parent, internal_node, split_key);
		}	
	}
}
/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */

bool BPlusTree::GetValue(const KeyType &key, RecordPointer &result) {

    LeafNode*  leaf_node = (LeafNode*)findLeafNode(root, key);
    if(leaf_node == NULL)
        return false;
    int index = binarySearch(leaf_node->keys,0,leaf_node->key_num,key);
    if(index >= 0 and index < leaf_node->key_num){
        result.page_id = leaf_node->pointers[index].page_id;
        result.record_id = leaf_node->pointers[index].record_id;
        return true;
    }
    return false;	
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * If current tree is empty, start new tree, otherwise insert into leaf Node.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */

bool BPlusTree::Insert(const KeyType &key, const RecordPointer &value) {

    //checking if tree is empty
    // if empty create new leaf node and insert the key
    if(IsEmpty()){
        LeafNode *leaf = new LeafNode();
        leaf->is_leaf = true;
        leaf->key_num = 1;
        leaf->keys[0] = key;
        leaf->parent = NULL;
		leaf->pointers[0] = value;
		root = leaf; // setting this leaf node to root as this is the first leaf node created.
		return true;
    }

    // checking if a key is already present ,if present then return false
    RecordPointer pres_value=value;
    if (GetValue(key, pres_value))
    {
        return false;
    }



    // Getting the leaf node where the key can be inserted
    LeafNode *leaf = (LeafNode*) findLeafNode(root, key); 

    if(leaf->key_num < MAX_FANOUT-1){  // checking if num_keys are maximum in the node
        insertKeyNode(leaf, key, value);
    }
    else{
        // need to split the leaf node as number of keys are maximum.
        // creating temporary keys array to find and split the node from midpoint
        KeyType curr_keys[MAX_FANOUT]; //to store keys of current leaf node
        RecordPointer curr_ptr[MAX_FANOUT]; //to store values current leaf node
       
        for(int i = 0; i < MAX_FANOUT-1; i++){
            curr_keys[i] = leaf->keys[i];
            curr_ptr[i] = leaf->pointers[i];
        }
        // find index of position where we can insert the key in leaf node

        int i;
        for (i = MAX_FANOUT-2; (i >= 0 && curr_keys[i] > key); i--){
            curr_keys[i + 1] = curr_keys[i];
            curr_ptr[i + 1] =  curr_ptr[i];
        }
        curr_keys[i + 1] = key;
        curr_ptr[i + 1] = value;
 
        //  creating a new leaf node after splitting
        LeafNode *new_leaf = new LeafNode;
        new_leaf->is_leaf = true;
        new_leaf->parent = leaf->parent;

        //Connecting the two leaf nodes together
        new_leaf->next_leaf = leaf->next_leaf;
        leaf->next_leaf = new_leaf;
        new_leaf->prev_leaf = leaf;

        new_leaf->key_num = MAX_FANOUT - ceil(MAX_FANOUT / 2.0); //setting size of new leaf node after splitting

        leaf->key_num = ceil(MAX_FANOUT / 2.0); //get midpoint of current leaf node
        
        // storing the values in new leaf node and current leaf node after splitting
        for(i = 0;i < leaf->key_num; i++){
            leaf->keys[i] = curr_keys[i];
            leaf->pointers[i] = curr_ptr[i];
        }
        int j;
        for(i = 0, j = leaf->key_num; i < new_leaf->key_num; i++, j++){
            new_leaf->keys[i] = curr_keys[j];
            new_leaf->pointers[i] = curr_ptr[j];
        }

        // Key at which the leaf node are splitted
        KeyType split_key = new_leaf->keys[0];

        // checking if internal node parent is already present or not
        //if not present create an internal parent node for the two leaf nodes created right now
        // if present, check if parent node has space else split parent node as well
        if(leaf->parent == NULL){
            
            InternalNode *internal_node = new InternalNode();
            internal_node->is_leaf = false;
            internal_node->keys[0] = split_key;
            internal_node->key_num = 1;
                        
            internal_node->children[0] = leaf;
            internal_node->children[1] = new_leaf;

            leaf->parent = internal_node;
            new_leaf->parent = internal_node;

            root = internal_node;
        }
        else{
			splitInternalNode((InternalNode*) leaf->parent, (InternalNode*) new_leaf, split_key);
        }
    }
    return true;
}




/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immdiately.
 * If not, User needs to first find the right leaf node as deletion target, then
 * delete entry from leaf node. Remember to deal with redistribute or merge if
 * necessary.
 */


void BPlusTree::Remove(const KeyType &key) {
    
    // Checking if tree is empty
    // if empty we just return as we key is not present
    if(IsEmpty()){
        return;
    }

    RecordPointer value;
    //checking if key is present in the tree
    if(!GetValue(key, value)){
        return;
    }

    // Getting the leaf node where the key is present
    LeafNode *leaf = (LeafNode*) findLeafNode(root, key); 

    // finding index of key in leaf node
    int key_leaf_index = -1; // the delindex will store the index in the leaf node from which we have to delete the key
    for (int i = 0; i < leaf->key_num; i++)
    {
        // loop over all the keys and see if the key is present or not
        if (leaf->keys[i] == key)
        {
            key_leaf_index = i;
            break;
        }
    }

    // removing the key from the leaf node
    for (int i = key_leaf_index; i < leaf->key_num - 1; i++)
    {
        leaf->keys[i] = leaf->keys[i + 1];
        leaf->pointers[i] = leaf->pointers[i + 1];
    }
    leaf->key_num--;

    //check if leaf is root node
    // then we just delete the key from leaf node and return
    // if after deleting key, there are no keys left then we clear the root node memory and assign root null
    if (leaf == (LeafNode *)root)
    {
        if(leaf->key_num == 0){
            removeNodePointer((Node *)root);
            root = NULL; 
            return; 
        }
        return;
    }

    // checking after deleting the key, if leaf node has minimum keys
    if (leaf->key_num < MAX_FANOUT / 2)
    {   
        //parent of leaf node
        InternalNode *parent = (InternalNode *)leaf->parent;
        // finding the index of leaf node in parent's child pointer array
        // for merging keys
        int leaf_child_index = -1;
        if (parent != NULL)
        {
            for (int i = 0; i < parent->key_num + 1; i++)
            {
                if (leaf == (LeafNode *) parent->children[i])
                {
                    leaf_child_index = i;
                    break;
                }
            }
        }
        
        // child array index of left and right sibling of the leaf node
        int left_sibling_child_index = leaf_child_index - 1;
        int right_sibling_child_index = leaf_child_index + 1;

        /* Checking for MERGING condition if possible with left and right sibling leaf nodes  */
        
        // Checking if left sibling exists 
        if (left_sibling_child_index >= 0)
        { 
            LeafNode *left_sibling_leaf = leaf->prev_leaf;

            // checking if left sibling has keys to merge
            if (left_sibling_leaf->key_num + leaf->key_num < MAX_FANOUT)
            {
                // merging the leaf node with its left sibling
                for (int i = 0; i < leaf->key_num; i++)
                {
                    left_sibling_leaf->keys[left_sibling_leaf->key_num + i] = leaf->keys[i];
                    left_sibling_leaf->pointers[left_sibling_leaf->key_num + i] = leaf->pointers[i];
                }
                // updating the left sibling class member values
                left_sibling_leaf->key_num = left_sibling_leaf->key_num + leaf->key_num;
                left_sibling_leaf->next_leaf = leaf->next_leaf;

                if (left_sibling_leaf->next_leaf != NULL || left_sibling_leaf->next_leaf != nullptr)
                {
                    left_sibling_leaf->next_leaf->prev_leaf = left_sibling_leaf;
                }

                updateParentInternalNode(leaf, left_sibling_child_index, parent);
                removeNodePointer((Node *) leaf);
                return;
            }
        }
        // Checking if right sibling exists 
        if (right_sibling_child_index <= parent->key_num)
        {
            LeafNode *right_sibling_leaf = leaf->next_leaf;

           // checking if right sibling has keys to merge
            if (right_sibling_leaf->key_num + leaf->key_num < MAX_FANOUT)
            {
                // merging the leaf node with its right sibling
                for (int i = 0; i < right_sibling_leaf->key_num; i++)
                {
                    leaf->keys[i + leaf->key_num] = right_sibling_leaf->keys[i];
                    leaf->pointers[i + leaf->key_num] = right_sibling_leaf->pointers[i];
                }
                // updating the right sibling class member values
                leaf->key_num = leaf->key_num + right_sibling_leaf->key_num;
                leaf->next_leaf = right_sibling_leaf->next_leaf;

                if (right_sibling_leaf->next_leaf != NULL || right_sibling_leaf->next_leaf != nullptr)
                    right_sibling_leaf->next_leaf->prev_leaf = leaf;

                updateParentInternalNode(right_sibling_leaf, right_sibling_child_index - 1, parent);
                removeNodePointer((Node *) right_sibling_leaf);
                return;
            }
        }

          /* Checking for LENDING condition if possible with left and right sibling leaf nodes 
          after MERGING condition did not satisfy */

        // Checking if left sibling exists 
        if (left_sibling_child_index >= 0)
        {

            LeafNode *left_sibling_leaf = leaf->prev_leaf;

             // checking if left sibling has keys to lend current leaf node
            if (left_sibling_leaf->key_num > MAX_FANOUT / 2) // check if left sibling has enough data
            {
                // key and pointer value to lend from left sibling to current leaf node
                // we will lend the maximum value key from left sibling and update parent key accordingly.
                KeyType keyToInsert = left_sibling_leaf->keys[left_sibling_leaf->key_num - 1];
                RecordPointer pointerToInsert = left_sibling_leaf->pointers[left_sibling_leaf->key_num - 1];

                // inserting key and pointer at specific location in keys array of leaf node
                int i;
                for (i = leaf->key_num-1; (i >= 0 && leaf->keys[i] > keyToInsert); i--){
                    leaf->keys[i + 1] = leaf->keys[i];
                    leaf->pointers[i + 1] =  leaf->pointers[i];
                }
                leaf->keys[i + 1] = keyToInsert;
                leaf->pointers[i + 1] = pointerToInsert;
                leaf->key_num++;
                left_sibling_leaf->key_num--;

                // updating parent keys array after lending the key
                parent->keys[left_sibling_child_index] = leaf->keys[0];
                return;
            }
        }
         // Checking if right sibling exists 
        if (right_sibling_child_index <= parent->key_num)
        {
            LeafNode *right_sibling_leaf = leaf->next_leaf;

            // checking if right sibling has keys to merge
            if (right_sibling_leaf->key_num > MAX_FANOUT / 2)
            {
                // key and pointer value to lend from left sibling to current leaf node
                // we will lend the minimum value key from right sibling and update parent key accordingly.
                KeyType keyToInsert = right_sibling_leaf->keys[0];
                RecordPointer pointerToInsert = right_sibling_leaf->pointers[0];
                
                // inserting key and pointer at specific location in keys array of leaf node
                int i;
                for (i = leaf->key_num-1; (i >= 0 && leaf->keys[i] > keyToInsert); i--){
                    leaf->keys[i + 1] = leaf->keys[i];
                    leaf->pointers[i + 1] =  leaf->pointers[i];
                }
                leaf->keys[i + 1] = keyToInsert;
                leaf->pointers[i + 1] = pointerToInsert;
                leaf->key_num++;

                // updating the right sibling keys after lending key to current leaf node
                for (int i = 0; i < right_sibling_leaf->key_num - 1; i++)
                {
                    right_sibling_leaf->keys[i] = right_sibling_leaf->keys[i + 1];
                    right_sibling_leaf->pointers[i] = right_sibling_leaf->pointers[i + 1];
                }
                right_sibling_leaf->key_num--;

                 // updating parent keys array after lending the key
                parent->keys[right_sibling_child_index - 1] = right_sibling_leaf->keys[0];
                return;
            }
        }
    }
    return;
    
}

/*****************************************************************************
 * RANGE_SCAN
 *****************************************************************************/
/*
 * Return the values that within the given key range
 * First find the node large or equal to the key_start, then traverse the leaf
 * nodes until meet the key_end position, fetch all the records.
 */
void BPlusTree::RangeScan(const KeyType &key_start, const KeyType &key_end,std::vector<RecordPointer> &result) 
{
    LeafNode* leaf_node = (LeafNode*) findLeafNode(root, key_start);
    bool flag = true;

    while((leaf_node != NULL) && (flag)){
        for(int i=0;i<leaf_node->key_num;i++){
            if((leaf_node->keys[i] >= key_start) && (leaf_node->keys[i] <= key_end))
                result.push_back(leaf_node->pointers[i]);
            else if(leaf_node->keys[i] > key_end){
                flag = false;
                break;
            }
        }
        leaf_node = leaf_node->next_leaf;
    }
}

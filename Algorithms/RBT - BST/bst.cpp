// AUTHOR: EREN CULHACI
// ID: 150220763

// EB
// Implementing Binary Search Tree in C++

#include <iostream>

namespace BST {
  // Node structure
  struct Node {
    int data;
    std::string name;
    Node *parent;
    Node *left;
    Node *right;
  };
}

class BinarySearchTree {
private:
  BST::Node *root;  // Root of the tree
 
//////////////////////// HELPER FUNCTIONS //////////////////////////
  void preOrder(BST::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) {
    if (node == nullptr) {
      //base case when node is null
      return; 
    }
    orderedDataBST[index].first = node->name; //store the name in the array
    orderedDataBST[index].second = node->data;  //store the data in the array
    index++; //increment the index
    preOrder(node->left, orderedDataBST, index);  //traverse the left subtree and add the data to the array
    preOrder(node->right, orderedDataBST, index); //traverse the right subtree and add the data to the array
  }

  void inOrder(BST::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) {
    if (node == nullptr) {
      //base case when node is null
      return;
    }
    inOrder(node->left, orderedDataBST, index); //traverse the left subtree and add the data to the array
    orderedDataBST[index].first = node->name; //store the name in the array
    orderedDataBST[index].second = node->data;//store the data in the array
    index++;  //increment the index
    inOrder(node->right, orderedDataBST, index);  //traverse the right subtree and add the data to the array
  }

  void postOrder(BST::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) { //traverse the tree in postorder and add the data to the array
    if (node == nullptr) {
      //base case when node is null
      return;
    }
    postOrder(node->left, orderedDataBST, index); //traverse the left subtree and add the data to the array
    postOrder(node->right, orderedDataBST, index);  //traverse the right subtree and add the data to the array
    orderedDataBST[index].first = node->name; //store the name in the array
    orderedDataBST[index].second = node->data;  //store the data in the array
    index++;  //increment the index
  }

  int calculateHeight(BST::Node* node) {
    if (node == nullptr) {
      //base case when node is null
      return 0; //return 0 as the height
    }

      int leftHeight = calculateHeight(node->left); //calculate the height of the left subtree
      int rightHeight = calculateHeight(node->right); //calculate the height of the right subtree

      return 1 + std::max(leftHeight, rightHeight); //return the maximum height of the subtrees
    }

  int getTotalNodes(BST::Node *node) {
    if (node == nullptr) {
      //base case when node is null return 0 as the number of nodes
      return 0;
    }
    return 1 + getTotalNodes(node->left) + getTotalNodes(node->right); //return the total number of nodes by adding the number of nodes in the left and right subtrees and adding 1 for the current node
  }
  /////////////////////////////////////////////////////////////////////////

public:
  BinarySearchTree() { //constructor
    root = nullptr; //initialize the root as null
  }

  void preorder(std::pair<std::string, int> orderedDataBST[], int index) {
    preOrder(root, orderedDataBST, index); //call the helper function to traverse the tree in preorder and add the data to the array by passing the root
  }
  
  void inorder(std::pair<std::string, int> orderedDataBST[], int index) {
    inOrder(root, orderedDataBST, index); //call the helper function to traverse the tree in inorder and add the data to the array by passing the root
  }
  
  void postorder(std::pair<std::string, int> orderedDataBST[], int index) {
    postOrder(root, orderedDataBST, index); //call the helper function to traverse the tree in postorder and add the data to the array by passing the root
  }

  BST::Node* searchTree(int key) { //search the tree for the given key
    BST::Node *node = root; //start from the root
    while (node != nullptr) { //traverse the tree until the node is null
      if (key < node->data) { //if the key is smaller than the current node's data, traverse the left subtree
        node = node->left; //go to the left child
      } else if (key > node->data) { //if the key is greater than the current node's data, traverse the right subtree
        node = node->right; //go to the right child
      } else {  //if the key is equal to the current node's data, return the node
        return node;  //return the node
      }
    }
    return node;  //return null if the key is not found
  }

  BST::Node* successor(BST::Node *node) { //find the successor of the given node
  if (node->right != NULL) {  //if the node has a right child, find the minimum value in the right subtree
        BST::Node* temp = node->right; //start from the right child
        while (temp->left != NULL) {  //traverse the left subtree until the left child is null
            temp = temp->left;  //go to the left child
        }
        return temp;  //return the minimum value
    }

    BST::Node* succ = NULL; //if the node does not have a right child, find the ancestor of the node that is the left child of its parent
    BST::Node* ancestor = root; //start from the root
    while (ancestor != node) {  //traverse the tree until the node is found
        if (node->data < ancestor->data) {  //if the node's data is smaller than the ancestor's data, the ancestor is the successor
            succ = ancestor;  //set the successor as the ancestor
            ancestor = ancestor->left;  //go to the left child
        }
        else  //if the node's data is greater than the ancestor's data, the successor is the ancestor's successor
            ancestor = ancestor->right; //go to the right child
    }

    return succ;  //return the successor
}

  BST::Node* predecessor(BST::Node *node) {
    // If the left subtree is not null, the predecessor is the maximum value in the left subtree
    if (node->left != NULL) {
        BST::Node* temp = node->left;
        while (temp->right != NULL) {
            temp = temp->right;
        }
        return temp;
    }

    BST::Node* pred = NULL; // If the left subtree is null, find the ancestor of the node that is the right child of its parent
    BST::Node* ancestor = root; // Start from the root
    while (ancestor != node) { // Traverse the tree until the node is found
        if (node->data > ancestor->data) { // If the node's data is greater than the ancestor's data, the ancestor is the predecessor
            pred = ancestor; // Set the predecessor as the ancestor
            ancestor = ancestor->right; // Go to the right child
        }
        else // If the node's data is smaller than the ancestor's data, the predecessor is the ancestor's predecessor
            ancestor = ancestor->left; // Go to the left child
    }

    return pred; // Return the predecessor
  }

  void insert(std::string datastr, int key) { //insert the given key and data to the tree
    BST::Node *node = new BST::Node;  
    //create a new node and set its data and name as the given key and data and set its parent, left and right children as null
    node->data = key; 
    node->name = datastr; 
    node->parent = nullptr;
    node->left = nullptr;
    node->right = nullptr;

    BST::Node *temp = root; //start from the root
    if (temp == nullptr) { //if the root is null, set the root as the new node
        root = node; //set the root as the new node
        return; //return
    }
    while (temp != nullptr) { //traverse the tree until the node is null
        node->parent = temp; //set the parent of the node as the current node
        if (node->data == temp->data) { //if the value is the same as the current node's data
            // Insert as the right child when encountering the same value
            temp = temp->right;
        } else if (node->data < temp->data) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }
    // If the value is the same as the parent, insert as the right child
    if (node->data >= node->parent->data) {
        node->parent->right = node;
    } else {
        node->parent->left = node;
    }
}

  void deleteNode(int key) {
    BST::Node *node = searchTree(key); //search the tree for the given key
    if (node == nullptr) { //if the node is null, return
      return; 
    }
    if (node->left == nullptr && node->right == nullptr) { //if the node is a leaf node, delete the node
      if (node->parent->left == node) {// if the node is the left child of its parent, set the left child as null
        node->parent->left = nullptr;
      } else { // if the node is the right child of its parent, set the right child as null
        node->parent->right = nullptr;
      }
      delete node;

    } else if (node->left == nullptr) {//if the node has only one child, delete the node and set its child as the child of its parent
      if (node->parent->left == node) { //if the node is the left child of its parent
        node->parent->left = node->right; //set the left child as the child of its parent
      } else { //if the node is the right child of its parent
        node->parent->right = node->right; //set the right child as the child of its parent
      }
      delete node;

    } else if (node->right == nullptr) {  //if the node has only one child, delete the node and set its child as the child of its parent
      if (node->parent->left == node) { //if the node is the left child of its parent
        node->parent->left = node->left;  //set the left child as the child of its parent
      } else {  //if the node is the right child of its parent
        node->parent->right = node->left; //set the right child as the child of its parent
      }
      delete node;

    } else { //if the node has two children, find the successor of the node and replace the node with the successor
      BST::Node *succ = successor(node);  //find the successor of the node
      node->data = succ->data;  //replace the node's data with the successor's data
      node->name = succ->name; //replace the node's name with the successor's name
      if (succ->parent->left == succ) { //if the successor is the left child of its parent, set the left child as null
        succ->parent->left = nullptr; //set the left child as null
      } else {  //if the successor is the right child of its parent, set the right child as null
        succ->parent->right = nullptr; //set the right child as null
      }
      delete succ; //delete the successor
    }
  }

  int getHeight() { 
    return calculateHeight(root) - 1; //return the height of the tree by calling the helper function with giving root as parameter and subtracting 1 from the height because the root is not counted.
}


  BST::Node* getMaximum() {
    BST::Node *max = root; //start from the root
    while (max->right != nullptr) { //traverse the tree until the right child is null
      max = max->right; //go to the right child
    }
    return max; //return the maximum value
  }

  BST::Node* getMinimum() {
    BST::Node *min = root; //start from the root
    while (min->left != nullptr) { //traverse the tree until the left child is null
      min = min->left; //go to the left child
    }
    return min; //return the minimum value
  }

  int getTotalNodes() {
    return getTotalNodes(root); //call the helper function with giving root as parameter
  }


};
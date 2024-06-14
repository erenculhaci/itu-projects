// AUTHOR: EREN CULHACI
// ID: 150220763

// EB
// Implementing Red-Black Tree in C++

#include <iostream>

namespace RBT {
  struct Node {
    int data; //population data 
    std::string name; //data name
    Node *parent; //pointer to parent node
    Node *left; //pointer to left child
    Node *right;//pointer to right child
    int color;//1->red, 0->black
  };
}

class RedBlackTree {
private:
  RBT::Node *root; //pointer to root node

//////////////////////// HELPER FUNCTIONS //////////////////////////
  void preOrder(RBT::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) {
    //adding elements to array in preorder. 
    if (node == nullptr) { //if node is null, return
      return;
    }
    orderedDataBST[index].first = node->name; //adding name to array
    orderedDataBST[index].second = node->data; //adding data to array
    index++; //incrementing index
    preOrder(node->left, orderedDataBST, index); //traversing left subtree
    preOrder(node->right, orderedDataBST, index); //traversing right subtree
  }

  void inOrder(RBT::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) {
    //adding elements to array in inorder.
    if (node == nullptr) {
      return; //if node is null, return
    }
    inOrder(node->left, orderedDataBST, index); //traversing left subtree
    orderedDataBST[index].first = node->name; //adding name to array
    orderedDataBST[index].second = node->data; //adding data to array
    index++; //incrementing index
    inOrder(node->right, orderedDataBST, index); //traversing right subtree
  }

  void postOrder(RBT::Node *node, std::pair<std::string, int> orderedDataBST[], int& index) {
    //adding elements to array in postorder.
    if (node == nullptr) {
      return; //if node is null, return
    }
    postOrder(node->left, orderedDataBST, index);//traversing left subtree
    postOrder(node->right, orderedDataBST, index);//traversing right subtree
    orderedDataBST[index].first = node->name;//adding name to array
    orderedDataBST[index].second = node->data;//adding data to array
    index++;//incrementing index
  }

  void fixInsert(RBT::Node *z) {
    while (z->parent && z->parent->color == 1) { // Check if parent color is red
      if (z->parent == z->parent->parent->left) { // If parent is left child
        RBT::Node *y = z->parent->parent->right; // y is uncle
        if (y && y->color == 1) { // Check if uncle is red
          z->parent->color = 0; // Change parent color to black
          y->color = 0; // Change uncle color to black
          z->parent->parent->color = 1; // Change grandparent color to red
          z = z->parent->parent; // z is grandparent
        } else { // If uncle is black
          if (z == z->parent->right) { // If z is right child
            z = z->parent; // z is parent
            // Left rotate
            leftRotate(z);
          }
          z->parent->color = 0; // Change parent color to black
          z->parent->parent->color = 1; // Change grandparent color to red
          // Right rotate
          rightRotate(z->parent->parent); // Right rotate
        }
      } else { // If parent is right child
        RBT::Node *y = z->parent->parent->left; // y is uncle
        if (y && y->color == 1) { // Check if uncle is red
          z->parent->color = 0; // Change parent color to black
          y->color = 0; // Change uncle color to black
          z->parent->parent->color = 1; // Change grandparent color to red
          z = z->parent->parent; // z is grandparent
        } else { // If uncle is black
          if (z == z->parent->left) {// If z is left child
            z = z->parent; // z is parent
            // Right rotate
            rightRotate(z);
          }
          z->parent->color = 0; // Change parent color to black
          z->parent->parent->color = 1; // Change grandparent color to red
          // Left rotate
          leftRotate(z->parent->parent);// Left rotate
        }
      }
    }
    root->color = 0; // Change root color to black
  }

  // Helper function for left rotation
  void leftRotate(RBT::Node *x) { 
    RBT::Node *y = x->right; // y is the right child of x
    x->right = y->left; // y's left subtree becomes x's right subtree
    if (y->left) {// Check if y's left subtree is not null
      y->left->parent = x; // Change y's left subtree's parent to x
    }
    y->parent = x->parent; // Change y's parent to x's parent
    if (!x->parent) { // Check if x's parent is null
      root = y; // y becomes root
    } else if (x == x->parent->left) { // Check if x is left child
      x->parent->left = y; // y becomes x's parent's left child
    } else { // If x is right child
      x->parent->right = y; // y becomes x's parent's right child
    }
    y->left = x; // x becomes y's left child
    x->parent = y; // x's parent becomes y
  }

  // Helper function for right rotation
  void rightRotate(RBT::Node *y) {
    RBT::Node *x = y->left; // x is the left child of y
    y->left = x->right; // x's right subtree becomes y's left subtree
    if (x->right) { // Check if x's right subtree is not null
      x->right->parent = y; // Change x's right subtree's parent to y
    }
    x->parent = y->parent; // Change x's parent to y's parent
    if (!y->parent) { // Check if y's parent is null
      root = x; // make x as root
    } else if (y == y->parent->left) { // Check if y is left child
      y->parent->left = x; // x becomes y's parent's left child
    } else { // If y is right child
      y->parent->right = x; // x becomes y's parent's right child
    }
    x->right = y; // y becomes x's right child
    y->parent = x;// y's parent becomes x
  }

    // Helper function to replace one subtree with another
  void transplant(RBT::Node *u, RBT::Node *v) { // Replace u with v
    if (!u->parent) { // Check if u's parent is null
      root = v; // v becomes root
    } else if (u == u->parent->left) { // Check if u is left child
      u->parent->left = v; // v becomes u's parent's left child
    } else { // If u is right child
      u->parent->right = v; // v becomes u's parent's right child
    }
    if (v) { // Check if v is not null
      v->parent = u->parent; // Change v's parent to u's parent
    }
  }

  RBT::Node* treeMinimum(RBT::Node *node) {
    while (node->left) { // if node's left child is not null
      node = node->left; // node becomes node's left child
    }
    return node; // return node which is the minimum
  }

// Helper function to fix the RB tree after deleting a node
void fixDelete(RBT::Node *x) { // x is the node to be deleted or replaced
  while (x != root && (!x || x->color == 0)) { // Check if x is not root and x is black
    if (x == x->parent->left) {   // If x is left child
      RBT::Node *w = x->parent->right;  // w is x's sibling
      if (w && w->color == 1) { // Check if w is red
        w->color = 0;   // Change w's color to black
        x->parent->color = 1; // Change x's parent's color to red
        leftRotate(x->parent); // Left rotate
        w = x->parent->right; // w is x's sibling
      }
      if ((!w->left || w->left->color == 0) && (!w->right || w->right->color == 0)) {
        // Check if both of w's children are black
        w->color = 1; // Change w's color to red
        x = x->parent; // x becomes x's parent
      } else {  // If at least one of w's children is red
        if (!w->right || w->right->color == 0) { // Check if w's right child is black
          if (w->left) w->left->color = 0; // Change w's left child's color to black
          w->color = 1; // Change w's color to red
          rightRotate(w); // Right rotate
          w = x->parent->right; // w is x's sibling
        }
        w->color = x->parent->color; // Change w's color to x's parent's color
        x->parent->color = 0;  // Change x's parent's color to black
        if (w->right) w->right->color = 0; // Change w's right child's color to black
        leftRotate(x->parent); // Left rotate
        x = root; // x becomes root
      }
    } else { // If x is right child
      RBT::Node *w = x->parent->left;   // w is x's sibling
      if (w && w->color == 1) {   // Check if w is red
        w->color = 0; // Change w's color to black
        x->parent->color = 1; // Change x's parent's color to red
        rightRotate(x->parent); // Right rotate
        w = x->parent->left; // w is x's sibling
      }
      if ((!w->right || w->right->color == 0) && (!w->left || w->left->color == 0)) {
        // Check if both of w's children are black
        w->color = 1; // Change w's color to red
        x = x->parent; // x becomes x's parent
      } else { // If at least one of w's children is red
        if (!w->left || w->left->color == 0) {  // Check if w's left child is black
          if (w->right) w->right->color = 0;    // Change w's right child's color to black
          w->color = 1;   // Change w's color to red
          leftRotate(w);    // Left rotate
          w = x->parent->left;    // w is x's sibling
        }
        w->color = x->parent->color;    // Change w's color to x's parent's color
        x->parent->color = 0; // Change x's parent's color to black
        if (w->left) w->left->color = 0;  // Change w's left child's color to black
        rightRotate(x->parent); // Right rotate
        x = root; // x becomes root
      }
    }
  }
  if (x) x->color = 0;    // Change x's color to black
}

  int calculateHeight(RBT::Node* node) {
      if (node == nullptr) {
        // If node is null, return height 0
        return 0;
      }
      int leftHeight = calculateHeight(node->left); // Calculate left subtree's height by recursively calling calculateHeight
      int rightHeight = calculateHeight(node->right); // Calculate right subtree's height by recursively calling calculateHeight
      return std::max(leftHeight, rightHeight) + 1; // Return the maximum of leftHeight and rightHeight plus 1 for the current node
    }
  
  int getTotalNodes(RBT::Node* node) { 
    if (node == nullptr) {
      // If node is null, return height 0
      return 0;
    }
    return 1 + getTotalNodes(node->left) + getTotalNodes(node->right); // Return 1 plus the total number of nodes in left subtree plus the total number of nodes in right subtree
  }
////////////////////////////////////////////////////////////////////
public:
  RedBlackTree() {
    // Constructor setting root to null
    root = nullptr;
  }

  void preorder(std::pair<std::string, int> orderedDataBST[], int index) {
    preOrder(root, orderedDataBST, index); // Call preOrder function with root as parameter
  }
  
  void inorder(std::pair<std::string, int> orderedDataBST[], int index) {
    inOrder(root, orderedDataBST, index); // Call inOrder function with root as parameter
  }
  
  void postorder(std::pair<std::string, int> orderedDataBST[], int index) {
    postOrder(root, orderedDataBST, index); // Call postOrder function with root as parameter
  }

  RBT::Node* searchTree(int key) {
    RBT::Node* temp = root; // Create a temporary node and set it to root
    while (temp != nullptr) { // While temp is not null
      if (key == temp->data) { // If key is equal to temp's data
        return temp; // Return temp
      }
      else if (key < temp->data) { // If key is less than temp's data
        temp = temp->left; // Set temp to temp's left child
      }
      else { // If key is greater than temp's data
        temp = temp->right; // Set temp to temp's right child
      }
    }
    return nullptr; // Return null if key is not found
  }

  RBT::Node* successor(RBT::Node* node) {   // Finding successor of a node
    if (node->right != nullptr) { // If node's right child is not null
      RBT::Node* temp = node->right; // Create a temporary node and set it to node's right child
      while (temp->left != nullptr) { // While temp's left child is not null
        temp = temp->left; // Set temp to temp's left child
      }
      return temp; // Return temp
    }
    RBT::Node* temp = node->parent; // Create a temporary node and set it to node's parent
    while (temp != nullptr && node == temp->right) { // While temp is not null and node is temp's right child
      node = temp; // Set node to temp
      temp = temp->parent; // Set temp to temp's parent
    }
    return temp; // Return temp
  }

  RBT::Node* predecessor(RBT::Node* node) { // Finding predecessor of a node
    if (node->left != nullptr) { // If node's left child is not null
      RBT::Node* temp = node->left; // Create a temporary node and set it to node's left child
      while (temp->right != nullptr) { // While temp's right child is not null
        temp = temp->right; // Set temp to temp's right child
      }
      return temp;  // Return temp
    }
    RBT::Node* temp = node->parent; // Create a temporary node and set it to node's parent
    while (temp != nullptr && node == temp->left) { // While temp is not null and node is temp's left child
      node = temp; // Set node to temp
      temp = temp->parent; // Set temp to temp's parent
    }
    return temp; // Return temp
  }

  void insert(std::string datastr, int key) {
    RBT::Node *z = new RBT::Node; // Create a new node
    // Set node's data, name, left child, right child, parent and color
    z->data = key;
    z->name = datastr;
    z->left = nullptr;
    z->right = nullptr;
    z->parent = nullptr;
    z->color = 1; // Red

    RBT::Node *y = nullptr; // y is the parent of x
    RBT::Node *x = root;    // x is the current node

    while (x) { // While x is not null
      y = x; // y becomes x
      if (z->data < x->data) { // Insert as left child if z's data is less than x's data
        x = x->left;  // x becomes x's left child
      } else if (z->data == x->data) { // If values are the same
        // Insert as right child if values are the same
        x = x->right;
      } else { // Insert as right child if z's data is greater than x's data
        x = x->right;
      }
    }

    z->parent = y;  // z's parent becomes y
    if (!y) { // Check if y is null
      root = z; // Tree was empty
    } else if (z->data < y->data) { // Insert as left child if z's data is less than y's data
      y->left = z; // z becomes y's left child
    } else { // Insert as right child if z's data is greater than y's data
      y->right = z; // z becomes y's right child
    }

    fixInsert(z); // Fix the tree by calling fixInsert function with z as parameter
  }

    void deleteNode(int key) {    // Delete a node with a given key
    RBT::Node *z = root;    // z is the node to be deleted
    while (z) {   // While z is not null
        if (key < z->data) {    // If key is less than z's data
            z = z->left; // z becomes z's left child
        } else if (key > z->data) { // If key is greater than z's data
            z = z->right; // z becomes z's right child
        } else { // If key is equal to z's data
            break; // Break the loop
        }
    }

    if (!z) { // Check if z is null
        std::cout << "Node with key " << key << " not found for deletion.\n"; // Node not found
        return;
    }

    RBT::Node *y = z; // y is the node to be deleted or replaced
    int yOriginalColor = y->color; // y's original color
    RBT::Node *x; // x is the node that replaces y

    if (!z->left) { // If z's left child is null
        x = z->right; // x becomes z's right child
        transplant(z, z->right); // Replace z with z's right child
    } else if (!z->right) { // If z's right child is null
        x = z->left; // x becomes z's left child
        transplant(z, z->left); // Replace z with z's left child
    } else { // If z has two children
        y = treeMinimum(z->right); // y becomes z's successor
        yOriginalColor = y->color; // y's original color
        x = y->right; // x becomes y's right child
        if (y->parent == z) { // If y is z's right child
            if (x) x->parent = y; // Check if x is not null
        } else {    // If y is not z's right child
            transplant(y, y->right);    // Replace y with y's right child
            y->right = z->right;    /// y's right child becomes z's right child
            if (y->right) y->right->parent = y; // Check if y->right is not null
        }
        transplant(z, y);   // Replace z with y
        y->left = z->left;  // y's left child becomes z's left child
        if (y->left) y->left->parent = y; // Check if y->left is not null
        y->color = z->color;  // y's color becomes z's color
    }

    if (yOriginalColor == 0 && x) { // Check if x is not null
        fixDelete(x);   // Fix the tree by calling fixDelete function with x as parameter
    }
    delete z;   // Delete the node
}


  int getHeight() {
    return calculateHeight(root) - 1; // Return the height of the tree by calling calculateHeight function with root as parameter and subtract 1 since root is not counted
  }

  RBT::Node* getMaximum() {
    RBT::Node* max = root;    // max is the maximum node
    while (max->right != nullptr) {     // While max's right child is not null
      max = max->right;   // max becomes max's right child
    }
    return max;   // Return max
  }

  RBT::Node* getMinimum() {   // Finding minimum node
    RBT::Node* min = root;    // min is the minimum node
    while (min->left != nullptr) {    // While min's left child is not null
      min = min->left;    // min becomes min's left child
    }
    return min;   // Return min
  }

  int getTotalNodes() {
    return getTotalNodes(root); // Return the total number of nodes by calling getTotalNodes function with root as parameter
  }


};
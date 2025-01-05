#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <vector>

class Limit;

class AVLTree {
private:
    Limit* root;
    bool isStopTree;  // Determines if this is a stop order tree or limit order tree

    // Tree manipulation helpers
    Limit* insert(Limit* root, Limit* limit, Limit* parent = nullptr);
    Limit* balance(Limit* limit);
    int getHeight(Limit* limit) const;
    int getBalanceFactor(Limit* limit) const;

    // Rotations
    Limit* rotateRight(Limit* parent);
    Limit* rotateLeft(Limit* parent);
    Limit* rotateLR(Limit* parent);
    Limit* rotateRL(Limit* parent);

    // Tree traversal
    std::vector<int> inOrderTraversal(Limit* root) const;
    std::vector<int> preOrderTraversal(Limit* root) const;
    std::vector<int> postOrderTraversal(Limit* root) const;

public:
    AVLTree(bool isStopTree = false);
    ~AVLTree();

    // Core operations
    void insertLimit(Limit* limit);
    void deleteLimit(Limit* limit);

    // Getters
    Limit* getRoot() const { return root; }
    bool isEmpty() const { return root == nullptr; }

    // Tree traversal public interface
    std::vector<int> getInOrder() const;
    std::vector<int> getPreOrder() const;
    std::vector<int> getPostOrder() const;

    // Performance counter
    int balanceCount = 0;
};

#endif
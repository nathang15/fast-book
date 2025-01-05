#include "AVLTree.hpp"
#include "Limit.hpp"

AVLTree::AVLTree(bool isStopTree) : root(nullptr), isStopTree(isStopTree) {}

AVLTree::~AVLTree() {
    root = nullptr;
}

void AVLTree::insertLimit(Limit* limit) {
    if (root == nullptr) {
        root = limit;
    }
    else {
        root = insert(root, limit);
    }
}

Limit* AVLTree::insert(Limit* root, Limit* limit, Limit* parent) {
    if (root == nullptr) {
        limit->setParent(parent);
        return limit;
    }

    if (limit->getLimitPrice() < root->getLimitPrice()) {
        root->setLeftChild(insert(root->getLeftChild(), limit, root));
    }
    else if (limit->getLimitPrice() > root->getLimitPrice()) {
        root->setRightChild(insert(root->getRightChild(), limit, root));
    }

    return balance(root);
}

int AVLTree::getHeight(Limit* limit) const {
    if (limit == nullptr) {
        return 0;
    }
    return std::max(getHeight(limit->getLeftChild()),
        getHeight(limit->getRightChild())) + 1;
}

int AVLTree::getBalanceFactor(Limit* limit) const {
    if (limit == nullptr) {
        return 0;
    }
    return getHeight(limit->getLeftChild()) - getHeight(limit->getRightChild());
}

Limit* AVLTree::rotateRight(Limit* parent) {
    Limit* newParent = parent->getLeftChild();
    parent->setLeftChild(newParent->getRightChild());

    if (newParent->getRightChild() != nullptr) {
        newParent->getRightChild()->setParent(parent);
    }

    newParent->setRightChild(parent);

    if (parent->getParent() != nullptr) {
        newParent->setParent(parent->getParent());
    }
    else {
        newParent->setParent(nullptr);
        root = newParent;
    }

    parent->setParent(newParent);
    return newParent;
}

Limit* AVLTree::rotateLeft(Limit* parent) {
    Limit* newParent = parent->getRightChild();
    parent->setRightChild(newParent->getLeftChild());

    if (newParent->getLeftChild() != nullptr) {
        newParent->getLeftChild()->setParent(parent);
    }

    newParent->setLeftChild(parent);

    if (parent->getParent() != nullptr) {
        newParent->setParent(parent->getParent());
    }
    else {
        newParent->setParent(nullptr);
        root = newParent;
    }

    parent->setParent(newParent);
    return newParent;
}

Limit* AVLTree::rotateLR(Limit* parent) {
    Limit* child = parent->getLeftChild();
    parent->setLeftChild(rotateLeft(child));
    return rotateRight(parent);
}

Limit* AVLTree::rotateRL(Limit* parent) {
    Limit* child = parent->getRightChild();
    parent->setRightChild(rotateRight(child));
    return rotateLeft(parent);
}

Limit* AVLTree::balance(Limit* limit) {
    int balanceFactor = getBalanceFactor(limit);

    if (balanceFactor > 1) {
        if (getBalanceFactor(limit->getLeftChild()) >= 0) {
            limit = rotateRight(limit);
        }
        else {
            limit = rotateLR(limit);
        }
        balanceCount++;
    }
    else if (balanceFactor < -1) {
        if (getBalanceFactor(limit->getRightChild()) > 0) {
            limit = rotateRL(limit);
        }
        else {
            limit = rotateLeft(limit);
        }
        balanceCount++;
    }

    return limit;
}

void AVLTree::deleteLimit(Limit* limit) {
    Limit* parent = limit->getParent();
    int limitPrice = limit->getLimitPrice();

    // Update parent pointers
    while (parent != nullptr) {
        parent = balance(parent);
        if (parent->getParent() != nullptr) {
            if (parent->getParent()->getLimitPrice() > limitPrice) {
                parent->getParent()->setLeftChild(parent);
            }
            else {
                parent->getParent()->setRightChild(parent);
            }
        }
        parent = parent->getParent();
    }

    // Update root if needed
    if (limit == root) {
        if (root->getRightChild() != nullptr) {
            root = root->getRightChild();
            while (root->getLeftChild() != nullptr) {
                root = root->getLeftChild();
            }
        }
        else {
            root = limit->getLeftChild();
        }
    }
}

std::vector<int> AVLTree::getInOrder() const {
    return inOrderTraversal(root);
}

std::vector<int> AVLTree::getPreOrder() const {
    return preOrderTraversal(root);
}

std::vector<int> AVLTree::getPostOrder() const {
    return postOrderTraversal(root);
}

std::vector<int> AVLTree::inOrderTraversal(Limit* root) const {
    std::vector<int> result;
    if (root == nullptr) return result;

    auto leftSubtree = inOrderTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

    result.push_back(root->getLimitPrice());

    auto rightSubtree = inOrderTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

    return result;
}

std::vector<int> AVLTree::preOrderTraversal(Limit* root) const {
    std::vector<int> result;
    if (root == nullptr) return result;

    result.push_back(root->getLimitPrice());

    auto leftSubtree = preOrderTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

    auto rightSubtree = preOrderTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

    return result;
}

std::vector<int> AVLTree::postOrderTraversal(Limit* root) const {
    std::vector<int> result;
    if (root == nullptr) return result;

    auto leftSubtree = postOrderTraversal(root->getLeftChild());
    result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

    auto rightSubtree = postOrderTraversal(root->getRightChild());
    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

    result.push_back(root->getLimitPrice());

    return result;
}
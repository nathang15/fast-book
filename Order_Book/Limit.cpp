#include "Limit.hpp"
#include "Order.hpp"
#include <iostream>

Limit::Limit(int _limitPrice, bool _buyOrSell, int _size, int _totalVolume) {
	limitPrice = _limitPrice;
	size = _size;
	totalVolume = _totalVolume;
	buyOrSell = _buyOrSell;
	parent = nullptr;
	leftChild = nullptr;
	rightChild = nullptr;
	headOrder = nullptr;
	tailOrder = nullptr;
}

Limit::~Limit() {
	if (parent != nullptr) {
		bool leftOrRightChild = (limitPrice < parent->getLimitPrice());

		// Node with only 1 child or no child
		if (leftChild == nullptr) {
			if (leftOrRightChild) {
				parent->leftChild = rightChild;
			}
			else {
				parent->rightChild = rightChild;
			}
			if (rightChild != nullptr) {
				rightChild->setParent(parent);
			}
			return;
		}
		else if (rightChild == nullptr) {
			if (leftOrRightChild) {
				parent->leftChild = leftChild;
			}
			else {
				parent->rightChild = leftChild;
			}
			leftChild->setParent(parent);
			return;
		}

		// Node with 2 children
		Limit* temp = rightChild;

		while (temp->getLeftChild() != nullptr) {
			temp = temp->getLeftChild();
		}

		if (rightChild->getLeftChild() != nullptr) {
			temp->getParent()->setLeftChild(temp->getRightChild());
			if (temp->getRightChild() != nullptr) {
				temp->getRightChild()->setParent(temp->getParent());
			}
			temp->setRightChild(rightChild);
			rightChild->setParent(temp);
		}
		temp->setParent(parent);
		temp->setLeftChild(leftChild);
		leftChild->setParent(temp);
		if (leftOrRightChild) {
			parent->leftChild = temp;
		}
		else {
			parent->rightChild = temp;
		}
	}
	else {
		// Node with 1 child or no child
		if (leftChild == nullptr && rightChild == nullptr) {
			return;
		}
		else if (leftChild == nullptr) {
			rightChild->setParent(nullptr);
			return;
		}
		else if (rightChild == nullptr) {
			leftChild->setParent(nullptr);
			return;
		}

		// Node with 2 children
		Limit* temp = rightChild;
		while (temp->getLeftChild() != nullptr) {
			temp = temp->getLeftChild();
		}
		if (rightChild->getLeftChild() != nullptr) {
			temp->getParent()->setLeftChild(temp->getRightChild());
			if (temp->getRightChild() != nullptr) {
				temp->getRightChild()->setParent(temp->getParent());
			}
			temp->setRightChild(rightChild);
			rightChild->setParent(temp);
		}
		temp->setParent(nullptr);
		temp->setLeftChild(leftChild);
		leftChild->setParent(temp);
	}
}

Order *Limit::getHeadOrder() const {
	return headOrder;
}

int Limit::getLimitPrice() const {
	return limitPrice;
}

int Limit::getSize() const {
	return size;
}

int Limit::getTotalVolume() const {
	return totalVolume;
}

bool Limit::getBuyOrSell() const {
	return buyOrSell;
}

Limit *Limit::getParent() const {
	return parent;
}

Limit *Limit::getLeftChild() const {
	return leftChild;
}

Limit *Limit::getRightChild() const {
	return rightChild;
}

void Limit::setParent(Limit* newParent) {
	parent = newParent;
}

void Limit::setLeftChild(Limit* newLeftChild) {
	leftChild = newLeftChild;
}

void Limit::setRightChild(Limit* newRightChild) {
	rightChild = newRightChild;
}

void Limit::partiallyFillTotalVolume(int orderedShares) {
	totalVolume -= orderedShares;
}

void Limit::addOrder(Order* order) {
	if (headOrder == nullptr) {
		headOrder = order;
		tailOrder = order;
	}
	else {
		tailOrder->nextOrder = order;
		order->prevOrder = tailOrder;
		order->nextOrder = nullptr;
		tailOrder = order;
	}
	size++;
	totalVolume += order->getShares();
	order->parentLimit = this;
}

void Limit::printForward() const
{
	Order* current = headOrder;
	while (current != nullptr) {
		std::cout << current->getOrderId() << " ";
		current = current->nextOrder;
	}
	std::cout << std::endl;
}

void Limit::printBackward() const
{
	Order* current = tailOrder;
	while (current != nullptr) {
		std::cout << current->getOrderId() << " ";
		current = current->prevOrder;
	}
	std::cout << std::endl;
}

void Limit::print() const
{
	std::cout << "Limit Price: " << limitPrice << std::endl;
	std::cout << "Limit Size: " << size << std::endl;
	std::cout << "Limit Volume: " << totalVolume << std::endl;
}
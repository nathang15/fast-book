#ifndef LIMIT_HPP
#define LIMIT_HPP

class Order;

class Limit {
private:
	int limitPrice;    // Price level
	int size;         // Number of orders at this price
	int totalVolume;  // Total shares at this price
	bool buyOrSell;
	Limit* parent;
	Limit* leftChild;
	Limit* rightChild;
	Order* headOrder; // Linked list of orders at this price
	Order* tailOrder;

	friend class Order;

public:
	Limit(int limitPrice, bool buyOrSell, int size = 0, int totalVolume = 0);
	~Limit();

	Order *getHeadOrder() const;
	int getLimitPrice() const;
	int getSize() const;
	int getTotalVolume() const;
	bool getBuyOrSell() const;
	Limit *getParent() const;
	Limit *getLeftChild() const;
	Limit *getRightChild() const;
	void setParent(Limit* newParent);
	void setLeftChild(Limit* newLeftChild);
	void setRightChild(Limit* newRightChild);
	void partiallyFillTotalVolume(int orderedShares);

	void addOrder(Order* _order);
	void printForward() const;
	void printBackward() const;
	void print() const;
};

#endif
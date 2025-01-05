#ifndef ORDER_HPP
#define ORDER_HPP

class Limit;

class Order {
private:
	int orderId;
	bool buyOrSell;
	int shares;
	int limit;
	int entryTime;
	int eventTime;
	Order* nextOrder;
	Order* prevOrder;

	Limit* parentLimit;

	friend class Limit;

public:
	Order(int _orderId, bool _buyOrSell, int _shares, int _limit, int _entryTime = 0, int _eventTime = 0);

	int getOrderId() const;
	int getShares() const;
	bool getBuyOrSell() const;
	int getLimit() const;
	int getEntryTime() const;
	int getEventTime() const;
	Limit* getParentLimit() const;

	void partiallyFillOrder(int orderedShares);
	void cancel();
	void execute();
	void modifyOrder(int newShares, int newLimit);
	void setShares(int newShares);

	void print() const;
};

#endif
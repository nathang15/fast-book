#ifndef BOOK_HPP
#define BOOK_HPP

#include "AVLTree.hpp"
#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

class Limit;
class Order;

class Book {
private:
	// AVL trees for different order types
	AVLTree buyTree;
	AVLTree sellTree;
	AVLTree stopBuyTree;
	AVLTree stopSellTree;

	// Book edges
	Limit* lowestSell;
	Limit* highestBuy;
	Limit* highestStopSell;
	Limit* lowestStopBuy;

	std::unordered_map<int, Order*> orderMap;
	std::unordered_map<int, Limit*> limitBuyMap;
	std::unordered_map<int, Limit*> limitSellMap;
	std::unordered_map<int, Limit*> stopMap;

	// Helper functions
	void addLimit(int limitPrice, bool buyOrSell);
	void addStop(int stopPrice, bool buyOrSell);
	void updateBookEdgeInsert(Limit* newLimit);
	void updateStopBookEdgeInsert(Limit* newStop);
	void updateBookEdgeDelete(Limit* limit);
	void updateStopBookEdgeDelete(Limit* stop);
	void deleteLimit(Limit* limit);
	void deleteStop(Limit* stop);
	void deleteFromOrderMap(int orderId);
	void deleteFromLimitMaps(int limitPrice, bool buyOrSell);
	void deleteFromStopMap(int stopPrice);

	// Order execution helpers
	int limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
	int stopOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
	int currentOrderAsMarketOrder(Order* headOrder, bool buyOrSell);
	int stopLimitOrderAsLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
	void executeStopOrders(bool buyOrSell);
	void stopLimitOrderToLimitOrder(Order* headOrder, bool buyOrSell);
	void marketOrderHelper(int orderId, bool buyOrSell, int shares);

public:
	Book();
	~Book();

	// Counts used in order book benchmarking
	int executedOrdersCount = 0;
	int AVLTreeBalanceCount = 0;

	// Getters
	AVLTree& getBuyTree() { return buyTree; }
	AVLTree& getSellTree() { return sellTree; }
	Limit* getLowestSell() const { return lowestSell; }
	Limit* getHighestBuy() const { return highestBuy; }
	AVLTree& getStopBuyTree() { return stopBuyTree; }
	AVLTree& getStopSellTree() { return stopSellTree; }
	Limit* getHighestStopSell() const { return highestStopSell; }
	Limit* getLowestStopBuy() const { return lowestStopBuy; }

	// Order management functions
	void marketOrder(int orderId, bool buyOrSell, int shares);
	void addLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
	void cancelLimitOrder(int orderId);
	void modifyLimitOrder(int orderId, int newShares, int newLimit);
	void addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
	void cancelStopOrder(int orderId);
	void modifyStopOrder(int orderId, int newShares, int newStopPrice);
	void addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
	void cancelStopLimitOrder(int orderId);
	void modifyStopLimitOrder(int orderId, int newShares, int newLimitPrice, int newStopPrice);

	// Search functions
	int getLimitHeight(Limit* limit) const;
	Order* searchOrderMap(int orderId) const;
	Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
	Limit* searchStopMap(int stopPrice) const;

	// visualising the order book
	void printLimit(int limitPrice, bool buyOrSell) const;
	void printOrder(int orderId) const;
	void printBookEdges() const;
	void printOrderBook() const;

	// generating sample data
	Order* getRandomOrder(int key, std::mt19937 gen) const;
	std::unordered_set<Order*> limitOrders;
	std::unordered_set<Order*> stopOrders;
	std::unordered_set<Order*> stopLimitOrders;
};

#endif
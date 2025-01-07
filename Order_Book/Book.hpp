#ifndef BOOK_HPP
#define BOOK_HPP

#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

class Limit;
class Order;

class Book {
private:
	Limit* buyTree;      // AVL tree for buy orders
	Limit* sellTree;     // AVL tree for sell orders	
	Limit* lowestSell;
	Limit* highestBuy;

	Limit* stopBuyTree;  // AVL tree for stop buy orders
	Limit* stopSellTree; // AVL tree for stop sell orders
	Limit* highestStopSell;
	Limit* lowestStopBuy;

	std::unordered_map<int, Order*> orderMap;
	std::unordered_map<int, Limit*> limitBuyMap;
	std::unordered_map<int, Limit*> limitSellMap;
	std::unordered_map<int, Limit*> stopMap;

	void addLimit(int limitPrice, bool buyOrSell);
	void addStop(int stopPrice, bool buyOrSell);
	Limit* insert(Limit* root, Limit* limit, Limit* parent = nullptr);
	Limit* insertStop(Limit* root, Limit* limit, Limit* parent = nullptr);
	void updateBookEdgeInsert(Limit* newLimit);
	void updateStopBookEdgeInsert(Limit* newStop);
	void updateBookEdgeDelete(Limit* limit);
	void updateStopBookEdgeDelete(Limit* stop);
	void changeBookRoots(Limit* limit);
	void changeStopBookRoots(Limit* stop);
	void deleteLimit(Limit* limit);
	void deleteStop(Limit* stop);
	void deleteFromOrderMap(int orderId);
	void deleteFromLimitMaps(int limitPrice, bool buyOrSell);
	void deleteFromStopMap(int stopPrice);
	int limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice);
	int stopOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int stopPrice);
	int currentOrderAsMarketOrder(Order* headOrder, bool buyOrSell);
	int stopLimitOrderAsLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice);
	void executeStopOrders(bool buyOrSell);
	void stopLimitOrderToLimitOrder(Order* headOrder, bool buyOrSell);
	void marketOrderHelper(int orderId, bool buyOrSell, int shares);

	// Balance AVL tree
	int limitHeightDifference(Limit* limit);
	Limit* rr_rotate(Limit* limit);
	Limit* ll_rotate(Limit* limit);
	Limit* lr_rotate(Limit* limit);
	Limit* rl_rotate(Limit* limit);
	Limit* balance(Limit* limit);
	Limit* rr_rotateStop(Limit* limit);
	Limit* ll_rotateStop(Limit* limit);
	Limit* lr_rotateStop(Limit* limit);
	Limit* rl_rotateStop(Limit* limit);
	Limit* balanceStop(Limit* limit);

public:
	Book();
	~Book();

	// Counts used in order book benchmarking
	int executedOrdersCount = 0;
	int AVLTreeBalanceCount = 0;

	// Getter and setter
	Limit* getBuyTree() const;
	Limit* getSellTree() const;
	Limit* getLowestSell() const;
	Limit* getHighestBuy() const;
	Limit* getStopBuyTree() const;
	Limit* getStopSellTree() const;
	Limit* getHighestStopSell() const;
	Limit* getLowestStopBuy() const;

	// Functions for different types of orders
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

	int getLimitHeight(Limit* limit) const;
	Order* searchOrderMap(int orderId) const;
	Limit* searchLimitMaps(int limitPrice, bool buyOrSell) const;
	Limit* searchStopMap(int stopPrice) const;

	// visualising the order book
	void printLimit(int limitPrice, bool buyOrSell) const;
	void printOrder(int orderId) const;
	void printBookEdges() const;
	void printOrderBook() const;
	std::vector<int> inOrderTreeTraversal(Limit* root) const;
	std::vector<int> preOrderTreeTraversal(Limit* root) const;
	std::vector<int> postOrderTreeTraversal(Limit* root) const;

	// generating sample data
	Order* getRandomOrder(int key, std::mt19937 gen) const;
	std::unordered_set<Order*> limitOrders;
	std::unordered_set<Order*> stopOrders;
	std::unordered_set<Order*> stopLimitOrders;
};

#endif
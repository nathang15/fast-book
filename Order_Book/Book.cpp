#include "Book.hpp"
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <iterator>

Book::Book() :
    buyTree(nullptr),
    sellTree(nullptr),
    stopBuyTree(nullptr),
    stopSellTree(nullptr),
    lowestSell(nullptr),
    highestBuy(nullptr),
    highestStopSell(nullptr),
    lowestStopBuy(nullptr) {}

Book::~Book() {
	for (auto& [id, order] : orderMap) {
		delete order;
	}
	orderMap.clear();

	for (auto& [limitPrice, limit] : limitBuyMap) {
		delete limit;
	}

	limitBuyMap.clear();

	for (auto& [limitPrice, limit] : limitSellMap) {
		delete limit;
	}
	limitSellMap.clear();

	for (auto& [stopPrice, stop] : stopMap) {
		delete stop;
	}
	stopMap.clear();
}

//exec market order
void Book::marketOrder(int orderId, bool buyOrSell, int shares) {
	executedOrdersCount = 0;
	AVLTreeBalanceCount = 0;
	marketOrderHelper(orderId, buyOrSell, shares);

	executeStopOrders(buyOrSell);
}

void Book::addLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice) {
	AVLTreeBalanceCount = 0;
	// Order being executed immediately
	shares = limitOrderAsMarketOrder(orderId, buyOrSell, shares, limitPrice);

	if (shares != 0) {
		Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
		orderMap.emplace(orderId, newOrder);

		auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

		if (limitMap.find(limitPrice) == limitMap.end()) {
			addLimit(limitPrice, newOrder->getBuyOrSell());
		}

		limitMap.at(limitPrice)->append(newOrder);
	}
	else {
		executeStopOrders(buyOrSell);
	}
}

// Delete a limit order from the book
void Book::cancelLimitOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteLimit(order->getParentLimit());
        }
        deleteFromOrderMap(orderId);
        delete order;
    }
}

// Modify an existing limit order
void Book::modifyLimitOrder(int orderId, int newShares, int newLimit)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteLimit(order->getParentLimit());
        }

        order->modifyOrder(newShares, newLimit);
        auto& limitMap = order->getBuyOrSell() ? limitBuyMap : limitSellMap;

        if (limitMap.find(newLimit) == limitMap.end())
        {
            addLimit(newLimit, order->getBuyOrSell());
        }
        limitMap.at(newLimit)->append(order);
    }
}

// Add a stop order
void Book::addStopOrder(int orderId, bool buyOrSell, int shares, int stopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    // Account for stop order being executed immediately
    shares = stopOrderAsMarketOrder(orderId, buyOrSell, shares, stopPrice);

    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, 0);
        orderMap.emplace(orderId, newOrder);

        if (stopMap.find(stopPrice) == stopMap.end())
        {
            addStop(stopPrice, newOrder->getBuyOrSell());
        }
        stopMap.at(stopPrice)->append(newOrder);
    }
}

void Book::cancelStopOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteStop(order->getParentLimit());
        }
        deleteFromOrderMap(orderId);
        delete order;
    }
}

void Book::modifyStopOrder(int orderId, int newShares, int newStopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteStop(order->getParentLimit());
        }

        order->modifyOrder(newShares, 0);

        if (stopMap.find(newStopPrice) == stopMap.end())
        {
            addStop(newStopPrice, order->getBuyOrSell());
        }
        stopMap.at(newStopPrice)->append(order);
    }
}

void Book::addStopLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    // stop limit order being executed immediately
    shares = stopLimitOrderAsLimitOrder(orderId, buyOrSell, shares, limitPrice, stopPrice);

    if (shares != 0)
    {
        Order* newOrder = new Order(orderId, buyOrSell, shares, limitPrice);
        orderMap.emplace(orderId, newOrder);

        if (stopMap.find(stopPrice) == stopMap.end())
        {
            addStop(stopPrice, newOrder->getBuyOrSell());
        }
        stopMap.at(stopPrice)->append(newOrder);
    }
}

void Book::cancelStopLimitOrder(int orderId)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);

    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteStop(order->getParentLimit());
        }
        deleteFromOrderMap(orderId);
        delete order;
    }
}

void Book::modifyStopLimitOrder(int orderId, int newShares, int newLimitPrice, int newStopPrice)
{
    executedOrdersCount = 0;
    AVLTreeBalanceCount = 0;
    Order* order = searchOrderMap(orderId);
    if (order != nullptr)
    {
        order->cancel();
        if (order->getParentLimit()->getSize() == 0)
        {
            deleteStop(order->getParentLimit());
        }

        order->modifyOrder(newShares, newLimitPrice);

        if (stopMap.find(newStopPrice) == stopMap.end())
        {
            addStop(newStopPrice, order->getBuyOrSell());
        }
        stopMap.at(newStopPrice)->append(order);
    }
}

int Book::getLimitHeight(Limit* limit) const {
    if (limit == nullptr) {
        return 0;
    }
    else {
        int l_height = getLimitHeight(limit->getLeftChild());
        int r_height = getLimitHeight(limit->getRightChild());
        int max = std::max(l_height, r_height) + 1;
        return max;
    }
}

// Find an order
Order* Book::searchOrderMap(int orderId) const
{
    auto it = orderMap.find(orderId);
    if (it != orderMap.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "No order number " << orderId << std::endl;
        return nullptr;
    }
}

// Find a limit
Limit* Book::searchLimitMaps(int limitPrice, bool buyOrSell) const
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

    auto it = limitMap.find(limitPrice);
    if (it != limitMap.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "No " << (buyOrSell ? "buy " : "sell ") << "limit at " << limitPrice << std::endl;
        return nullptr;
    }
}

// Find a stop level
Limit* Book::searchStopMap(int stopPrice) const
{
    auto it = stopMap.find(stopPrice);
    if (it != stopMap.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "No stop level at " << stopPrice << std::endl;
        return nullptr;
    }
}

void Book::printLimit(int limitPrice, bool buyOrSell) const
{
    searchLimitMaps(limitPrice, buyOrSell)->print();
}

void Book::printOrder(int orderId) const
{
    searchOrderMap(orderId)->print();
}

void Book::printBookEdges() const
{
    std::cout << "Buy edge: " << highestBuy->getLimitPrice()
        << "Sell edge: " << lowestSell->getLimitPrice() << std::endl;
}

void Book::printOrderBook() const {
    // Print stop buy orders
    std::vector<int> prices = stopBuyTree.getInOrder();
    std::cout << "Stop Buy Orders: [";
    for (size_t i = 0; i < prices.size(); ++i) {
        std::cout << prices[i] << "-" << searchStopMap(prices[i])->getTotalVolume();
        if (i != prices.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";

    // Print stop sell orders
    prices = stopSellTree.getInOrder();
    std::cout << "Stop Sell Orders: [";
    for (size_t i = 0; i < prices.size(); ++i) {
        std::cout << prices[i] << "-" << searchStopMap(prices[i])->getTotalVolume();
        if (i != prices.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";

    // Print buy orders
    prices = buyTree.getInOrder();
    std::cout << "Buy Orders: [";
    for (size_t i = 0; i < prices.size(); ++i) {
        std::cout << prices[i] << "-" << searchLimitMaps(prices[i], true)->getTotalVolume();
        if (i != prices.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";

    // Print sell orders
    prices = sellTree.getInOrder();
    std::cout << "Sell Orders: [";
    for (size_t i = 0; i < prices.size(); ++i) {
        std::cout << prices[i] << "-" << searchLimitMaps(prices[i], false)->getTotalVolume();
        if (i != prices.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}

// Return a random active order for testing purposes
// 0:Limit, 1:Stop, 2:StopLimit
Order* Book::getRandomOrder(int key, std::mt19937 gen) const
{
    if (key == 0)
    {
        if (limitOrders.size() > 10000)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, limitOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = limitOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    }
    else if (key == 1)
    {
        if (stopOrders.size() > 500)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, stopOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = stopOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    }
    else if (key == 2)
    {
        if (stopLimitOrders.size() > 500)
        {
            // Generate a random index within the range of the hash set size
            std::uniform_int_distribution<> mapDist(0, stopLimitOrders.size() - 1);
            int randomIndex = mapDist(gen);

            // Access the element at the random index directly
            auto it = stopLimitOrders.begin();
            std::advance(it, randomIndex);
            return *it;
        }
        return nullptr;
    }
    return nullptr;
}

void Book::addLimit(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;
    auto& tree = buyOrSell ? buyTree : sellTree;
    auto& bookEdge = buyOrSell ? highestBuy : lowestSell;

    Limit* newLimit = new Limit(limitPrice, buyOrSell);
    limitMap.emplace(limitPrice, newLimit);

    if (tree == nullptr)
    {
        tree = newLimit;
        bookEdge = newLimit;
    }
    else
    {
        Limit* root = insert(tree, newLimit);
        updateBookEdgeInsert(newLimit);
    }
}

void Book::addStop(int stopPrice, bool buyOrSell) {
    auto& tree = buyOrSell ? stopBuyTree : stopSellTree;
    auto& bookEdge = buyOrSell ? lowestStopBuy : highestStopSell;

    Limit* newStop = new Limit(stopPrice, buyOrSell);
    stopMap.emplace(stopPrice, newStop);

    tree.insertLimit(newStop);

    if (tree.isEmpty()) {
        bookEdge = newStop;
    }
    else {
        updateStopBookEdgeInsert(newStop);
    }
}

// Update the edge of the book if new limit is on edge of the book
void Book::updateBookEdgeInsert(Limit* newLimit)
{
    if (newLimit->getBuyOrSell())
    {
        if (newLimit->getLimitPrice() > highestBuy->getLimitPrice())
        {
            highestBuy = newLimit;
        }
    }
    else
    {
        if (newLimit->getLimitPrice() < lowestSell->getLimitPrice())
        {
            lowestSell = newLimit;
        }
    }
}

// Update the edge of the stop book if new stop is on edge of the book
void Book::updateStopBookEdgeInsert(Limit* newStop)
{
    if (newStop->getBuyOrSell())
    {
        if (newStop->getLimitPrice() < lowestStopBuy->getLimitPrice())
        {
            lowestStopBuy = newStop;
        }
    }
    else
    {
        if (newStop->getLimitPrice() > highestStopSell->getLimitPrice())
        {
            highestStopSell = newStop;
        }
    }
}

void Book::updateBookEdgeDelete(Limit* limit) {
    auto& bookEdge = limit->getBuyOrSell() ? highestBuy : lowestSell;
    auto& tree = limit->getBuyOrSell() ? buyTree : sellTree;

    if (limit == bookEdge) {
        if (limit->getBuyOrSell()) {
            // Find next highest buy
            if (limit->getLeftChild() != nullptr) {
                bookEdge = limit->getLeftChild();
                while (bookEdge->getRightChild() != nullptr) {
                    bookEdge = bookEdge->getRightChild();
                }
            }
            else {
                bookEdge = limit->getParent();
            }
        }
        else {
            // Find next lowest sell
            if (limit->getRightChild() != nullptr) {
                bookEdge = limit->getRightChild();
                while (bookEdge->getLeftChild() != nullptr) {
                    bookEdge = bookEdge->getLeftChild();
                }
            }
            else {
                bookEdge = limit->getParent();
            }
        }
    }
}

void Book::updateStopBookEdgeDelete(Limit* stop) {
    auto& bookEdge = stop->getBuyOrSell() ? lowestStopBuy : highestStopSell;
    auto& tree = stop->getBuyOrSell() ? stopBuyTree : stopSellTree;

    if (stop == bookEdge) {
        if (stop->getBuyOrSell()) {
            // Find next lowest stop buy
            if (stop->getRightChild() != nullptr) {
                bookEdge = stop->getRightChild();
                while (bookEdge->getLeftChild() != nullptr) {
                    bookEdge = bookEdge->getLeftChild();
                }
            }
            else {
                bookEdge = stop->getParent();
            }
        }
        else {
            // Find next highest stop sell
            if (stop->getLeftChild() != nullptr) {
                bookEdge = stop->getLeftChild();
                while (bookEdge->getRightChild() != nullptr) {
                    bookEdge = bookEdge->getRightChild();
                }
            }
            else {
                bookEdge = stop->getParent();
            }
        }
    }
}

// Change the root limit in the AVL tree if the root limit is deleted
void Book::changeBookRoots(Limit* limit) {
    auto& tree = limit->getBuyOrSell() ? buyTree : sellTree;
    if (limit == tree)
    {
        if (limit->getRightChild() != nullptr)
        {
            tree = tree->getRightChild();
            while (tree->getLeftChild() != nullptr)
            {
                tree = tree->getLeftChild();
            }
        }
        else
        {
            tree = limit->getLeftChild();
        }
    }
}

// Change the root stop level in the AVL tree if the root stop level is deleted
void Book::changeStopBookRoots(Limit* stopLevel) {
    auto& tree = stopLevel->getBuyOrSell() ? stopBuyTree : stopSellTree;
    if (stopLevel == tree)
    {
        if (stopLevel->getRightChild() != nullptr)
        {
            tree = tree->getRightChild();
            while (tree->getLeftChild() != nullptr)
            {
                tree = tree->getLeftChild();
            }
        }
        else
        {
            tree = stopLevel->getLeftChild();
        }
    }
}

void Book::deleteLimit(Limit* limit) {
    updateBookEdgeDelete(limit);
    deleteFromLimitMaps(limit->getLimitPrice(), limit->getBuyOrSell());

    auto& tree = limit->getBuyOrSell() ? buyTree : sellTree;
    tree.deleteLimit(limit);
}

void Book::deleteStop(Limit* stop) {
    updateStopBookEdgeDelete(stop);
    deleteFromStopMap(stop->getLimitPrice());

    auto& tree = stop->getBuyOrSell() ? stopBuyTree : stopSellTree;
    tree.deleteLimit(stop);
}

void Book::deleteFromOrderMap(int orderId)
{
    orderMap.erase(orderId);
}

void Book::deleteFromLimitMaps(int limitPrice, bool buyOrSell)
{
    auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;
    limitMap.erase(limitPrice);
}

void Book::deleteFromStopMap(int stopPrice)
{
    stopMap.erase(stopPrice);
}

// When a limit order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a market order
int Book::limitOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int limitPrice)
{
    if (buyOrSell)
    {
        while (lowestSell != nullptr && shares != 0 && lowestSell->getLimitPrice() <= limitPrice)
        {
            if (shares <= lowestSell->getTotalVolume())
            {
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            }
            else {
                shares -= lowestSell->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, lowestSell->getTotalVolume());
            }
        }
        return shares;
    }
    else {
        while (highestBuy != nullptr && shares != 0 && highestBuy->getLimitPrice() >= limitPrice)
        {
            if (shares <= highestBuy->getTotalVolume())
            {
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            }
            else {
                shares -= highestBuy->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, highestBuy->getTotalVolume());
            }
        }
        return shares;
    }
}

// When a stop order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a market order
int Book::stopOrderAsMarketOrder(int orderId, bool buyOrSell, int shares, int stopPrice)
{
    if (buyOrSell && lowestSell != nullptr && stopPrice <= lowestSell->getLimitPrice())
    {
        marketOrder(orderId, true, shares);
        return 0;
    }
    else if (!buyOrSell && highestBuy != nullptr && stopPrice >= highestBuy->getLimitPrice())
    {
        marketOrder(orderId, false, shares);
        return 0;
    }
    return shares;
}

// When a limit order that used to be a stop limit order overlaps with the highest buy or lowest sell, 
// immediately execute it as if it were a market order
int Book::currentOrderAsMarketOrder(Order* headOrder, bool buyOrSell)
{
    int shares = headOrder->getShares();
    int orderId = headOrder->getOrderId();
    int limitPrice = headOrder->getLimit();

    if (buyOrSell)
    {
        while (lowestSell != nullptr && lowestSell->getLimitPrice() <= limitPrice)
        {
            if (shares <= lowestSell->getTotalVolume())
            {
                deleteFromOrderMap(orderId);
                delete headOrder;
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            }
            else {
                shares -= lowestSell->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, lowestSell->getTotalVolume());
            }
        }
        return shares;
    }
    else {
        while (highestBuy != nullptr && highestBuy->getLimitPrice() >= limitPrice)
        {
            if (shares <= highestBuy->getTotalVolume())
            {
                deleteFromOrderMap(orderId);
                delete headOrder;
                marketOrderHelper(orderId, buyOrSell, shares);
                return 0;
            }
            else {
                shares -= highestBuy->getTotalVolume();
                marketOrderHelper(orderId, buyOrSell, highestBuy->getTotalVolume());
            }
        }
        return shares;
    }
}

// When a stop limit order overlaps with the highest buy or lowest sell, immediately
// execute it as if it were a limit order
int Book::stopLimitOrderAsLimitOrder(int orderId, bool buyOrSell, int shares, int limitPrice, int stopPrice)
{
    if (buyOrSell && lowestSell != nullptr && stopPrice <= lowestSell->getLimitPrice())
    {
        addLimitOrder(orderId, true, shares, limitPrice);
        return 0;
    }
    else if (!buyOrSell && highestBuy != nullptr && stopPrice >= highestBuy->getLimitPrice())
    {
        addLimitOrder(orderId, false, shares, limitPrice);
        return 0;
    }
    return shares;
}

// Executes any stop orders which need to be executed
void Book::executeStopOrders(bool buyOrSell)
{
    if (buyOrSell)
    {
        // Execute any buy stop market orders
        // If the book is empty and can't complete stop market order then it just doesn't execute and is forgotten.
        while (lowestStopBuy != nullptr && (lowestSell == nullptr || lowestStopBuy->getLimitPrice() <= lowestSell->getLimitPrice()))
        {
            Order* headOrder = lowestStopBuy->getHeadOrder();
            if (headOrder->getLimit() == 0)
            {
                int shares = headOrder->getShares();
                headOrder->execute();
                if (lowestStopBuy->getSize() == 0)
                {
                    deleteStop(lowestStopBuy);
                }
                deleteFromOrderMap(headOrder->getOrderId());
=                delete headOrder;
                marketOrderHelper(0, true, shares);
            }
            else {
=                stopLimitOrderToLimitOrder(headOrder, buyOrSell);
            }
        }
    }
    else {
        // Execute any sell stop market orders
        // If the book is empty and can't complete stop market order then it doesn't execute and is forgotten.
        while (highestStopSell != nullptr && (highestBuy == nullptr || highestStopSell->getLimitPrice() >= highestBuy->getLimitPrice()))
        {
            Order* headOrder = highestStopSell->getHeadOrder();
            if (headOrder->getLimit() == 0)
            {
                int shares = headOrder->getShares();
                headOrder->execute();
                if (highestStopSell->getSize() == 0)
                {
                    deleteStop(highestStopSell);
                }
                deleteFromOrderMap(headOrder->getOrderId());
                delete headOrder;
                marketOrderHelper(0, false, shares);
            }
            else {
                stopLimitOrderToLimitOrder(headOrder, buyOrSell);
            }
        }
    }
}

// Turn stop limit order into limit order
void Book::stopLimitOrderToLimitOrder(Order* headOrder, bool buyOrSell)
{
    auto& bookEdge = buyOrSell ? lowestStopBuy : highestStopSell;
    headOrder->execute();
    if (bookEdge->getSize() == 0)
    {
        deleteStop(bookEdge);
    }

    // Account for order being executed immediately - majority of cases
    int shares = currentOrderAsMarketOrder(headOrder, buyOrSell);

    if (shares != 0)
    {
        headOrder->setShares(shares);
        auto& limitMap = buyOrSell ? limitBuyMap : limitSellMap;

        if (limitMap.find(headOrder->getLimit()) == limitMap.end())
        {
            addLimit(headOrder->getLimit(), buyOrSell);
        }
        limitMap.at(headOrder->getLimit())->append(headOrder);
    }
}

// Function which actually executes the market order.
// If the book is empty and can't complete market order then market order doesn't execute and is forgotten
void Book::marketOrderHelper(int orderId, bool buyOrSell, int shares)
{
    auto& bookEdge = buyOrSell ? lowestSell : highestBuy;

    while (bookEdge != nullptr && bookEdge->getHeadOrder()->getShares() <= shares)
    {
        Order* headOrder = bookEdge->getHeadOrder();
        shares -= headOrder->getShares();
        headOrder->execute();
        if (bookEdge->getSize() == 0)
        {
            deleteLimit(bookEdge);
        }
        deleteFromOrderMap(headOrder->getOrderId());
        delete headOrder;
        executedOrdersCount += 1;
    }
    if (bookEdge != nullptr && shares != 0)
    {
        bookEdge->getHeadOrder()->partiallyFillOrder(shares);
        executedOrdersCount += 1;
    }
}
#include "Order.hpp"
#include "Limit.hpp"
#include <iostream>

Order::Order(int _orderId, bool _buyOrSell, int _shares, int _limit, int _entryTime, int _eventTime) {
	orderId = _orderId;
	buyOrSell = _buyOrSell;
	shares = _shares;
	limit = _limit;
	entryTime = _entryTime;
	eventTime = _eventTime;
	nextOrder = nullptr;
	prevOrder = nullptr;
	parentLimit = nullptr;
}

int Order::getOrderId() const {
	return orderId;
}

int Order::getShares() const {
	return shares;
}

bool Order::getBuyOrSell() const {
	return buyOrSell;
}

int Order::getLimit() const {
	return limit;
}

int Order::getEntryTime() const {
	return entryTime;
}

int Order::getEventTime() const {
	return eventTime;
}

Limit* Order::getParentLimit() const {
	return parentLimit;
}

void Order::partiallyFillOrder(int orderedShares) {
	shares -= orderedShares;
	parentLimit->partiallyFillTotalVolume(orderedShares);
}

void Order::cancel() {
	if (prevOrder == nullptr) {
		parentLimit->headOrder = nextOrder;
	}
	else {
		prevOrder->nextOrder = nextOrder;
	}
	if (nextOrder == nullptr) {
		parentLimit->tailOrder = prevOrder;
	}
	else {
		nextOrder->prevOrder = prevOrder;
	}

	parentLimit->totalVolume -= shares;
	parentLimit->size--;
}

void Order::execute() {
	parentLimit->headOrder = nextOrder;
	if (nextOrder == nullptr) {
		parentLimit->tailOrder = nullptr;
	}
	else {
		nextOrder->prevOrder = nullptr;
	}
	nextOrder = nullptr;
	prevOrder = nullptr;

	parentLimit->totalVolume -= shares;
	parentLimit->size--;
}

void Order::modifyOrder(int newShares, int newLimit) {
	shares = newShares;
	limit = newLimit;
	nextOrder = nullptr;
	prevOrder = nullptr;
	parentLimit = nullptr;
}

void Order::setShares(int newShares) {
	shares = newShares;
}

void Order::print() const {
	std::cout << "Order ID: " << orderId << std::endl;
	std::cout << "Order Type: " << (buyOrSell == 1 ? "buy" : "sell") << std::endl;
	std::cout << "Shares: " << shares << std::endl;
	std::cout << "Limit: " << limit << std::endl;
	std::cout << "Entry Time: " << entryTime << std::endl;
	std::cout << "Event Time: " << eventTime << std::endl;
	std::cout << std::endl;
}
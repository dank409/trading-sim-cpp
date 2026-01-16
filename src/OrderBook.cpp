#include "OrderBook.h"
#include <algorithm>

namespace trading {

OrderBook::OrderBook(const AssetId& assetId)
    : assetId_(assetId)
{
}

void OrderBook::addOrder(Order& order) {
    if (order.getSide() == Side::Buy) {
        bids_[order.getLimitPrice()].push_back(&order);
    } else {
        asks_[order.getLimitPrice()].push_back(&order);
    }
}

bool OrderBook::cancelOrder(OrderId orderId) {
    // Search bids
    for (auto& [price, orderQueue] : bids_) {
        for (auto it = orderQueue.begin(); it != orderQueue.end(); ++it) {
            if ((*it)->getOrderId() == orderId) {
                (*it)->cancel();
                orderQueue.erase(it);
                cleanupEmptyLevels();
                return true;
            }
        }
    }

    // Search asks
    for (auto& [price, orderQueue] : asks_) {
        for (auto it = orderQueue.begin(); it != orderQueue.end(); ++it) {
            if ((*it)->getOrderId() == orderId) {
                (*it)->cancel();
                orderQueue.erase(it);
                cleanupEmptyLevels();
                return true;
            }
        }
    }

    return false;
}

Price OrderBook::getBestBid() const {
    if (bids_.empty()) {
        return 0;
    }
    // With std::greater, begin() gives highest price
    return bids_.begin()->first;
}

Price OrderBook::getBestAsk() const {
    if (asks_.empty()) {
        return 0;
    }
    // Default ordering, begin() gives lowest price
    return asks_.begin()->first;
}

Price OrderBook::getSpread() const {
    Price bestBid = getBestBid();
    Price bestAsk = getBestAsk();

    if (bestBid == 0 || bestAsk == 0) {
        return 0;
    }

    return bestAsk - bestBid;
}

Price OrderBook::getMidPrice() const {
    Price bestBid = getBestBid();
    Price bestAsk = getBestAsk();

    if (bestBid == 0 || bestAsk == 0) {
        return 0;
    }

    return (bestBid + bestAsk) / 2;
}

std::vector<std::pair<Price, Quantity>> OrderBook::getDepth(Side side, size_t numLevels) const {
    std::vector<std::pair<Price, Quantity>> depth;
    depth.reserve(numLevels);

    if (side == Side::Buy) {
        size_t count = 0;
        for (const auto& [price, orderQueue] : bids_) {
            if (count >= numLevels) break;

            Quantity totalQty = 0;
            for (const auto* order : orderQueue) {
                totalQty += order->getRemainingQuantity();
            }
            depth.emplace_back(price, totalQty);
            ++count;
        }
    } else {
        size_t count = 0;
        for (const auto& [price, orderQueue] : asks_) {
            if (count >= numLevels) break;

            Quantity totalQty = 0;
            for (const auto* order : orderQueue) {
                totalQty += order->getRemainingQuantity();
            }
            depth.emplace_back(price, totalQty);
            ++count;
        }
    }

    return depth;
}

void OrderBook::removeOrder(Order* order) {
    if (order->getSide() == Side::Buy) {
        auto priceIt = bids_.find(order->getLimitPrice());
        if (priceIt != bids_.end()) {
            auto& queue = priceIt->second;
            auto it = std::find(queue.begin(), queue.end(), order);
            if (it != queue.end()) {
                queue.erase(it);
            }
        }
    } else {
        auto priceIt = asks_.find(order->getLimitPrice());
        if (priceIt != asks_.end()) {
            auto& queue = priceIt->second;
            auto it = std::find(queue.begin(), queue.end(), order);
            if (it != queue.end()) {
                queue.erase(it);
            }
        }
    }
    cleanupEmptyLevels();
}

bool OrderBook::isEmpty() const {
    return bids_.empty() && asks_.empty();
}

size_t OrderBook::getOrderCount() const {
    size_t count = 0;
    for (const auto& [price, queue] : bids_) {
        count += queue.size();
    }
    for (const auto& [price, queue] : asks_) {
        count += queue.size();
    }
    return count;
}

void OrderBook::cleanupEmptyLevels() {
    // Remove empty bid levels
    for (auto it = bids_.begin(); it != bids_.end(); ) {
        if (it->second.empty()) {
            it = bids_.erase(it);
        } else {
            ++it;
        }
    }

    // Remove empty ask levels
    for (auto it = asks_.begin(); it != asks_.end(); ) {
        if (it->second.empty()) {
            it = asks_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace trading

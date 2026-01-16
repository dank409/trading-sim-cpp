#include "MatchingEngine.h"
#include <algorithm>

namespace trading {

std::vector<Fill> MatchingEngine::processOrder(Order& order, OrderBook& book) {
    std::vector<Fill> fills;

    if (order.getSide() == Side::Buy) {
        fills = matchBuyOrder(order, book);
    } else {
        fills = matchSellOrder(order, book);
    }

    // If limit order has remaining quantity, add to book
    if (order.getOrderType() == OrderType::Limit && !order.isFilled()) {
        book.addOrder(order);
    }

    return fills;
}

std::vector<Fill> MatchingEngine::matchBuyOrder(Order& order, OrderBook& book) {
    std::vector<Fill> fills;
    auto& asks = book.getAsks();

    // Match against asks from lowest price up
    auto it = asks.begin();
    while (it != asks.end() && order.getRemainingQuantity() > 0) {
        Price askPrice = it->first;

        // Check if we can match at this price
        if (!canMatchBuy(order, askPrice)) {
            break;
        }

        auto& orderQueue = it->second;

        // Match against orders at this price level (FIFO)
        while (!orderQueue.empty() && order.getRemainingQuantity() > 0) {
            Order* makerOrder = orderQueue.front();

            Quantity fillQty = std::min(order.getRemainingQuantity(),
                                        makerOrder->getRemainingQuantity());

            // Create fill at maker's price (price improvement for taker)
            fills.emplace_back(
                order.getOrderId(),
                makerOrder->getOrderId(),
                askPrice,
                fillQty,
                order.getTimestamp()
            );

            // Update both orders
            order.fill(fillQty);
            makerOrder->fill(fillQty);

            // Remove maker if fully filled
            if (makerOrder->isFilled()) {
                orderQueue.pop_front();
            }
        }

        // Move to next price level
        if (orderQueue.empty()) {
            it = asks.erase(it);
        } else {
            ++it;
        }
    }

    return fills;
}

std::vector<Fill> MatchingEngine::matchSellOrder(Order& order, OrderBook& book) {
    std::vector<Fill> fills;
    auto& bids = book.getBids();

    // Match against bids from highest price down
    auto it = bids.begin();
    while (it != bids.end() && order.getRemainingQuantity() > 0) {
        Price bidPrice = it->first;

        // Check if we can match at this price
        if (!canMatchSell(order, bidPrice)) {
            break;
        }

        auto& orderQueue = it->second;

        // Match against orders at this price level (FIFO)
        while (!orderQueue.empty() && order.getRemainingQuantity() > 0) {
            Order* makerOrder = orderQueue.front();

            Quantity fillQty = std::min(order.getRemainingQuantity(),
                                        makerOrder->getRemainingQuantity());

            // Create fill at maker's price (price improvement for taker)
            fills.emplace_back(
                order.getOrderId(),
                makerOrder->getOrderId(),
                bidPrice,
                fillQty,
                order.getTimestamp()
            );

            // Update both orders
            order.fill(fillQty);
            makerOrder->fill(fillQty);

            // Remove maker if fully filled
            if (makerOrder->isFilled()) {
                orderQueue.pop_front();
            }
        }

        // Move to next price level
        if (orderQueue.empty()) {
            it = bids.erase(it);
        } else {
            ++it;
        }
    }

    return fills;
}

bool MatchingEngine::canMatchBuy(const Order& order, Price askPrice) const {
    // Market orders always match
    if (order.getOrderType() == OrderType::Market) {
        return true;
    }

    // Limit buy matches if limit price >= ask price
    return order.getLimitPrice() >= askPrice;
}

bool MatchingEngine::canMatchSell(const Order& order, Price bidPrice) const {
    // Market orders always match
    if (order.getOrderType() == OrderType::Market) {
        return true;
    }

    // Limit sell matches if limit price <= bid price
    return order.getLimitPrice() <= bidPrice;
}

} // namespace trading

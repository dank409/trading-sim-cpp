#pragma once

#include "Types.h"
#include "Order.h"
#include "OrderBook.h"
#include "Fill.h"
#include <vector>

namespace trading {

/// Matching engine that processes orders against an order book
/// Implements price-time priority (FIFO) matching
class MatchingEngine {
public:
    /// Default constructor
    MatchingEngine() = default;

    /// Process an order against the order book
    /// Matches the order against the book and returns fills
    /// Unmatched limit orders are added to the book
    /// @param order The incoming order to process
    /// @param book The order book to match against
    /// @return Vector of fills resulting from the match
    std::vector<Fill> processOrder(Order& order, OrderBook& book);

private:
    /// Match a buy order against asks
    /// @param order The incoming buy order
    /// @param book The order book
    /// @return Vector of fills
    std::vector<Fill> matchBuyOrder(Order& order, OrderBook& book);

    /// Match a sell order against bids
    /// @param order The incoming sell order
    /// @param book The order book
    /// @return Vector of fills
    std::vector<Fill> matchSellOrder(Order& order, OrderBook& book);

    /// Check if a buy order can match at a given price
    /// @param order The buy order
    /// @param askPrice The ask price to check
    /// @return true if order can match (market order or limit price >= ask)
    bool canMatchBuy(const Order& order, Price askPrice) const;

    /// Check if a sell order can match at a given price
    /// @param order The sell order
    /// @param bidPrice The bid price to check
    /// @return true if order can match (market order or limit price <= bid)
    bool canMatchSell(const Order& order, Price bidPrice) const;
};

} // namespace trading

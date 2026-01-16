#pragma once

#include "Types.h"
#include "Order.h"
#include <map>
#include <deque>
#include <vector>
#include <utility>
#include <functional>

namespace trading {

/// Order book for a single asset maintaining price-time priority
/// Uses std::map with deques at each price level for FIFO ordering
/// Orders are stored as pointers - ownership is external
class OrderBook {
public:
    /// Construct an order book for a specific asset
    /// @param assetId The asset this book is for
    explicit OrderBook(const AssetId& assetId);

    /// Get the asset ID for this book
    const AssetId& getAssetId() const { return assetId_; }

    /// Add an order to the appropriate side of the book
    /// @param order Reference to the order (ownership remains external)
    void addOrder(Order& order);

    /// Cancel an order and remove it from the book
    /// @param orderId The order to cancel
    /// @return true if order was found and cancelled
    bool cancelOrder(OrderId orderId);

    /// Get the best (highest) bid price
    /// @return Best bid price, or 0 if no bids
    Price getBestBid() const;

    /// Get the best (lowest) ask price
    /// @return Best ask price, or 0 if no asks
    Price getBestAsk() const;

    /// Get the bid-ask spread
    /// @return Best ask - best bid, or 0 if either side empty
    Price getSpread() const;

    /// Get the mid price (average of best bid and ask)
    /// @return (best bid + best ask) / 2, or 0 if either side empty
    Price getMidPrice() const;

    /// Get depth information for one side of the book
    /// @param side Buy (bids) or Sell (asks)
    /// @param numLevels Maximum number of price levels to return
    /// @return Vector of (price, totalQuantity) pairs, best price first
    std::vector<std::pair<Price, Quantity>> getDepth(Side side, size_t numLevels) const;

    /// Remove a specific order from the book (used by matching engine)
    /// @param order The order to remove
    void removeOrder(Order* order);

    /// Get reference to bid side (for matching engine)
    /// Map of price to deque of orders, highest price first (std::greater)
    std::map<Price, std::deque<Order*>, std::greater<Price>>& getBids() { return bids_; }

    /// Get reference to ask side (for matching engine)
    /// Map of price to deque of orders, lowest price first
    std::map<Price, std::deque<Order*>>& getAsks() { return asks_; }

    /// Check if the book is empty on both sides
    bool isEmpty() const;

    /// Get total number of orders in the book
    size_t getOrderCount() const;

private:
    /// Remove empty price levels after order removal
    void cleanupEmptyLevels();

    AssetId assetId_;

    // Bids: highest price first (std::greater), then FIFO at each level
    std::map<Price, std::deque<Order*>, std::greater<Price>> bids_;

    // Asks: lowest price first (default ordering), then FIFO at each level
    std::map<Price, std::deque<Order*>> asks_;
};

} // namespace trading

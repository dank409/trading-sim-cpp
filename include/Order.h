#pragma once

#include "Types.h"
#include <atomic>

namespace trading {

/// Represents an order in the trading system
/// Orders have unique IDs and track their fill status
class Order {
public:
    /// Construct a limit order
    /// @param assetId The asset to trade
    /// @param side Buy or Sell
    /// @param quantity Number of units to trade
    /// @param limitPrice Price limit for the order
    /// @param timestamp When the order was placed
    Order(const AssetId& assetId, Side side, Quantity quantity,
          Price limitPrice, Timestamp timestamp);

    /// Construct a market order (limitPrice = 0)
    /// @param assetId The asset to trade
    /// @param side Buy or Sell
    /// @param quantity Number of units to trade
    /// @param timestamp When the order was placed
    Order(const AssetId& assetId, Side side, Quantity quantity, Timestamp timestamp);

    // Accessors
    OrderId getOrderId() const { return orderId_; }
    const AssetId& getAssetId() const { return assetId_; }
    Side getSide() const { return side_; }
    OrderType getOrderType() const { return orderType_; }
    Quantity getQuantity() const { return quantity_; }
    Quantity getFilledQuantity() const { return filledQuantity_; }
    Price getLimitPrice() const { return limitPrice_; }
    Timestamp getTimestamp() const { return timestamp_; }
    OrderStatus getStatus() const { return status_; }

    /// Get remaining unfilled quantity
    /// @return quantity - filledQuantity
    Quantity getRemainingQuantity() const;

    /// Check if order is completely filled
    /// @return true if filledQuantity >= quantity
    bool isFilled() const;

    /// Cancel the order
    /// Sets status to Cancelled if not already Filled
    void cancel();

    /// Fill part of the order
    /// @param fillQuantity Amount to fill
    /// Updates filledQuantity and status appropriately
    void fill(Quantity fillQuantity);

    /// Reset the static order ID counter (for testing)
    static void resetIdCounter();

private:
    /// Generate a unique order ID
    static OrderId generateOrderId();

    static std::atomic<OrderId> nextOrderId_;

    OrderId orderId_;
    AssetId assetId_;
    Side side_;
    OrderType orderType_;
    Quantity quantity_;
    Quantity filledQuantity_;
    Price limitPrice_;
    Timestamp timestamp_;
    OrderStatus status_;
};

} // namespace trading

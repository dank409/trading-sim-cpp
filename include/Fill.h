#pragma once

#include "Types.h"

namespace trading {

/// Represents a trade/fill record capturing executed trade details
/// Used for trade history and portfolio updates
struct Fill {
    OrderId orderId;          // Taker order ID (aggressive order)
    OrderId counterOrderId;   // Maker order ID (passive order on book)
    Price price;              // Execution price (maker's price)
    Quantity quantity;        // Quantity filled
    Timestamp timestamp;      // When the fill occurred

    /// Default constructor
    Fill() = default;

    /// Construct a fill with all fields
    Fill(OrderId order, OrderId counterOrder, Price fillPrice,
         Quantity fillQuantity, Timestamp fillTimestamp)
        : orderId(order)
        , counterOrderId(counterOrder)
        , price(fillPrice)
        , quantity(fillQuantity)
        , timestamp(fillTimestamp)
    {
    }

    /// Equality comparison
    bool operator==(const Fill& other) const {
        return orderId == other.orderId &&
               counterOrderId == other.counterOrderId &&
               price == other.price &&
               quantity == other.quantity &&
               timestamp == other.timestamp;
    }

    bool operator!=(const Fill& other) const {
        return !(*this == other);
    }
};

} // namespace trading

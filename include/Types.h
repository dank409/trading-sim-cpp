#pragma once

#include <cstdint>
#include <string>

namespace trading {

// Core type definitions for the trading simulator
// All prices use integer arithmetic (cents or basis points) for precision

/// Price type - represents price in cents or basis points (integer for precision)
using Price = int;

/// Quantity type - signed integer to support both long (positive) and short (negative) positions
using Quantity = int;

/// Asset identifier - string-based for flexibility (e.g., "AAPL", "BTC")
using AssetId = std::string;

/// Order identifier - unique ID for each order
using OrderId = uint64_t;

/// Timestamp - represents simulation day/tick (discrete time steps)
using Timestamp = int;

/// Side of a trade - whether buying or selling
enum class Side {
    Buy,
    Sell
};

/// Type of order - market executes immediately, limit waits for price
enum class OrderType {
    Market,
    Limit
};

/// Current status of an order in the system
enum class OrderStatus {
    Pending,         // Order submitted but not yet processed
    PartiallyFilled, // Some quantity filled, some remaining
    Filled,          // Entire order quantity executed
    Cancelled        // Order cancelled before full execution
};

} // namespace trading

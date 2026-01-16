#pragma once

#include "Types.h"
#include <string>

namespace trading {

/// Represents a tradeable asset in the simulation
/// Contains metadata about the asset including trading constraints
class Asset {
public:
    /// Construct an asset with all properties
    /// @param id Unique identifier for the asset
    /// @param symbol Human-readable symbol (e.g., "AAPL")
    /// @param tickSize Minimum price increment (in cents/basis points)
    /// @param lotSize Minimum quantity increment
    Asset(const AssetId& id, const std::string& symbol, Price tickSize = 1, Quantity lotSize = 1);

    /// Factory method to create a simple asset with default tick/lot sizes
    static Asset createSimple(const AssetId& id, const std::string& symbol);

    // Accessors
    const AssetId& getId() const { return id_; }
    const std::string& getSymbol() const { return symbol_; }
    Price getTickSize() const { return tickSize_; }
    Quantity getLotSize() const { return lotSize_; }

    // Comparison operators for use in containers
    bool operator==(const Asset& other) const;
    bool operator!=(const Asset& other) const;
    bool operator<(const Asset& other) const;

private:
    AssetId id_;          // Unique identifier
    std::string symbol_;  // Display symbol
    Price tickSize_;      // Minimum price movement
    Quantity lotSize_;    // Minimum quantity increment
};

} // namespace trading

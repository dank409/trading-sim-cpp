#pragma once

#include "Types.h"
#include <map>
#include <vector>

namespace trading {

/// Stores price history for multiple assets over time
/// Provides efficient access to current and historical prices
class PriceHistory {
public:
    /// Default constructor
    PriceHistory() = default;

    /// Record a price for an asset at a given timestamp
    /// @param assetId The asset identifier
    /// @param timestamp The simulation timestamp
    /// @param price The price to record
    void recordPrice(const AssetId& assetId, Timestamp timestamp, Price price);

    /// Get the price at a specific timestamp
    /// @param assetId The asset identifier
    /// @param timestamp The simulation timestamp
    /// @return The price at that timestamp, or 0 if not found
    Price getPrice(const AssetId& assetId, Timestamp timestamp) const;

    /// Get the most recent recorded price for an asset
    /// @param assetId The asset identifier
    /// @return The latest price, or 0 if no prices recorded
    Price getLatestPrice(const AssetId& assetId) const;

    /// Get the full price history for an asset
    /// @param assetId The asset identifier
    /// @return Vector of all recorded prices in chronological order
    std::vector<Price> getPriceHistory(const AssetId& assetId) const;

    /// Check if an asset has any recorded prices
    /// @param assetId The asset identifier
    /// @return true if prices exist for this asset
    bool hasAsset(const AssetId& assetId) const;

    /// Clear all recorded price history
    void clear();

private:
    /// Map of asset ID to vector of prices (indexed by timestamp)
    std::map<AssetId, std::vector<Price>> priceData_;
};

} // namespace trading

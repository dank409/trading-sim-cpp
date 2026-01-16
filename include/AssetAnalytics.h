#pragma once

#include "Types.h"
#include "TradeLog.h"
#include <unordered_map>
#include <vector>

namespace trading {

/// Per-asset statistics
struct AssetStats {
    AssetId assetId;
    Price totalPnL = 0;          // Total realized PnL for this asset
    size_t tradeCount = 0;       // Number of trades
    size_t winningTrades = 0;    // Number of winning trades
    size_t losingTrades = 0;     // Number of losing trades
    int winRate = 0;             // Win rate in basis points (0-10000)
    int64_t totalVolume = 0;     // Total trading volume (qty * price)
    Price grossProfit = 0;       // Total profit from winning trades
    Price grossLoss = 0;         // Total loss from losing trades (negative)
};

/// Provides per-asset breakdown of trading performance
/// Analyzes which assets contributed most to profits/losses
class AssetAnalytics {
public:
    /// Default constructor
    AssetAnalytics() = default;

    /// Analyze trade log and compute per-asset statistics
    /// @param tradeLog The trade history to analyze
    void analyze(const TradeLog& tradeLog);

    /// Get PnL for a specific asset
    /// @param assetId The asset to query
    /// @return Total realized PnL for the asset
    Price getAssetPnL(const AssetId& assetId) const;

    /// Get trade count for a specific asset
    /// @param assetId The asset to query
    /// @return Number of trades in that asset
    size_t getAssetTradeCount(const AssetId& assetId) const;

    /// Get win rate for a specific asset
    /// @param assetId The asset to query
    /// @return Win rate in basis points (0-10000)
    int getAssetWinRate(const AssetId& assetId) const;

    /// Get trading volume for a specific asset
    /// @param assetId The asset to query
    /// @return Total volume (sum of qty * price)
    int64_t getAssetVolume(const AssetId& assetId) const;

    /// Get full statistics for a specific asset
    /// @param assetId The asset to query
    /// @return Pointer to AssetStats, or nullptr if asset not found
    const AssetStats* getAssetStats(const AssetId& assetId) const;

    /// Get the most profitable asset
    /// @return AssetId with highest total PnL, or empty string if no trades
    AssetId getMostProfitableAsset() const;

    /// Get the least profitable (most losing) asset
    /// @return AssetId with lowest total PnL, or empty string if no trades
    AssetId getLeastProfitableAsset() const;

    /// Get the most traded asset by trade count
    /// @return AssetId with most trades, or empty string if no trades
    AssetId getMostTradedAsset() const;

    /// Get the most traded asset by volume
    /// @return AssetId with highest volume, or empty string if no trades
    AssetId getHighestVolumeAsset() const;

    /// Get all asset IDs that have been traded
    /// @return Vector of asset IDs
    std::vector<AssetId> getAllAssets() const;

    /// Get all asset statistics sorted by PnL (descending)
    /// @return Vector of AssetStats sorted by total PnL
    std::vector<AssetStats> getAssetStatsByPnL() const;

    /// Get all asset statistics sorted by volume (descending)
    /// @return Vector of AssetStats sorted by total volume
    std::vector<AssetStats> getAssetStatsByVolume() const;

    /// Clear all analytics data
    void clear() { assetStats_.clear(); }

private:
    std::unordered_map<AssetId, AssetStats> assetStats_;
};

} // namespace trading

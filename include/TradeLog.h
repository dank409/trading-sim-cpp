#pragma once

#include "Types.h"
#include <vector>
#include <unordered_map>

namespace trading {

/// Record of a single executed trade
/// Captures all details needed for analytics and reporting
struct TradeRecord {
    Timestamp timestamp;      // When the trade occurred
    AssetId assetId;          // Which asset was traded
    Side side;                // Buy or Sell
    Quantity quantity;        // Number of shares traded
    Price price;              // Execution price
    OrderId orderId;          // Associated order ID
    uint64_t fillId;          // Unique fill identifier
    Price pnlRealized;        // Realized PnL if closing trade (0 otherwise)

    /// Default constructor
    TradeRecord() = default;

    /// Full constructor
    TradeRecord(Timestamp ts, const AssetId& asset, Side s, Quantity qty,
                Price px, OrderId order, uint64_t fill, Price pnl = 0)
        : timestamp(ts)
        , assetId(asset)
        , side(s)
        , quantity(qty)
        , price(px)
        , orderId(order)
        , fillId(fill)
        , pnlRealized(pnl)
    {
    }

    /// Check if this trade was profitable (realized PnL > 0)
    bool isProfitable() const { return pnlRealized > 0; }

    /// Check if this trade was a loss (realized PnL < 0)
    bool isLoss() const { return pnlRealized < 0; }

    /// Check if this trade realized any PnL (closing trade)
    bool hasRealizedPnL() const { return pnlRealized != 0; }
};

/// Maintains a log of all executed trades
/// Provides methods for querying trade history and computing statistics
class TradeLog {
public:
    /// Default constructor
    TradeLog() = default;

    /// Record a new trade in the log
    /// @param trade The trade record to add
    void recordTrade(const TradeRecord& trade);

    /// Get all recorded trades
    /// @return Vector of all trade records in chronological order
    const std::vector<TradeRecord>& getTrades() const { return trades_; }

    /// Get trades for a specific asset
    /// @param assetId The asset to filter by
    /// @return Vector of trades for that asset
    std::vector<TradeRecord> getTradesByAsset(const AssetId& assetId) const;

    /// Get total number of trades
    /// @return Count of all recorded trades
    size_t getTradeCount() const { return trades_.size(); }

    /// Get total trading volume across all trades
    /// @return Sum of quantity * price for all trades
    int64_t getTotalVolume() const;

    /// Get win rate as a percentage (0-10000 for 0-100.00%)
    /// Only considers trades with realized PnL
    /// @return Win rate in basis points (e.g., 5000 = 50.00%)
    int getWinRate() const;

    /// Get total realized PnL across all trades
    /// @return Sum of all realized PnL values
    Price getTotalRealizedPnL() const;

    /// Get count of trades with realized PnL (closing trades)
    /// @return Number of trades that closed positions
    size_t getClosingTradeCount() const;

    /// Get count of winning trades
    /// @return Number of trades with positive realized PnL
    size_t getWinningTradeCount() const;

    /// Get count of losing trades
    /// @return Number of trades with negative realized PnL
    size_t getLosingTradeCount() const;

    /// Clear all recorded trades
    void clear() { trades_.clear(); }

private:
    std::vector<TradeRecord> trades_;  // All recorded trades
};

} // namespace trading

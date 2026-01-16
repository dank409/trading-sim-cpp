#pragma once

#include "Types.h"

namespace trading {

/// Tracks a position in a single asset
/// Handles both long (positive quantity) and short (negative quantity) positions
/// Uses average cost accounting for entry price tracking
class Position {
public:
    /// Construct a position for an asset
    /// @param assetId The asset this position tracks
    explicit Position(const AssetId& assetId);

    /// Add to the position (buying more shares or covering shorts)
    /// Updates average entry price using weighted average
    /// @param quantity Number of shares to add (positive for long, negative for short)
    /// @param price Price per share (in cents/basis points)
    void addToPosition(Quantity quantity, Price price);

    /// Reduce the position (selling longs or adding to shorts)
    /// Realizes PnL based on difference from entry price
    /// @param quantity Number of shares to reduce (positive value)
    /// @param price Price per share (in cents/basis points)
    void reducePosition(Quantity quantity, Price price);

    /// Calculate unrealized PnL at current market price
    /// @param currentPrice Current market price of the asset
    /// @return Unrealized profit/loss in cents/basis points
    Price getUnrealizedPnL(Price currentPrice) const;

    // Accessors
    const AssetId& getAssetId() const { return assetId_; }
    Quantity getQuantity() const { return quantity_; }
    Price getAvgEntryPrice() const { return avgEntryPrice_; }
    Price getRealizedPnL() const { return realizedPnL_; }

    /// Check if position is flat (no exposure)
    bool isFlat() const { return quantity_ == 0; }

    /// Check if position is long (positive exposure)
    bool isLong() const { return quantity_ > 0; }

    /// Check if position is short (negative exposure)
    bool isShort() const { return quantity_ < 0; }

private:
    AssetId assetId_;       // The asset this position is for
    Quantity quantity_;     // Current position size (positive=long, negative=short)
    Price avgEntryPrice_;   // Average entry price (weighted by quantity)
    Price realizedPnL_;     // Cumulative realized profit/loss
};

} // namespace trading

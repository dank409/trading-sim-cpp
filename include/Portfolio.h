#pragma once

#include "Types.h"
#include "Position.h"
#include "RiskConfig.h"
#include "RiskMetrics.h"
#include <unordered_map>

namespace trading {

/// Manages a portfolio of positions across multiple assets
/// Tracks cash balance, positions, and provides aggregate metrics
class Portfolio {
public:
    /// Construct a portfolio with initial cash balance
    /// @param initialCash Starting cash balance (in cents/basis points)
    explicit Portfolio(Price initialCash = 0);

    /// Get the position for a specific asset
    /// @param assetId The asset to look up
    /// @return Pointer to position, or nullptr if no position exists
    const Position* getPosition(const AssetId& assetId) const;

    /// Get mutable position for a specific asset
    /// @param assetId The asset to look up
    /// @return Pointer to position, or nullptr if no position exists
    Position* getPosition(const AssetId& assetId);

    /// Update position for an asset (buy or sell)
    /// Adjusts cash balance and position accordingly
    /// @param assetId The asset being traded
    /// @param quantity Number of shares (positive for buy, negative for sell)
    /// @param price Price per share
    /// @param side Whether this is a buy or sell order
    void updatePosition(const AssetId& assetId, Quantity quantity, Price price, Side side);

    /// Calculate total portfolio equity at current market prices
    /// Equity = Cash + Sum of (position value) for all positions
    /// @param priceMap Map of AssetId to current market price
    /// @return Total equity value
    Price getEquity(const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Get total realized PnL across all positions
    /// @return Sum of realized PnL from all closed/reduced positions
    Price getTotalRealizedPnL() const;

    /// Get total unrealized PnL across all positions
    /// @param priceMap Map of AssetId to current market price
    /// @return Sum of unrealized PnL from all open positions
    Price getTotalUnrealizedPnL(const std::unordered_map<AssetId, Price>& priceMap) const;

    // Cash management
    Price getCash() const { return cash_; }
    void setCash(Price cash) { cash_ = cash; }
    void adjustCash(Price amount) { cash_ += amount; }

    /// Get all positions (for iteration/display)
    const std::unordered_map<AssetId, Position>& getPositions() const { return positions_; }

    /// Check if portfolio has any open positions
    bool hasOpenPositions() const;

    /// Get number of assets with open positions
    size_t getPositionCount() const;

    /// Get risk metrics snapshot for this portfolio
    /// @param priceMap Current market prices for all assets
    /// @param config Risk configuration parameters
    /// @return RiskMetrics snapshot of current risk state
    RiskMetrics getRiskMetrics(const std::unordered_map<AssetId, Price>& priceMap,
                               const RiskConfig& config) const;

private:
    Price cash_;  // Available cash balance
    std::unordered_map<AssetId, Position> positions_;  // Asset positions
};

} // namespace trading

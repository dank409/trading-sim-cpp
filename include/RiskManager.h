#pragma once

#include "Types.h"
#include "RiskConfig.h"
#include "RiskMetrics.h"
#include "Portfolio.h"
#include "Order.h"
#include <string>
#include <unordered_map>

namespace trading {

/// Result of a risk validation check
struct RiskCheckResult {
    /// Whether the trade is approved
    bool approved;

    /// Reason for rejection (empty if approved)
    std::string reason;

    /// If rejected, the maximum quantity that would be allowed
    Quantity maxAllowedQuantity;

    /// Default constructor - approved with no restrictions
    RiskCheckResult()
        : approved(true)
        , reason("")
        , maxAllowedQuantity(0)
    {
    }

    /// Constructor for rejected order
    RiskCheckResult(const std::string& rejectionReason, Quantity maxAllowed)
        : approved(false)
        , reason(rejectionReason)
        , maxAllowedQuantity(maxAllowed)
    {
    }

    /// Static factory for approved result
    static RiskCheckResult Approved() {
        return RiskCheckResult();
    }

    /// Static factory for rejected result
    static RiskCheckResult Rejected(const std::string& reason, Quantity maxAllowed = 0) {
        return RiskCheckResult(reason, maxAllowed);
    }
};

/// Manages risk validation and monitoring for a portfolio
/// Enforces margin requirements, position limits, and leverage limits
class RiskManager {
public:
    /// Construct a RiskManager with given configuration
    /// @param config Risk configuration parameters
    explicit RiskManager(const RiskConfig& config);

    /// Get the current risk configuration
    const RiskConfig& getConfig() const { return config_; }

    /// Update risk configuration
    void setConfig(const RiskConfig& config) { config_ = config; }

    // ========== Pre-trade Validation Methods ==========

    /// Check if a position can be opened given current portfolio state
    /// @param portfolio Current portfolio state
    /// @param assetId Asset to trade
    /// @param quantity Number of units to trade
    /// @param price Price per unit
    /// @param side Buy or Sell
    /// @param priceMap Current market prices for all assets
    /// @return true if trade passes all risk checks
    bool canOpenPosition(const Portfolio& portfolio,
                         const AssetId& assetId,
                         Quantity quantity,
                         Price price,
                         Side side,
                         const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Get the maximum order quantity that would pass all risk checks
    /// @param portfolio Current portfolio state
    /// @param assetId Asset to trade
    /// @param price Price per unit
    /// @param side Buy or Sell
    /// @param priceMap Current market prices for all assets
    /// @return Maximum quantity that passes all risk checks
    Quantity getMaxOrderQuantity(const Portfolio& portfolio,
                                  const AssetId& assetId,
                                  Price price,
                                  Side side,
                                  const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Validate an order against all risk rules
    /// @param portfolio Current portfolio state
    /// @param order The order to validate
    /// @param currentPrice Current market price of the asset
    /// @param priceMap Current market prices for all assets
    /// @return RiskCheckResult with approval status and details
    RiskCheckResult validateOrder(const Portfolio& portfolio,
                                   const Order& order,
                                   Price currentPrice,
                                   const std::unordered_map<AssetId, Price>& priceMap) const;

    // ========== Margin Calculation Methods ==========

    /// Calculate initial margin required for a new position
    /// @param quantity Number of units
    /// @param price Price per unit
    /// @return Margin required (quantity * price * initialMarginPct / 100)
    int calculateInitialMargin(Quantity quantity, Price price) const;

    /// Calculate maintenance margin for an existing position
    /// @param quantity Number of units (absolute value used)
    /// @param price Price per unit
    /// @return Maintenance margin (|quantity| * price * maintenanceMarginPct / 100)
    int calculateMaintenanceMargin(Quantity quantity, Price price) const;

    /// Calculate total margin used by all positions
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return Total margin used across all positions
    int calculateTotalMarginUsed(const Portfolio& portfolio,
                                  const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Calculate available margin for new positions
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return Available margin (equity - margin used)
    int calculateAvailableMargin(const Portfolio& portfolio,
                                  const std::unordered_map<AssetId, Price>& priceMap) const;

    // ========== Position Monitoring Methods ==========

    /// Check if a margin call should be triggered
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return true if portfolio is below maintenance margin threshold
    bool checkMarginCall(const Portfolio& portfolio,
                          const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Get current margin utilization as a percentage
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return Margin utilization percentage (0-100+)
    int getMarginUtilization(const Portfolio& portfolio,
                              const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Get total absolute exposure across all positions
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return Sum of |position_value| for all positions
    int getExposure(const Portfolio& portfolio,
                     const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Get current leverage ratio
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return Leverage ratio as percentage (100 = 1:1, 200 = 2:1)
    int getLeverageRatio(const Portfolio& portfolio,
                          const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Generate complete risk metrics snapshot
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices
    /// @return RiskMetrics with all current risk measures
    RiskMetrics generateRiskMetrics(const Portfolio& portfolio,
                                     const std::unordered_map<AssetId, Price>& priceMap) const;

private:
    RiskConfig config_;

    /// Check if order would exceed position limit for asset
    bool checkPositionLimit(const Portfolio& portfolio,
                            const AssetId& assetId,
                            Quantity quantity,
                            Side side) const;

    /// Check if order would exceed total exposure limit
    bool checkExposureLimit(const Portfolio& portfolio,
                            const AssetId& assetId,
                            Quantity quantity,
                            Price price,
                            Side side,
                            const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Check if order would exceed leverage limit
    bool checkLeverageLimit(const Portfolio& portfolio,
                            const AssetId& assetId,
                            Quantity quantity,
                            Price price,
                            Side side,
                            const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Check if sufficient margin is available for order
    bool checkMarginAvailable(const Portfolio& portfolio,
                              Quantity quantity,
                              Price price,
                              const std::unordered_map<AssetId, Price>& priceMap) const;
};

} // namespace trading

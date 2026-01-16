#pragma once

#include "Types.h"
#include <string>

namespace trading {

/// Snapshot of current risk state for a portfolio
/// All values use integer arithmetic for precision
struct RiskMetrics {
    /// Total absolute exposure across all positions
    /// Sum of |position_value| for all assets
    int totalExposure;

    /// Total margin currently being used
    /// Based on maintenance margin for existing positions
    int marginUsed;

    /// Margin available for new positions
    /// equity - marginUsed
    int marginAvailable;

    /// Current leverage ratio (as percentage)
    /// totalExposure / equity * 100
    /// e.g., 150 = 1.5:1 leverage, 200 = 2:1 leverage
    int leverageRatio;

    /// Asset ID of the largest position by absolute value
    AssetId largestPositionAsset;

    /// Size (quantity) of the largest position
    Quantity largestPositionSize;

    /// Value of the largest position
    int largestPositionValue;

    /// True if portfolio is below maintenance margin threshold
    bool marginCallTriggered;

    /// Current portfolio equity
    int equity;

    /// Default constructor
    RiskMetrics()
        : totalExposure(0)
        , marginUsed(0)
        , marginAvailable(0)
        , leverageRatio(0)
        , largestPositionAsset("")
        , largestPositionSize(0)
        , largestPositionValue(0)
        , marginCallTriggered(false)
        , equity(0)
    {
    }
};

} // namespace trading

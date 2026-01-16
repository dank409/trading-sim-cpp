#pragma once

#include "Types.h"

namespace trading {

/// Configuration for risk management parameters
/// All percentage values are stored as integers (e.g., 50 = 50%)
struct RiskConfig {
    /// Initial margin percentage required to open a position
    /// e.g., 50 = 50% margin required (can open $200 worth with $100)
    int initialMarginPct;

    /// Maintenance margin percentage to maintain a position
    /// If equity falls below this, a margin call is triggered
    /// e.g., 25 = 25% maintenance margin
    int maintenanceMarginPct;

    /// Maximum position size per asset (in shares/units)
    /// Prevents excessive concentration in any single asset
    Quantity maxPositionPerAsset;

    /// Maximum total exposure across all positions (absolute value sum)
    /// Limits total portfolio risk regardless of diversification
    int maxTotalExposure;

    /// Maximum leverage ratio (as percentage, e.g., 200 = 2:1, 400 = 4:1)
    /// Leverage = totalExposure / equity * 100
    int maxLeverageRatio;

    /// Default constructor with conservative defaults
    RiskConfig()
        : initialMarginPct(50)      // 50% initial margin (2:1 leverage on entry)
        , maintenanceMarginPct(25)  // 25% maintenance margin
        , maxPositionPerAsset(100)  // Max 100 shares per asset
        , maxTotalExposure(100000)  // Max $1000 total exposure (in cents)
        , maxLeverageRatio(200)     // Max 2:1 leverage
    {
    }

    /// Constructor with custom parameters
    RiskConfig(int initMargin, int maintMargin, Quantity maxPerAsset,
               int maxExposure, int maxLeverage)
        : initialMarginPct(initMargin)
        , maintenanceMarginPct(maintMargin)
        , maxPositionPerAsset(maxPerAsset)
        , maxTotalExposure(maxExposure)
        , maxLeverageRatio(maxLeverage)
    {
    }
};

} // namespace trading

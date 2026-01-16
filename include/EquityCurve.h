#pragma once

#include "Types.h"
#include <vector>
#include <utility>

namespace trading {

/// Represents a single equity point in time
struct EquityPoint {
    Timestamp timestamp;
    Price equity;

    EquityPoint() : timestamp(0), equity(0) {}
    EquityPoint(Timestamp ts, Price eq) : timestamp(ts), equity(eq) {}
};

/// Tracks portfolio equity over time and calculates drawdown metrics
/// Provides analysis of equity progression and peak-to-trough drawdowns
class EquityCurve {
public:
    /// Default constructor
    EquityCurve() = default;

    /// Record an equity value at a specific timestamp
    /// @param timestamp The simulation timestamp
    /// @param equity The equity value at that time
    void recordEquity(Timestamp timestamp, Price equity);

    /// Get the full equity history
    /// @return Vector of (timestamp, equity) pairs in chronological order
    const std::vector<EquityPoint>& getEquityHistory() const { return history_; }

    /// Get the starting equity value
    /// @return First recorded equity, or 0 if no history
    Price getStartingEquity() const;

    /// Get the ending (most recent) equity value
    /// @return Last recorded equity, or 0 if no history
    Price getEndingEquity() const;

    /// Get the net change in equity
    /// @return Ending equity minus starting equity
    Price getNetChange() const;

    /// Get the highest equity ever reached
    /// @return Peak equity value
    Price getPeakEquity() const;

    /// Get the current drawdown from peak as basis points
    /// @return (peak - current) / peak * 10000
    int getCurrentDrawdown() const;

    /// Get the maximum drawdown experienced as basis points
    /// @return Largest drawdown from peak in basis points
    int getMaxDrawdown() const;

    /// Get the timestamp when max drawdown occurred
    /// @return Timestamp of the lowest point during max drawdown
    Timestamp getMaxDrawdownTimestamp() const;

    /// Get the duration of the current drawdown
    /// @return Number of time steps since peak equity
    int getDrawdownDuration() const;

    /// Get the number of recorded equity points
    /// @return Count of equity records
    size_t size() const { return history_.size(); }

    /// Check if equity history is empty
    /// @return true if no equity recorded
    bool empty() const { return history_.empty(); }

    /// Clear all equity history
    void clear();

private:
    std::vector<EquityPoint> history_;  // Chronological equity records
    Price peakEquity_ = 0;              // Running peak equity
    Timestamp peakTimestamp_ = 0;       // When peak was reached
    int maxDrawdown_ = 0;               // Maximum drawdown in basis points
    Timestamp maxDrawdownTimestamp_ = 0; // When max drawdown occurred
};

} // namespace trading

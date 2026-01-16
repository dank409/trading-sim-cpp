#pragma once

#include "Types.h"
#include "TradeLog.h"
#include "EquityCurve.h"
#include <vector>

namespace trading {

/// Performance metrics calculated from trade history and equity curve
/// All percentage values are in basis points (e.g., 2500 = 25.00%)
struct PerformanceMetrics {
    // Return metrics
    int totalReturn = 0;        // In basis points (ending/starting - 1) * 10000
    int sharpeRatio = 0;        // Scaled by 100 (so 145 = 1.45 Sharpe)

    // Trade statistics
    int winRate = 0;            // In basis points (e.g., 5833 = 58.33%)
    int profitFactor = 0;       // Scaled by 100 (so 182 = 1.82 profit factor)
    Price averageWin = 0;       // Average profit on winning trades
    Price averageLoss = 0;      // Average loss on losing trades (negative)
    Price largestWin = 0;       // Largest single winning trade
    Price largestLoss = 0;      // Largest single losing trade (negative)

    // Streak statistics
    int maxConsecutiveWins = 0;   // Longest winning streak
    int maxConsecutiveLosses = 0; // Longest losing streak

    // Holding period
    int averageHoldingPeriod = 0; // Average time in position (time steps)

    // Drawdown metrics
    int maxDrawdown = 0;        // In basis points
    int maxDrawdownDuration = 0; // In time steps

    // Trade counts
    size_t totalTrades = 0;
    size_t winningTrades = 0;
    size_t losingTrades = 0;
    Price grossProfit = 0;
    Price grossLoss = 0;
};

/// Calculates performance statistics from trade history and equity data
/// All calculations use integer arithmetic for consistency
class PerformanceStats {
public:
    /// Default constructor
    PerformanceStats() = default;

    /// Calculate all performance metrics
    /// @param tradeLog The trade history to analyze
    /// @param equityCurve The equity progression to analyze
    /// @return Filled PerformanceMetrics struct
    PerformanceMetrics calculate(const TradeLog& tradeLog, const EquityCurve& equityCurve);

    /// Get the last calculated metrics
    /// @return Most recent performance metrics
    const PerformanceMetrics& getMetrics() const { return metrics_; }

    // Static helper functions for individual calculations

    /// Calculate total return in basis points
    /// @param startingEquity Initial equity
    /// @param endingEquity Final equity
    /// @return Return in basis points (e.g., 2500 = 25.00%)
    static int calculateTotalReturn(Price startingEquity, Price endingEquity);

    /// Calculate Sharpe ratio from period returns
    /// @param returns Vector of period returns
    /// @return Sharpe ratio scaled by 100 (145 = 1.45)
    static int calculateSharpeRatio(const std::vector<int>& returns);

    /// Calculate profit factor
    /// @param grossProfit Total profit from winning trades
    /// @param grossLoss Total loss from losing trades (should be negative)
    /// @return Profit factor scaled by 100 (182 = 1.82)
    static int calculateProfitFactor(Price grossProfit, Price grossLoss);

    /// Calculate win rate in basis points
    /// @param wins Number of winning trades
    /// @param total Total number of trades with realized PnL
    /// @return Win rate in basis points (5833 = 58.33%)
    static int calculateWinRate(size_t wins, size_t total);

    /// Integer square root approximation
    /// @param n The number to take square root of
    /// @return Integer approximation of sqrt(n)
    static int64_t integerSqrt(int64_t n);

private:
    PerformanceMetrics metrics_;

    /// Analyze trades to extract win/loss statistics
    void analyzeTradeStatistics(const TradeLog& tradeLog);

    /// Calculate streak statistics (consecutive wins/losses)
    void analyzeStreaks(const TradeLog& tradeLog);

    /// Calculate period returns from equity curve
    std::vector<int> calculatePeriodReturns(const EquityCurve& equityCurve);
};

} // namespace trading

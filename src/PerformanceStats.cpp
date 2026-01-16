#include "PerformanceStats.h"
#include <algorithm>
#include <cmath>

namespace trading {

PerformanceMetrics PerformanceStats::calculate(const TradeLog& tradeLog,
                                                const EquityCurve& equityCurve) {
    // Reset metrics
    metrics_ = PerformanceMetrics();

    // Calculate return metrics from equity curve
    if (!equityCurve.empty()) {
        Price startEq = equityCurve.getStartingEquity();
        Price endEq = equityCurve.getEndingEquity();
        metrics_.totalReturn = calculateTotalReturn(startEq, endEq);
        metrics_.maxDrawdown = equityCurve.getMaxDrawdown();
        metrics_.maxDrawdownDuration = equityCurve.getDrawdownDuration();

        // Calculate Sharpe ratio from period returns
        std::vector<int> returns = calculatePeriodReturns(equityCurve);
        metrics_.sharpeRatio = calculateSharpeRatio(returns);
    }

    // Analyze trade statistics
    analyzeTradeStatistics(tradeLog);

    // Analyze consecutive wins/losses
    analyzeStreaks(tradeLog);

    return metrics_;
}

void PerformanceStats::analyzeTradeStatistics(const TradeLog& tradeLog) {
    const auto& trades = tradeLog.getTrades();

    metrics_.totalTrades = trades.size();
    metrics_.winningTrades = 0;
    metrics_.losingTrades = 0;
    metrics_.grossProfit = 0;
    metrics_.grossLoss = 0;
    metrics_.largestWin = 0;
    metrics_.largestLoss = 0;

    for (const auto& trade : trades) {
        if (trade.pnlRealized > 0) {
            metrics_.winningTrades++;
            metrics_.grossProfit += trade.pnlRealized;
            if (trade.pnlRealized > metrics_.largestWin) {
                metrics_.largestWin = trade.pnlRealized;
            }
        } else if (trade.pnlRealized < 0) {
            metrics_.losingTrades++;
            metrics_.grossLoss += trade.pnlRealized;  // Will be negative
            if (trade.pnlRealized < metrics_.largestLoss) {
                metrics_.largestLoss = trade.pnlRealized;
            }
        }
    }

    // Calculate averages
    if (metrics_.winningTrades > 0) {
        metrics_.averageWin = metrics_.grossProfit / static_cast<Price>(metrics_.winningTrades);
    }
    if (metrics_.losingTrades > 0) {
        metrics_.averageLoss = metrics_.grossLoss / static_cast<Price>(metrics_.losingTrades);
    }

    // Calculate win rate
    size_t closingTrades = metrics_.winningTrades + metrics_.losingTrades;
    metrics_.winRate = calculateWinRate(metrics_.winningTrades, closingTrades);

    // Calculate profit factor
    metrics_.profitFactor = calculateProfitFactor(metrics_.grossProfit, metrics_.grossLoss);
}

void PerformanceStats::analyzeStreaks(const TradeLog& tradeLog) {
    const auto& trades = tradeLog.getTrades();

    int currentWinStreak = 0;
    int currentLossStreak = 0;
    metrics_.maxConsecutiveWins = 0;
    metrics_.maxConsecutiveLosses = 0;

    for (const auto& trade : trades) {
        if (trade.pnlRealized > 0) {
            // Winning trade
            currentWinStreak++;
            currentLossStreak = 0;
            if (currentWinStreak > metrics_.maxConsecutiveWins) {
                metrics_.maxConsecutiveWins = currentWinStreak;
            }
        } else if (trade.pnlRealized < 0) {
            // Losing trade
            currentLossStreak++;
            currentWinStreak = 0;
            if (currentLossStreak > metrics_.maxConsecutiveLosses) {
                metrics_.maxConsecutiveLosses = currentLossStreak;
            }
        }
        // Trades with 0 PnL don't affect streaks
    }
}

std::vector<int> PerformanceStats::calculatePeriodReturns(const EquityCurve& equityCurve) {
    std::vector<int> returns;
    const auto& history = equityCurve.getEquityHistory();

    if (history.size() < 2) {
        return returns;
    }

    returns.reserve(history.size() - 1);

    for (size_t i = 1; i < history.size(); ++i) {
        Price prevEquity = history[i - 1].equity;
        Price currEquity = history[i].equity;

        if (prevEquity != 0) {
            // Return in basis points
            int ret = static_cast<int>(
                (static_cast<int64_t>(currEquity - prevEquity) * 10000) / prevEquity
            );
            returns.push_back(ret);
        }
    }

    return returns;
}

int PerformanceStats::calculateTotalReturn(Price startingEquity, Price endingEquity) {
    if (startingEquity == 0) {
        return 0;
    }
    // Return in basis points: (ending/starting - 1) * 10000
    return static_cast<int>(
        ((static_cast<int64_t>(endingEquity) - startingEquity) * 10000) / startingEquity
    );
}

int PerformanceStats::calculateSharpeRatio(const std::vector<int>& returns) {
    if (returns.size() < 2) {
        return 0;
    }

    // Mean return (scaled by 10000 for precision)
    int64_t sum = 0;
    for (int r : returns) {
        sum += r;
    }
    int64_t meanReturn = (sum * 10000) / static_cast<int64_t>(returns.size());

    // Variance
    int64_t sqSum = 0;
    for (int r : returns) {
        int64_t diff = static_cast<int64_t>(r) * 10000 - meanReturn;
        sqSum += (diff * diff) / 10000;
    }
    int64_t variance = sqSum / static_cast<int64_t>(returns.size() - 1);

    // Standard deviation (integer square root approximation)
    int64_t stdDev = integerSqrt(variance);
    if (stdDev == 0) {
        return 0;
    }

    // Sharpe = mean / stdDev * 100
    // Annualization factor would be sqrt(252) for daily, but we keep it simple
    return static_cast<int>((meanReturn * 100) / stdDev);
}

int PerformanceStats::calculateProfitFactor(Price grossProfit, Price grossLoss) {
    if (grossLoss == 0) {
        // No losses - infinite profit factor, cap at 9999
        return grossProfit > 0 ? 9999 : 0;
    }
    // Profit factor = |grossProfit| / |grossLoss| * 100
    // grossLoss is negative, so we negate it
    return static_cast<int>(
        (static_cast<int64_t>(grossProfit) * 100) / (-grossLoss)
    );
}

int PerformanceStats::calculateWinRate(size_t wins, size_t total) {
    if (total == 0) {
        return 0;
    }
    return static_cast<int>((wins * 10000) / total);
}

int64_t PerformanceStats::integerSqrt(int64_t n) {
    if (n < 0) return 0;
    if (n == 0) return 0;

    // Newton's method for integer square root
    int64_t x = n;
    int64_t y = (x + 1) / 2;

    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }

    return x;
}

} // namespace trading

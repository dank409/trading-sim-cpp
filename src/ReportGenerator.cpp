#include "ReportGenerator.h"
#include "RiskMetrics.h"
#include "RiskConfig.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace trading {

std::string ReportGenerator::generateSummary(const PerformanceMetrics& metrics,
                                              const Portfolio& portfolio,
                                              const std::unordered_map<AssetId, Price>& priceMap) const {
    std::ostringstream oss;
    
    oss << "=== Trading Simulation Report ===\n\n";
    
    // Performance Summary
    oss << "Performance Summary:\n";
    Price endingEquity = portfolio.getEquity(priceMap);
    // Calculate starting equity from total return: starting = ending / (1 + totalReturn/10000)
    Price startingEquity = endingEquity;
    if (metrics.totalReturn != 0) {
        int64_t denominator = 10000 + metrics.totalReturn;
        if (denominator != 0) {
            startingEquity = static_cast<Price>((static_cast<int64_t>(endingEquity) * 10000) / denominator);
        }
    }
    
    oss << "- Starting Equity: " << formatPrice(startingEquity) << "\n";
    oss << "- Ending Equity: " << formatPrice(endingEquity) << "\n";
    oss << "- Total Return: " << formatPercent(metrics.totalReturn) << "\n";
    oss << "- Max Drawdown: " << formatPercent(metrics.maxDrawdown) << "\n";
    oss << "- Sharpe Ratio: " << formatRatio(metrics.sharpeRatio) << "\n";
    
    // Trading Statistics
    oss << "\nTrading Statistics:\n";
    oss << "- Total Trades: " << metrics.totalTrades << "\n";
    oss << "- Win Rate: " << formatPercent(metrics.winRate) << "\n";
    oss << "- Profit Factor: " << formatRatio(metrics.profitFactor) << "\n";
    if (metrics.averageWin != 0 || metrics.averageLoss != 0) {
        oss << "- Average Win: " << formatPrice(metrics.averageWin) << "\n";
        oss << "- Average Loss: " << formatPrice(metrics.averageLoss) << "\n";
    }
    
    // Position Summary
    oss << "\nPosition Summary:\n";
    const auto& positions = portfolio.getPositions();
    if (positions.empty()) {
        oss << "- No open positions\n";
    } else {
        for (const auto& [assetId, position] : positions) {
            Price currentPrice = 0;
            auto priceIt = priceMap.find(assetId);
            if (priceIt != priceMap.end()) {
                currentPrice = priceIt->second;
            }
            Price positionValue = position.getQuantity() * currentPrice;
            Price unrealizedPnL = position.getUnrealizedPnL(currentPrice);
            
            oss << "- " << assetId << ": "
                << position.getQuantity() << " shares @ "
                << formatPrice(currentPrice) << " = "
                << formatPrice(positionValue);
            
            if (unrealizedPnL != 0) {
                oss << " (Unrealized PnL: " << formatPrice(unrealizedPnL) << ")";
            }
            oss << "\n";
        }
    }
    
    // Risk Metrics
    oss << "\nRisk Metrics:\n";
    RiskMetrics riskMetrics = portfolio.getRiskMetrics(priceMap, RiskConfig());
    oss << "- Total Exposure: " << formatPrice(riskMetrics.totalExposure) << "\n";
    oss << "- Margin Used: " << formatPrice(riskMetrics.marginUsed) << "\n";
    oss << "- Margin Available: " << formatPrice(riskMetrics.marginAvailable) << "\n";
    oss << "- Leverage Ratio: " << (riskMetrics.leverageRatio / 100.0) << ":1\n";
    if (riskMetrics.marginCallTriggered) {
        oss << "- Margin Call: TRIGGERED\n";
    }
    
    return oss.str();
}

std::string ReportGenerator::generateDetailedReport(const PerformanceMetrics& metrics,
                                                     const TradeLog& tradeLog,
                                                     const EquityCurve& equityCurve,
                                                     const AssetAnalytics& assetAnalytics) const {
    std::ostringstream oss;
    
    oss << "=== Detailed Trading Simulation Report ===\n\n";
    
    // Performance Summary
    oss << "Performance Summary:\n";
    Price startingEquity = equityCurve.getStartingEquity();
    Price endingEquity = equityCurve.getEndingEquity();
    
    oss << "- Starting Equity: " << formatPrice(startingEquity) << "\n";
    oss << "- Ending Equity: " << formatPrice(endingEquity) << "\n";
    oss << "- Total Return: " << formatPercent(metrics.totalReturn) << "\n";
    oss << "- Max Drawdown: " << formatPercent(metrics.maxDrawdown) << "\n";
    oss << "- Sharpe Ratio: " << formatRatio(metrics.sharpeRatio) << "\n";
    
    // Trading Statistics
    oss << "\nTrading Statistics:\n";
    oss << "- Total Trades: " << metrics.totalTrades << "\n";
    oss << "- Win Rate: " << formatPercent(metrics.winRate) << "\n";
    oss << "- Profit Factor: " << formatRatio(metrics.profitFactor) << "\n";
    if (metrics.averageWin != 0 || metrics.averageLoss != 0) {
        oss << "- Average Win: " << formatPrice(metrics.averageWin) << "\n";
        oss << "- Average Loss: " << formatPrice(metrics.averageLoss) << "\n";
    }
    
    // Additional detailed sections
    oss << "\nTrade History:\n";
    const auto& trades = tradeLog.getTrades();
    if (trades.empty()) {
        oss << "- No trades executed\n";
    } else {
        oss << "- Total Trades: " << trades.size() << "\n";
        oss << "- Largest Win: " << formatPrice(metrics.largestWin) << "\n";
        oss << "- Largest Loss: " << formatPrice(metrics.largestLoss) << "\n";
        oss << "- Max Consecutive Wins: " << metrics.maxConsecutiveWins << "\n";
        oss << "- Max Consecutive Losses: " << metrics.maxConsecutiveLosses << "\n";
    }
    
    // Equity Curve Statistics
    oss << "\nEquity Curve:\n";
    if (!equityCurve.empty()) {
        oss << "- Starting Equity: " << formatPrice(equityCurve.getStartingEquity()) << "\n";
        oss << "- Ending Equity: " << formatPrice(equityCurve.getEndingEquity()) << "\n";
        oss << "- Peak Equity: " << formatPrice(equityCurve.getPeakEquity()) << "\n";
        oss << "- Max Drawdown: " << formatPercent(equityCurve.getMaxDrawdown()) << "\n";
        oss << "- Drawdown Duration: " << equityCurve.getDrawdownDuration() << " time steps\n";
    }
    
    // Per-Asset Breakdown
    oss << "\nPer-Asset Breakdown:\n";
    auto assetStatsList = assetAnalytics.getAssetStatsByPnL();
    if (assetStatsList.empty()) {
        oss << "- No asset trading data\n";
    } else {
        for (const auto& stats : assetStatsList) {
            oss << "- " << stats.assetId << ":\n";
            oss << "  PnL: " << formatPrice(stats.totalPnL) << "\n";
            oss << "  Trades: " << stats.tradeCount << "\n";
            oss << "  Win Rate: " << formatPercent(stats.winRate) << "\n";
            oss << "  Volume: " << (stats.totalVolume / 100) << " cents\n";
        }
    }
    
    // Most/Least Profitable Assets
    AssetId mostProfitable = assetAnalytics.getMostProfitableAsset();
    AssetId leastProfitable = assetAnalytics.getLeastProfitableAsset();
    AssetId mostTraded = assetAnalytics.getMostTradedAsset();
    
    if (!mostProfitable.empty()) {
        oss << "\nHighlights:\n";
        oss << "- Most Profitable Asset: " << mostProfitable 
            << " (" << formatPrice(assetAnalytics.getAssetPnL(mostProfitable)) << ")\n";
        if (!leastProfitable.empty() && leastProfitable != mostProfitable) {
            oss << "- Least Profitable Asset: " << leastProfitable 
                << " (" << formatPrice(assetAnalytics.getAssetPnL(leastProfitable)) << ")\n";
        }
        if (!mostTraded.empty()) {
            oss << "- Most Traded Asset: " << mostTraded 
                << " (" << assetAnalytics.getAssetTradeCount(mostTraded) << " trades)\n";
        }
    }
    
    return oss.str();
}

bool ReportGenerator::exportToFile(const std::string& filename, const std::string& content) const {
    if (!ensureReportDirectory()) {
        return false;
    }
    
    std::string fullPath = reportDir_ + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

bool ReportGenerator::exportTradeLog(const TradeLog& tradeLog, const std::string& filename) const {
    if (!ensureReportDirectory()) {
        return false;
    }
    
    std::string fullPath = reportDir_ + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    // CSV header
    file << "timestamp,assetId,side,quantity,price,orderId,fillId,pnlRealized\n";
    
    // CSV rows
    const auto& trades = tradeLog.getTrades();
    for (const auto& trade : trades) {
        file << trade.timestamp << ","
             << trade.assetId << ","
             << sideToString(trade.side) << ","
             << trade.quantity << ","
             << trade.price << ","
             << trade.orderId << ","
             << trade.fillId << ","
             << trade.pnlRealized << "\n";
    }
    
    file.close();
    return true;
}

bool ReportGenerator::exportEquityCurve(const EquityCurve& equityCurve, const std::string& filename) const {
    if (!ensureReportDirectory()) {
        return false;
    }
    
    std::string fullPath = reportDir_ + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    // CSV header
    file << "timestamp,equity\n";
    
    // CSV rows
    const auto& history = equityCurve.getEquityHistory();
    for (const auto& point : history) {
        file << point.timestamp << "," << point.equity << "\n";
    }
    
    file.close();
    return true;
}

bool ReportGenerator::exportAssetAnalytics(const AssetAnalytics& assetAnalytics, const std::string& filename) const {
    if (!ensureReportDirectory()) {
        return false;
    }
    
    std::string fullPath = reportDir_ + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    // CSV header
    file << "assetId,totalPnL,tradeCount,winningTrades,losingTrades,winRate,totalVolume,grossProfit,grossLoss\n";
    
    // CSV rows
    auto assetStatsList = assetAnalytics.getAssetStatsByPnL();
    for (const auto& stats : assetStatsList) {
        file << stats.assetId << ","
             << stats.totalPnL << ","
             << stats.tradeCount << ","
             << stats.winningTrades << ","
             << stats.losingTrades << ","
             << stats.winRate << ","
             << stats.totalVolume << ","
             << stats.grossProfit << ","
             << stats.grossLoss << "\n";
    }
    
    file.close();
    return true;
}

bool ReportGenerator::ensureReportDirectory() const {
    try {
        std::filesystem::path dir(reportDir_);
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        return std::filesystem::exists(dir) && std::filesystem::is_directory(dir);
    } catch (...) {
        return false;
    }
}

std::string ReportGenerator::formatPrice(Price price) const {
    // Price is in cents, convert to dollars with 2 decimal places
    double dollars = static_cast<double>(price) / 100.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << dollars;
    return oss.str();
}

std::string ReportGenerator::formatPercent(int basisPoints) const {
    // basisPoints: 0-10000 = 0-100.00%
    double percent = static_cast<double>(basisPoints) / 100.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << percent << "%";
    return oss.str();
}

std::string ReportGenerator::formatRatio(int scaledValue) const {
    // scaledValue: 145 = 1.45
    double ratio = static_cast<double>(scaledValue) / 100.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << ratio;
    return oss.str();
}

std::string ReportGenerator::sideToString(Side side) const {
    return (side == Side::Buy) ? "BUY" : "SELL";
}

} // namespace trading

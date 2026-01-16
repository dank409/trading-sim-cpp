#pragma once

#include "Types.h"
#include "TradeLog.h"
#include "EquityCurve.h"
#include "PerformanceStats.h"
#include "AssetAnalytics.h"
#include "Portfolio.h"
#include <string>
#include <unordered_map>

namespace trading {

/// Generates formatted reports and exports data to files
/// Supports console output and CSV exports for external analysis
class ReportGenerator {
public:
    /// Default constructor
    ReportGenerator() = default;

    /// Generate a summary report for console display
    /// @param metrics Performance metrics to report
    /// @param portfolio Current portfolio state
    /// @param priceMap Current market prices for equity calculation
    /// @return Formatted summary string
    std::string generateSummary(const PerformanceMetrics& metrics,
                                const Portfolio& portfolio,
                                const std::unordered_map<AssetId, Price>& priceMap) const;

    /// Generate a detailed report with full trade analysis
    /// @param metrics Performance metrics
    /// @param tradeLog Full trade history
    /// @param equityCurve Equity progression
    /// @param assetAnalytics Per-asset breakdown
    /// @return Formatted detailed report string
    std::string generateDetailedReport(const PerformanceMetrics& metrics,
                                        const TradeLog& tradeLog,
                                        const EquityCurve& equityCurve,
                                        const AssetAnalytics& assetAnalytics) const;

    /// Export content to a file in the ./reports/ directory
    /// Creates the reports directory if it doesn't exist
    /// @param filename Name of the file (without path)
    /// @param content Content to write
    /// @return true if export succeeded
    bool exportToFile(const std::string& filename, const std::string& content) const;

    /// Export trade log to CSV format
    /// @param tradeLog Trade history to export
    /// @param filename Name of the CSV file (without path)
    /// @return true if export succeeded
    bool exportTradeLog(const TradeLog& tradeLog, const std::string& filename) const;

    /// Export equity curve to CSV format
    /// @param equityCurve Equity history to export
    /// @param filename Name of the CSV file (without path)
    /// @return true if export succeeded
    bool exportEquityCurve(const EquityCurve& equityCurve, const std::string& filename) const;

    /// Export per-asset analytics to CSV format
    /// @param assetAnalytics Asset statistics to export
    /// @param filename Name of the CSV file (without path)
    /// @return true if export succeeded
    bool exportAssetAnalytics(const AssetAnalytics& assetAnalytics, const std::string& filename) const;

    /// Set the base directory for reports (default: "./reports/")
    /// @param directory The directory path (with trailing slash)
    void setReportDirectory(const std::string& directory) { reportDir_ = directory; }

    /// Get the current report directory
    const std::string& getReportDirectory() const { return reportDir_; }

private:
    std::string reportDir_ = "./reports/";

    /// Ensure the report directory exists
    /// @return true if directory exists or was created
    bool ensureReportDirectory() const;

    /// Format a price value for display (converts cents to dollars)
    /// @param price Price in cents
    /// @return Formatted string
    std::string formatPrice(Price price) const;

    /// Format a percentage value for display
    /// @param basisPoints Value in basis points (0-10000 = 0-100%)
    /// @return Formatted string with % sign
    std::string formatPercent(int basisPoints) const;

    /// Format a ratio value for display
    /// @param scaledValue Value scaled by 100 (145 = 1.45)
    /// @return Formatted string
    std::string formatRatio(int scaledValue) const;

    /// Get side as string
    /// @param side Buy or Sell
    /// @return "BUY" or "SELL"
    std::string sideToString(Side side) const;
};

} // namespace trading

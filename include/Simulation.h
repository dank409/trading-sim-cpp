#pragma once

#include "SimulationConfig.h"
#include "Market.h"
#include "Portfolio.h"
#include "OrderBook.h"
#include "MatchingEngine.h"
#include "RiskManager.h"
#include "TradeLog.h"
#include "EquityCurve.h"
#include "Order.h"
#include "Fill.h"
#include <unordered_map>
#include <string>

namespace trading {

/// Snapshot of current simulation state
struct SimulationState {
    int currentDay;                                    // Current simulation day
    std::unordered_map<AssetId, Price> prices;        // Current market prices
    Price portfolioEquity;                             // Current portfolio equity
    Price cash;                                        // Current cash balance
    std::unordered_map<AssetId, Quantity> positions;  // Current positions
    RiskMetrics riskMetrics;                          // Current risk metrics
    
    SimulationState()
        : currentDay(0)
        , portfolioEquity(0)
        , cash(0)
    {
    }
};

/// Core simulation engine orchestrating all components
/// Manages market, portfolio, orders, risk, and analytics
class Simulation {
public:
    /// Construct a simulation with the given configuration
    /// @param config Simulation configuration
    explicit Simulation(const SimulationConfig& config);

    /// Advance the simulation by one day
    /// Updates prices, processes pending orders, records equity
    void step();

    /// Submit an order for processing
    /// Validates risk, matches against order book, updates portfolio
    /// @param order The order to submit
    /// @return RiskCheckResult indicating if order was accepted/processed
    RiskCheckResult submitOrder(Order& order);

    /// Get current simulation state snapshot
    /// @return SimulationState with current day, prices, portfolio, risk
    SimulationState getState() const;

    /// Run the full simulation to completion (for auto mode)
    /// Continues stepping until isComplete() returns true
    void run();

    /// Reset simulation to initial state
    /// Restores initial prices, clears portfolio, resets day counter
    void reset();

    /// Check if simulation is complete
    /// @return true if currentDay >= numDays
    bool isComplete() const { return currentDay_ >= config_.numDays; }

    /// Get current day
    int getCurrentDay() const { return currentDay_; }

    /// Get the simulation configuration
    const SimulationConfig& getConfig() const { return config_; }

    /// Get reference to portfolio (for external access)
    Portfolio& getPortfolio() { return portfolio_; }
    const Portfolio& getPortfolio() const { return portfolio_; }

    /// Get reference to market (for external access)
    Market& getMarket() { return market_; }
    const Market& getMarket() const { return market_; }

    /// Get reference to trade log
    TradeLog& getTradeLog() { return tradeLog_; }
    const TradeLog& getTradeLog() const { return tradeLog_; }

    /// Get reference to equity curve
    EquityCurve& getEquityCurve() { return equityCurve_; }
    const EquityCurve& getEquityCurve() const { return equityCurve_; }

    /// Get reference to risk manager
    RiskManager& getRiskManager() { return riskManager_; }
    const RiskManager& getRiskManager() const { return riskManager_; }

private:
    SimulationConfig config_;
    Market market_;
    Portfolio portfolio_;
    std::unordered_map<AssetId, OrderBook> orderBooks_;
    MatchingEngine matchingEngine_;
    RiskManager riskManager_;
    TradeLog tradeLog_;
    EquityCurve equityCurve_;
    int currentDay_;
    
    uint64_t nextFillId_ = 1;

    /// Process fills and update portfolio
    /// @param fills Vector of fills to process
    /// @param order The order that generated these fills
    void processFills(const std::vector<Fill>& fills, const Order& order);

    /// Record a trade in the trade log
    /// @param fill The fill to record
    /// @param order The order that generated the fill
    /// @param realizedPnL Realized PnL if closing a position (0 otherwise)
    void recordTrade(const Fill& fill, const Order& order, Price realizedPnL);

    /// Get current price map for all assets
    std::unordered_map<AssetId, Price> getCurrentPrices() const;
};

} // namespace trading

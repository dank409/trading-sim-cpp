#include "Simulation.h"
#include "RiskMetrics.h"
#include "Position.h"
#include <algorithm>

namespace trading {

Simulation::Simulation(const SimulationConfig& config)
    : config_(config)
    , market_(config.marketConfig, config.initialPrices)
    , portfolio_(config.initialCash)
    , matchingEngine_()
    , riskManager_(config.riskConfig)
    , currentDay_(0)
    , nextFillId_(1)
{
    // Create order books for each asset
    for (const auto& asset : config.assets) {
        orderBooks_.emplace(asset.getId(), OrderBook(asset.getId()));
    }
    
    // Record initial equity
    equityCurve_.recordEquity(0, portfolio_.getEquity(getCurrentPrices()));
}

void Simulation::step() {
    // 1. Record start-of-day equity
    equityCurve_.recordEquity(currentDay_, portfolio_.getEquity(getCurrentPrices()));

    // 2. Process any pending limit orders that now match
    // (Market orders are processed immediately, so only limit orders remain)
    // For simplicity, we check if market price crosses limit prices
    auto prices = market_.getAllPrices();
    for (auto& [assetId, book] : orderBooks_) {
        auto priceIt = prices.find(assetId);
        if (priceIt == prices.end()) continue;
        
        Price marketPrice = priceIt->second;
        
        // Check if any buy limit orders can match (limit >= market)
        // Check if any sell limit orders can match (limit <= market)
        // In a real implementation, we'd iterate through pending orders
        // For now, we rely on the matching engine to handle this during order submission
    }

    // 3. Update market prices
    market_.step();

    // 4. Check for margin calls
    if (riskManager_.checkMarginCall(portfolio_, getCurrentPrices())) {
        // Margin call triggered - in a real system, we'd liquidate positions
        // For now, we just note it in the risk metrics
    }

    currentDay_++;
}

RiskCheckResult Simulation::submitOrder(Order& order) {
    // Get current price for the asset
    Price currentPrice = market_.getPrice(order.getAssetId());
    if (currentPrice == 0) {
        return RiskCheckResult::Rejected("Asset not found in market", 0);
    }

    // Validate order with risk manager
    auto priceMap = getCurrentPrices();
    RiskCheckResult riskResult = riskManager_.validateOrder(portfolio_, order, currentPrice, priceMap);
    
    if (!riskResult.approved) {
        return riskResult;
    }

    // Get or create order book for this asset
    auto bookIt = orderBooks_.find(order.getAssetId());
    if (bookIt == orderBooks_.end()) {
        return RiskCheckResult::Rejected("Order book not found for asset", 0);
    }
    OrderBook& book = bookIt->second;

    // Process order through matching engine
    std::vector<Fill> fills = matchingEngine_.processOrder(order, book);

    // Process fills and update portfolio
    processFills(fills, order);

    // Note: MatchingEngine already adds unfilled limit orders to the book

    return RiskCheckResult::Approved();
}

void Simulation::processFills(const std::vector<Fill>& fills, const Order& order) {
    for (const auto& fill : fills) {
        // Calculate realized PnL if this is closing/reducing a position
        Price realizedPnL = 0;
        const Position* existingPos = portfolio_.getPosition(order.getAssetId());
        
        if (existingPos && existingPos->getQuantity() != 0) {
            // Determine if this is closing/reducing a position
            bool isClosingPosition = false;
            if (order.getSide() == Side::Buy && existingPos->getQuantity() < 0) {
                // Closing a short position
                isClosingPosition = true;
            } else if (order.getSide() == Side::Sell && existingPos->getQuantity() > 0) {
                // Closing a long position
                isClosingPosition = true;
            }
            
            if (isClosingPosition) {
                // Calculate realized PnL based on average cost
                Quantity closedQty = std::min(std::abs(existingPos->getQuantity()), fill.quantity);
                Price avgCost = existingPos->getAvgEntryPrice();
                
                if (order.getSide() == Side::Buy) {
                    // Closing short: profit = (sell price - buy back price) * quantity
                    realizedPnL = (avgCost - fill.price) * closedQty;
                } else {
                    // Closing long: profit = (sell price - buy price) * quantity
                    realizedPnL = (fill.price - avgCost) * closedQty;
                }
            }
        }

        // Update portfolio
        portfolio_.updatePosition(order.getAssetId(), 
                                 order.getSide() == Side::Buy ? fill.quantity : -fill.quantity,
                                 fill.price, order.getSide());

        // Record trade in log
        recordTrade(fill, order, realizedPnL);
    }
}

void Simulation::recordTrade(const Fill& fill, const Order& order, Price realizedPnL) {
    TradeRecord trade(fill.timestamp, order.getAssetId(), order.getSide(), 
                     fill.quantity, fill.price, order.getOrderId(), 
                     nextFillId_++, realizedPnL);
    
    tradeLog_.recordTrade(trade);
}

SimulationState Simulation::getState() const {
    SimulationState state;
    state.currentDay = currentDay_;
    state.prices = getCurrentPrices();
    state.portfolioEquity = portfolio_.getEquity(state.prices);
    state.cash = portfolio_.getCash();
    
    // Get positions
    const auto& positions = portfolio_.getPositions();
    for (const auto& [assetId, position] : positions) {
        state.positions[assetId] = position.getQuantity();
    }
    
    // Get risk metrics
    state.riskMetrics = riskManager_.generateRiskMetrics(portfolio_, state.prices);
    
    return state;
}

void Simulation::run() {
    while (!isComplete()) {
        step();
    }
}

void Simulation::reset() {
    currentDay_ = 0;
    market_.reset();
    portfolio_ = Portfolio(config_.initialCash);
    orderBooks_.clear();
    
    // Recreate order books
    for (const auto& asset : config_.assets) {
        orderBooks_.emplace(asset.getId(), OrderBook(asset.getId()));
    }
    
    tradeLog_.clear();
    equityCurve_.clear();
    nextFillId_ = 1;
    
    // Record initial equity
    equityCurve_.recordEquity(0, portfolio_.getEquity(getCurrentPrices()));
}

std::unordered_map<AssetId, Price> Simulation::getCurrentPrices() const {
    auto priceMap = market_.getAllPrices();
    std::unordered_map<AssetId, Price> result;
    for (const auto& [assetId, price] : priceMap) {
        result[assetId] = price;
    }
    return result;
}

} // namespace trading

#include "AutoRunner.h"
#include <algorithm>
#include <random>

namespace trading {

AutoRunner::AutoRunner(Simulation& simulation, AutoRunStrategy strategy)
    : simulation_(simulation)
    , strategy_(strategy)
    , rng_(simulation.getConfig().randomSeed)
    , firstDay_(true)
{
}

void AutoRunner::run() {
    while (!simulation_.isComplete()) {
        // Get trading decision from strategy
        SimulationState state = simulation_.getState();
        auto orderOpt = getDecision(state);
        
        if (orderOpt.has_value()) {
            Order order = orderOpt.value();
            simulation_.submitOrder(order);
        }
        
        // Advance simulation
        simulation_.step();
    }
}

std::optional<Order> AutoRunner::getDecision(const SimulationState& state) {
    switch (strategy_) {
        case AutoRunStrategy::Random:
            return getRandomDecision(state);
        case AutoRunStrategy::BuyAndHold:
            return getBuyAndHoldDecision(state);
        case AutoRunStrategy::MeanReversion:
            return getMeanReversionDecision(state);
        case AutoRunStrategy::Momentum:
            return getMomentumDecision(state);
        default:
            return std::nullopt;
    }
}

std::optional<Order> AutoRunner::getRandomDecision(const SimulationState& state) {
    if (state.prices.empty()) {
        return std::nullopt;
    }
    
    // Randomly pick an asset and side
    std::uniform_int_distribution<> assetDist(0, static_cast<int>(state.prices.size()) - 1);
    std::uniform_int_distribution<> sideDist(0, 1);  // 0 = Buy, 1 = Sell
    std::uniform_int_distribution<> quantityDist(1, 10);
    
    auto it = state.prices.begin();
    std::advance(it, assetDist(rng_));
    AssetId assetId = it->first;
    Price currentPrice = it->second;
    
    Side side = (sideDist(rng_) == 0) ? Side::Buy : Side::Sell;
    Quantity quantity = quantityDist(rng_);
    
    // Create market order
    return Order(assetId, side, quantity, state.currentDay);
}

std::optional<Order> AutoRunner::getBuyAndHoldDecision(const SimulationState& state) {
    // Only buy on day 1
    if (state.currentDay != 1 || state.prices.empty()) {
        return std::nullopt;
    }
    
    // Buy first asset
    auto it = state.prices.begin();
    AssetId assetId = it->first;
    Quantity quantity = 10;  // Fixed quantity
    
    return Order(assetId, Side::Buy, quantity, state.currentDay);
}

std::optional<Order> AutoRunner::getMeanReversionDecision(const SimulationState& state) {
    if (state.prices.empty() || previousPrices_.empty()) {
        // Store current prices for next iteration
        previousPrices_ = state.prices;
        firstDay_ = false;
        return std::nullopt;
    }
    
    // Find asset with largest price drop (buy opportunity)
    // or largest price increase (sell opportunity)
    AssetId bestBuyAsset;
    int64_t maxDrop = 0;
    
    AssetId bestSellAsset;
    int64_t maxRise = 0;
    
    for (const auto& [assetId, currentPrice] : state.prices) {
        auto prevIt = previousPrices_.find(assetId);
        if (prevIt == previousPrices_.end()) continue;
        
        Price prevPrice = prevIt->second;
        int64_t change = static_cast<int64_t>(currentPrice) - prevPrice;
        
        if (change < maxDrop) {
            maxDrop = change;
            bestBuyAsset = assetId;
        }
        
        if (change > maxRise) {
            maxRise = change;
            bestSellAsset = assetId;
        }
    }
    
    // Update previous prices
    previousPrices_ = state.prices;
    
    // If significant drop, buy
    if (maxDrop < -500) {  // Drop of at least $5
        auto priceIt = state.prices.find(bestBuyAsset);
        if (priceIt == state.prices.end()) return std::nullopt;
        Price currentPrice = priceIt->second;
        Quantity quantity = 5;
        return Order(bestBuyAsset, Side::Buy, quantity, state.currentDay);
    }
    
    // If significant rise and we have a position, sell
    if (maxRise > 500) {
        const Position* pos = simulation_.getPortfolio().getPosition(bestSellAsset);
        if (pos && pos->getQuantity() > 0) {
            Quantity quantity = std::min(5, pos->getQuantity());
            return Order(bestSellAsset, Side::Sell, quantity, state.currentDay);
        }
    }
    
    return std::nullopt;
}

std::optional<Order> AutoRunner::getMomentumDecision(const SimulationState& state) {
    if (state.prices.empty() || previousPrices_.empty()) {
        // Store current prices for next iteration
        previousPrices_ = state.prices;
        firstDay_ = false;
        return std::nullopt;
    }
    
    // Find asset with largest price increase (momentum buy)
    // or largest price decrease (momentum sell)
    AssetId bestBuyAsset;
    int64_t maxRise = 0;
    
    AssetId bestSellAsset;
    int64_t maxDrop = 0;
    
    for (const auto& [assetId, currentPrice] : state.prices) {
        auto prevIt = previousPrices_.find(assetId);
        if (prevIt == previousPrices_.end()) continue;
        
        Price prevPrice = prevIt->second;
        int64_t change = static_cast<int64_t>(currentPrice) - prevPrice;
        
        if (change > maxRise) {
            maxRise = change;
            bestBuyAsset = assetId;
        }
        
        if (change < maxDrop) {
            maxDrop = change;
            bestSellAsset = assetId;
        }
    }
    
    // Update previous prices
    previousPrices_ = state.prices;
    
    // If significant rise, buy (momentum)
    if (maxRise > 500) {  // Rise of at least $5
        auto priceIt = state.prices.find(bestBuyAsset);
        if (priceIt == state.prices.end()) return std::nullopt;
        Price currentPrice = priceIt->second;
        Quantity quantity = 5;
        return Order(bestBuyAsset, Side::Buy, quantity, state.currentDay);
    }
    
    // If significant drop and we have a long position, sell (cut losses)
    if (maxDrop < -500) {
        const Position* pos = simulation_.getPortfolio().getPosition(bestSellAsset);
        if (pos && pos->getQuantity() > 0) {
            Quantity quantity = std::min(5, pos->getQuantity());
            return Order(bestSellAsset, Side::Sell, quantity, state.currentDay);
        }
    }
    
    return std::nullopt;
}

} // namespace trading

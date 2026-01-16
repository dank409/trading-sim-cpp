#include "Portfolio.h"
#include "RiskManager.h"
#include <cstdlib> // for std::abs

namespace trading {

Portfolio::Portfolio(Price initialCash)
    : cash_(initialCash)
{
}

const Position* Portfolio::getPosition(const AssetId& assetId) const {
    auto it = positions_.find(assetId);
    if (it != positions_.end()) {
        return &it->second;
    }
    return nullptr;
}

Position* Portfolio::getPosition(const AssetId& assetId) {
    auto it = positions_.find(assetId);
    if (it != positions_.end()) {
        return &it->second;
    }
    return nullptr;
}

void Portfolio::updatePosition(const AssetId& assetId, Quantity quantity, Price price, Side side) {
    if (quantity == 0) return;

    // Determine signed quantity based on side
    // Buy = positive quantity (acquiring shares)
    // Sell = negative quantity (disposing shares)
    Quantity signedQuantity = (side == Side::Buy) ? std::abs(quantity) : -std::abs(quantity);

    // Calculate cash impact
    // Buying costs money (negative cash adjustment)
    // Selling provides money (positive cash adjustment)
    Price cashImpact = -signedQuantity * price;
    cash_ += cashImpact;

    // Find or create position
    auto it = positions_.find(assetId);
    if (it == positions_.end()) {
        // Create new position
        Position newPos(assetId);
        newPos.addToPosition(signedQuantity, price);
        positions_.emplace(assetId, std::move(newPos));
    } else {
        // Update existing position
        it->second.addToPosition(signedQuantity, price);
    }
}

Price Portfolio::getEquity(const std::unordered_map<AssetId, Price>& priceMap) const {
    Price equity = cash_;

    for (const auto& [assetId, position] : positions_) {
        auto priceIt = priceMap.find(assetId);
        if (priceIt != priceMap.end()) {
            // Position value = quantity * current price
            // Works for both long (positive) and short (negative) positions
            equity += position.getQuantity() * priceIt->second;
        }
    }

    return equity;
}

Price Portfolio::getTotalRealizedPnL() const {
    Price totalRealized = 0;
    for (const auto& [assetId, position] : positions_) {
        totalRealized += position.getRealizedPnL();
    }
    return totalRealized;
}

Price Portfolio::getTotalUnrealizedPnL(const std::unordered_map<AssetId, Price>& priceMap) const {
    Price totalUnrealized = 0;
    for (const auto& [assetId, position] : positions_) {
        auto priceIt = priceMap.find(assetId);
        if (priceIt != priceMap.end()) {
            totalUnrealized += position.getUnrealizedPnL(priceIt->second);
        }
    }
    return totalUnrealized;
}

bool Portfolio::hasOpenPositions() const {
    for (const auto& [assetId, position] : positions_) {
        if (!position.isFlat()) {
            return true;
        }
    }
    return false;
}

size_t Portfolio::getPositionCount() const {
    size_t count = 0;
    for (const auto& [assetId, position] : positions_) {
        if (!position.isFlat()) {
            ++count;
        }
    }
    return count;
}

RiskMetrics Portfolio::getRiskMetrics(const std::unordered_map<AssetId, Price>& priceMap,
                                       const RiskConfig& config) const {
    RiskManager riskManager(config);
    return riskManager.generateRiskMetrics(*this, priceMap);
}

} // namespace trading

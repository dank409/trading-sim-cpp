#include "RiskManager.h"
#include <cstdlib> // for std::abs
#include <algorithm> // for std::min

namespace trading {

RiskManager::RiskManager(const RiskConfig& config)
    : config_(config)
{
}

// ========== Pre-trade Validation Methods ==========

bool RiskManager::canOpenPosition(const Portfolio& portfolio,
                                   const AssetId& assetId,
                                   Quantity quantity,
                                   Price price,
                                   Side side,
                                   const std::unordered_map<AssetId, Price>& priceMap) const {
    // Check all risk constraints
    if (!checkPositionLimit(portfolio, assetId, quantity, side)) {
        return false;
    }

    if (!checkMarginAvailable(portfolio, quantity, price, priceMap)) {
        return false;
    }

    if (!checkExposureLimit(portfolio, assetId, quantity, price, side, priceMap)) {
        return false;
    }

    if (!checkLeverageLimit(portfolio, assetId, quantity, price, side, priceMap)) {
        return false;
    }

    return true;
}

Quantity RiskManager::getMaxOrderQuantity(const Portfolio& portfolio,
                                           const AssetId& assetId,
                                           Price price,
                                           Side side,
                                           const std::unordered_map<AssetId, Price>& priceMap) const {
    if (price <= 0) {
        return 0;
    }

    // Start with position limit
    Quantity maxFromPositionLimit = config_.maxPositionPerAsset;

    // Adjust for existing position
    const Position* pos = portfolio.getPosition(assetId);
    Quantity currentQty = pos ? pos->getQuantity() : 0;

    // For buys, we're adding to long or reducing short
    // For sells, we're adding to short or reducing long
    if (side == Side::Buy) {
        // Buying increases position (or reduces short)
        if (currentQty >= 0) {
            // Long position - check how much more we can buy
            maxFromPositionLimit = config_.maxPositionPerAsset - currentQty;
        } else {
            // Short position - can buy up to cover the short + max long
            maxFromPositionLimit = config_.maxPositionPerAsset + std::abs(currentQty);
        }
    } else {
        // Selling increases short (or reduces long)
        if (currentQty <= 0) {
            // Already short - check how much more we can sell
            maxFromPositionLimit = config_.maxPositionPerAsset - std::abs(currentQty);
        } else {
            // Long position - can sell to close + go short up to limit
            maxFromPositionLimit = config_.maxPositionPerAsset + currentQty;
        }
    }

    if (maxFromPositionLimit <= 0) {
        return 0;
    }

    // Calculate max from available margin
    int availableMargin = calculateAvailableMargin(portfolio, priceMap);
    int marginPerUnit = (price * config_.initialMarginPct) / 100;
    Quantity maxFromMargin = (marginPerUnit > 0) ? (availableMargin / marginPerUnit) : 0;

    if (maxFromMargin <= 0) {
        return 0;
    }

    // Calculate max from exposure limit
    int currentExposure = getExposure(portfolio, priceMap);
    int remainingExposure = config_.maxTotalExposure - currentExposure;
    Quantity maxFromExposure = (price > 0) ? (remainingExposure / price) : 0;

    // Adjust for position closing reducing exposure
    if (pos) {
        if ((side == Side::Buy && currentQty < 0) ||
            (side == Side::Sell && currentQty > 0)) {
            // Order reduces exposure first, then adds
            Quantity closingQty = std::abs(currentQty);
            maxFromExposure += closingQty; // Can at least close the position
        }
    }

    if (maxFromExposure <= 0) {
        return 0;
    }

    // Calculate max from leverage limit
    int equity = portfolio.getEquity(priceMap);
    if (equity <= 0) {
        return 0;
    }

    int maxExposureFromLeverage = (equity * config_.maxLeverageRatio) / 100;
    int remainingLeverageCapacity = maxExposureFromLeverage - currentExposure;
    Quantity maxFromLeverage = (price > 0) ? (remainingLeverageCapacity / price) : 0;

    // Adjust for position closing
    if (pos) {
        if ((side == Side::Buy && currentQty < 0) ||
            (side == Side::Sell && currentQty > 0)) {
            Quantity closingQty = std::abs(currentQty);
            maxFromLeverage += closingQty;
        }
    }

    if (maxFromLeverage <= 0) {
        return 0;
    }

    // Return minimum of all constraints
    Quantity result = maxFromPositionLimit;
    result = std::min(result, maxFromMargin);
    result = std::min(result, maxFromExposure);
    result = std::min(result, maxFromLeverage);

    return std::max(result, Quantity(0));
}

RiskCheckResult RiskManager::validateOrder(const Portfolio& portfolio,
                                            const Order& order,
                                            Price currentPrice,
                                            const std::unordered_map<AssetId, Price>& priceMap) const {
    Quantity quantity = order.getQuantity();
    Side side = order.getSide();
    const AssetId& assetId = order.getAssetId();

    // Check position limit
    if (!checkPositionLimit(portfolio, assetId, quantity, side)) {
        Quantity maxAllowed = getMaxOrderQuantity(portfolio, assetId, currentPrice, side, priceMap);
        return RiskCheckResult::Rejected("Position limit exceeded", maxAllowed);
    }

    // Check margin availability
    if (!checkMarginAvailable(portfolio, quantity, currentPrice, priceMap)) {
        Quantity maxAllowed = getMaxOrderQuantity(portfolio, assetId, currentPrice, side, priceMap);
        return RiskCheckResult::Rejected("Insufficient margin", maxAllowed);
    }

    // Check exposure limit
    if (!checkExposureLimit(portfolio, assetId, quantity, currentPrice, side, priceMap)) {
        Quantity maxAllowed = getMaxOrderQuantity(portfolio, assetId, currentPrice, side, priceMap);
        return RiskCheckResult::Rejected("Total exposure limit exceeded", maxAllowed);
    }

    // Check leverage limit
    if (!checkLeverageLimit(portfolio, assetId, quantity, currentPrice, side, priceMap)) {
        Quantity maxAllowed = getMaxOrderQuantity(portfolio, assetId, currentPrice, side, priceMap);
        return RiskCheckResult::Rejected("Leverage limit exceeded", maxAllowed);
    }

    return RiskCheckResult::Approved();
}

// ========== Margin Calculation Methods ==========

int RiskManager::calculateInitialMargin(Quantity quantity, Price price) const {
    return (std::abs(quantity) * price * config_.initialMarginPct) / 100;
}

int RiskManager::calculateMaintenanceMargin(Quantity quantity, Price price) const {
    return (std::abs(quantity) * price * config_.maintenanceMarginPct) / 100;
}

int RiskManager::calculateTotalMarginUsed(const Portfolio& portfolio,
                                           const std::unordered_map<AssetId, Price>& priceMap) const {
    int totalMargin = 0;

    for (const auto& [assetId, position] : portfolio.getPositions()) {
        if (position.isFlat()) {
            continue;
        }

        auto priceIt = priceMap.find(assetId);
        if (priceIt != priceMap.end()) {
            totalMargin += calculateMaintenanceMargin(position.getQuantity(), priceIt->second);
        }
    }

    return totalMargin;
}

int RiskManager::calculateAvailableMargin(const Portfolio& portfolio,
                                           const std::unordered_map<AssetId, Price>& priceMap) const {
    int equity = portfolio.getEquity(priceMap);
    int marginUsed = calculateTotalMarginUsed(portfolio, priceMap);
    return equity - marginUsed;
}

// ========== Position Monitoring Methods ==========

bool RiskManager::checkMarginCall(const Portfolio& portfolio,
                                   const std::unordered_map<AssetId, Price>& priceMap) const {
    int equity = portfolio.getEquity(priceMap);
    int totalExposure = getExposure(portfolio, priceMap);

    if (totalExposure == 0) {
        return false; // No positions, no margin call
    }

    // Required maintenance margin
    int requiredMaintenance = (totalExposure * config_.maintenanceMarginPct) / 100;

    // Margin call if equity falls below required maintenance
    return equity < requiredMaintenance;
}

int RiskManager::getMarginUtilization(const Portfolio& portfolio,
                                       const std::unordered_map<AssetId, Price>& priceMap) const {
    int equity = portfolio.getEquity(priceMap);
    if (equity <= 0) {
        return (getExposure(portfolio, priceMap) > 0) ? 100 : 0;
    }

    int marginUsed = calculateTotalMarginUsed(portfolio, priceMap);
    return (marginUsed * 100) / equity;
}

int RiskManager::getExposure(const Portfolio& portfolio,
                              const std::unordered_map<AssetId, Price>& priceMap) const {
    int totalExposure = 0;

    for (const auto& [assetId, position] : portfolio.getPositions()) {
        if (position.isFlat()) {
            continue;
        }

        auto priceIt = priceMap.find(assetId);
        if (priceIt != priceMap.end()) {
            totalExposure += std::abs(position.getQuantity()) * priceIt->second;
        }
    }

    return totalExposure;
}

int RiskManager::getLeverageRatio(const Portfolio& portfolio,
                                   const std::unordered_map<AssetId, Price>& priceMap) const {
    int equity = portfolio.getEquity(priceMap);
    if (equity <= 0) {
        return (getExposure(portfolio, priceMap) > 0) ? 10000 : 0; // Very high if has exposure but no equity
    }

    int exposure = getExposure(portfolio, priceMap);
    return (exposure * 100) / equity;
}

RiskMetrics RiskManager::generateRiskMetrics(const Portfolio& portfolio,
                                              const std::unordered_map<AssetId, Price>& priceMap) const {
    RiskMetrics metrics;

    metrics.equity = portfolio.getEquity(priceMap);
    metrics.totalExposure = getExposure(portfolio, priceMap);
    metrics.marginUsed = calculateTotalMarginUsed(portfolio, priceMap);
    metrics.marginAvailable = calculateAvailableMargin(portfolio, priceMap);
    metrics.leverageRatio = getLeverageRatio(portfolio, priceMap);
    metrics.marginCallTriggered = checkMarginCall(portfolio, priceMap);

    // Find largest position
    int largestValue = 0;
    for (const auto& [assetId, position] : portfolio.getPositions()) {
        if (position.isFlat()) {
            continue;
        }

        auto priceIt = priceMap.find(assetId);
        if (priceIt != priceMap.end()) {
            int posValue = std::abs(position.getQuantity()) * priceIt->second;
            if (posValue > largestValue) {
                largestValue = posValue;
                metrics.largestPositionAsset = assetId;
                metrics.largestPositionSize = position.getQuantity();
                metrics.largestPositionValue = posValue;
            }
        }
    }

    return metrics;
}

// ========== Private Helper Methods ==========

bool RiskManager::checkPositionLimit(const Portfolio& portfolio,
                                      const AssetId& assetId,
                                      Quantity quantity,
                                      Side side) const {
    const Position* pos = portfolio.getPosition(assetId);
    Quantity currentQty = pos ? pos->getQuantity() : 0;

    // Calculate new position after order
    Quantity orderEffect = (side == Side::Buy) ? quantity : -quantity;
    Quantity newPosition = currentQty + orderEffect;

    return std::abs(newPosition) <= config_.maxPositionPerAsset;
}

bool RiskManager::checkExposureLimit(const Portfolio& portfolio,
                                      const AssetId& assetId,
                                      Quantity quantity,
                                      Price price,
                                      Side side,
                                      const std::unordered_map<AssetId, Price>& priceMap) const {
    // Calculate current exposure
    int currentExposure = getExposure(portfolio, priceMap);

    // Calculate change in exposure for this order
    const Position* pos = portfolio.getPosition(assetId);
    Quantity currentQty = pos ? pos->getQuantity() : 0;

    Quantity orderEffect = (side == Side::Buy) ? quantity : -quantity;
    Quantity newPosition = currentQty + orderEffect;

    // Get price for this asset
    auto priceIt = priceMap.find(assetId);
    Price assetPrice = (priceIt != priceMap.end()) ? priceIt->second : price;

    int oldPosExposure = std::abs(currentQty) * assetPrice;
    int newPosExposure = std::abs(newPosition) * assetPrice;

    int newTotalExposure = currentExposure - oldPosExposure + newPosExposure;

    return newTotalExposure <= config_.maxTotalExposure;
}

bool RiskManager::checkLeverageLimit(const Portfolio& portfolio,
                                      const AssetId& assetId,
                                      Quantity quantity,
                                      Price price,
                                      Side side,
                                      const std::unordered_map<AssetId, Price>& priceMap) const {
    int equity = portfolio.getEquity(priceMap);
    if (equity <= 0) {
        return false;
    }

    // Calculate new exposure after order
    int currentExposure = getExposure(portfolio, priceMap);

    const Position* pos = portfolio.getPosition(assetId);
    Quantity currentQty = pos ? pos->getQuantity() : 0;

    Quantity orderEffect = (side == Side::Buy) ? quantity : -quantity;
    Quantity newPosition = currentQty + orderEffect;

    auto priceIt = priceMap.find(assetId);
    Price assetPrice = (priceIt != priceMap.end()) ? priceIt->second : price;

    int oldPosExposure = std::abs(currentQty) * assetPrice;
    int newPosExposure = std::abs(newPosition) * assetPrice;

    int newTotalExposure = currentExposure - oldPosExposure + newPosExposure;

    // Calculate new leverage ratio
    int newLeverageRatio = (newTotalExposure * 100) / equity;

    return newLeverageRatio <= config_.maxLeverageRatio;
}

bool RiskManager::checkMarginAvailable(const Portfolio& portfolio,
                                        Quantity quantity,
                                        Price price,
                                        const std::unordered_map<AssetId, Price>& priceMap) const {
    int availableMargin = calculateAvailableMargin(portfolio, priceMap);
    int requiredMargin = calculateInitialMargin(quantity, price);

    return requiredMargin <= availableMargin;
}

} // namespace trading

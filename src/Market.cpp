#include "Market.h"
#include <algorithm>

namespace trading {

Market::Market(const MarketConfig& config)
    : config_(config)
    , rng_(config.seed)
    , currentTimestamp_(0) {
}

Market::Market(const MarketConfig& config, const std::map<AssetId, Price>& initialPrices)
    : config_(config)
    , rng_(config.seed)
    , currentTimestamp_(0) {
    // Add assets from initial prices map
    for (const auto& [assetId, price] : initialPrices) {
        Asset asset = Asset::createSimple(assetId, assetId);
        addAsset(asset, price);
    }
}

void Market::addAsset(const Asset& asset, Price initialPrice) {
    const AssetId& id = asset.getId();

    // Check if asset already exists
    for (const auto& existingAsset : assets_) {
        if (existingAsset.getId() == id) {
            return; // Asset already exists, don't add duplicate
        }
    }

    assets_.push_back(asset);
    currentPrices_[id] = initialPrice;
    initialPrices_[id] = initialPrice;

    // Record initial price in history at timestamp 0
    priceHistory_.recordPrice(id, currentTimestamp_, initialPrice);
}

Price Market::getPrice(const AssetId& assetId) const {
    auto it = currentPrices_.find(assetId);
    if (it == currentPrices_.end()) {
        return 0;
    }
    return it->second;
}

std::map<AssetId, Price> Market::getAllPrices() const {
    return currentPrices_;
}

void Market::step() {
    // Advance timestamp
    currentTimestamp_++;

    // Generate shared market shock for correlation
    int marketShock = generateMarketShock();

    // Update each asset's price
    for (const auto& asset : assets_) {
        const AssetId& id = asset.getId();
        Price oldPrice = currentPrices_[id];

        // Per-asset independent component
        int randomComponent = generateRandomComponent();
        int assetMove = config_.drift + (config_.volatility * randomComponent) / 100;

        // Shared market shock (affects all assets)
        int shockMove = (config_.correlationStrength * marketShock) / 100;

        // Calculate new price, clamped to minimum of 1
        Price newPrice = std::max(1, oldPrice + assetMove + shockMove);

        currentPrices_[id] = newPrice;
        priceHistory_.recordPrice(id, currentTimestamp_, newPrice);
    }
}

void Market::reset() {
    // Reset RNG with original seed for reproducibility
    rng_.seed(config_.seed);

    // Reset timestamp
    currentTimestamp_ = 0;

    // Clear price history and restore initial prices
    priceHistory_.clear();

    for (const auto& asset : assets_) {
        const AssetId& id = asset.getId();
        currentPrices_[id] = initialPrices_[id];
        priceHistory_.recordPrice(id, currentTimestamp_, initialPrices_[id]);
    }
}

bool Market::hasAsset(const AssetId& assetId) const {
    return currentPrices_.find(assetId) != currentPrices_.end();
}

int Market::generateRandomComponent() {
    // Uniform distribution in range [-100, 100] as specified
    std::uniform_int_distribution<int> dist(-100, 100);
    return dist(rng_);
}

int Market::generateMarketShock() {
    // Generate a shock value that will be shared across all assets
    // Uses the same distribution as the random component
    std::uniform_int_distribution<int> dist(-100, 100);
    return dist(rng_);
}

} // namespace trading

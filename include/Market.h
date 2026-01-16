#pragma once

#include "Types.h"
#include "Asset.h"
#include "MarketConfig.h"
#include "PriceHistory.h"
#include <map>
#include <random>
#include <vector>

namespace trading {

/// Multi-asset market simulator with configurable price dynamics
/// Uses seeded randomness for reproducible simulations
class Market {
public:
    /// Construct a market with the given configuration
    /// @param config Market configuration (seed, drift, volatility, correlation)
    explicit Market(const MarketConfig& config = MarketConfig());

    /// Construct a market with config and initial prices
    /// @param config Market configuration
    /// @param initialPrices Map of asset IDs to initial prices
    Market(const MarketConfig& config, const std::map<AssetId, Price>& initialPrices);

    /// Add an asset to the market with an initial price
    /// @param asset The asset to add
    /// @param initialPrice Starting price for the asset
    void addAsset(const Asset& asset, Price initialPrice);

    /// Get the current price of an asset
    /// @param assetId The asset identifier
    /// @return Current price, or 0 if asset not found
    Price getPrice(const AssetId& assetId) const;

    /// Get all current prices
    /// @return Map of asset IDs to current prices
    std::map<AssetId, Price> getAllPrices() const;

    /// Advance the market by one time step
    /// Updates all asset prices according to the price evolution model
    void step();

    /// Reset the market to initial state
    /// Restores initial prices and reseeds RNG for reproducibility
    void reset();

    /// Get the current simulation timestamp
    /// @return Current timestamp (0-indexed)
    Timestamp getCurrentTimestamp() const { return currentTimestamp_; }

    /// Get the price history for all assets
    /// @return Reference to the price history object
    const PriceHistory& getPriceHistory() const { return priceHistory_; }

    /// Check if an asset exists in the market
    /// @param assetId The asset identifier
    /// @return true if asset exists
    bool hasAsset(const AssetId& assetId) const;

    /// Get the number of assets in the market
    /// @return Number of assets
    size_t getAssetCount() const { return assets_.size(); }

    /// Get the market configuration
    /// @return Reference to the configuration
    const MarketConfig& getConfig() const { return config_; }

private:
    /// Generate the random component for price evolution
    /// @return Random value in range [-100, 100]
    int generateRandomComponent();

    /// Generate the shared market shock
    /// @return Shock value scaled by correlation strength
    int generateMarketShock();

    MarketConfig config_;
    std::mt19937_64 rng_;

    std::vector<Asset> assets_;
    std::map<AssetId, Price> currentPrices_;
    std::map<AssetId, Price> initialPrices_;

    PriceHistory priceHistory_;
    Timestamp currentTimestamp_ = 0;
};

} // namespace trading

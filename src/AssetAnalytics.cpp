#include "AssetAnalytics.h"
#include <algorithm>

namespace trading {

void AssetAnalytics::analyze(const TradeLog& tradeLog) {
    assetStats_.clear();

    const auto& trades = tradeLog.getTrades();

    // Accumulate statistics per asset
    for (const auto& trade : trades) {
        AssetStats& stats = assetStats_[trade.assetId];
        stats.assetId = trade.assetId;
        stats.tradeCount++;
        stats.totalPnL += trade.pnlRealized;
        stats.totalVolume += static_cast<int64_t>(trade.quantity) * trade.price;

        if (trade.pnlRealized > 0) {
            stats.winningTrades++;
            stats.grossProfit += trade.pnlRealized;
        } else if (trade.pnlRealized < 0) {
            stats.losingTrades++;
            stats.grossLoss += trade.pnlRealized;
        }
    }

    // Calculate win rates
    for (auto& pair : assetStats_) {
        AssetStats& stats = pair.second;
        size_t closingTrades = stats.winningTrades + stats.losingTrades;
        if (closingTrades > 0) {
            stats.winRate = static_cast<int>((stats.winningTrades * 10000) / closingTrades);
        }
    }
}

Price AssetAnalytics::getAssetPnL(const AssetId& assetId) const {
    auto it = assetStats_.find(assetId);
    if (it != assetStats_.end()) {
        return it->second.totalPnL;
    }
    return 0;
}

size_t AssetAnalytics::getAssetTradeCount(const AssetId& assetId) const {
    auto it = assetStats_.find(assetId);
    if (it != assetStats_.end()) {
        return it->second.tradeCount;
    }
    return 0;
}

int AssetAnalytics::getAssetWinRate(const AssetId& assetId) const {
    auto it = assetStats_.find(assetId);
    if (it != assetStats_.end()) {
        return it->second.winRate;
    }
    return 0;
}

int64_t AssetAnalytics::getAssetVolume(const AssetId& assetId) const {
    auto it = assetStats_.find(assetId);
    if (it != assetStats_.end()) {
        return it->second.totalVolume;
    }
    return 0;
}

const AssetStats* AssetAnalytics::getAssetStats(const AssetId& assetId) const {
    auto it = assetStats_.find(assetId);
    if (it != assetStats_.end()) {
        return &it->second;
    }
    return nullptr;
}

AssetId AssetAnalytics::getMostProfitableAsset() const {
    if (assetStats_.empty()) {
        return "";
    }

    AssetId bestAsset;
    Price bestPnL = std::numeric_limits<Price>::min();

    for (const auto& pair : assetStats_) {
        if (pair.second.totalPnL > bestPnL) {
            bestPnL = pair.second.totalPnL;
            bestAsset = pair.first;
        }
    }

    return bestAsset;
}

AssetId AssetAnalytics::getLeastProfitableAsset() const {
    if (assetStats_.empty()) {
        return "";
    }

    AssetId worstAsset;
    Price worstPnL = std::numeric_limits<Price>::max();

    for (const auto& pair : assetStats_) {
        if (pair.second.totalPnL < worstPnL) {
            worstPnL = pair.second.totalPnL;
            worstAsset = pair.first;
        }
    }

    return worstAsset;
}

AssetId AssetAnalytics::getMostTradedAsset() const {
    if (assetStats_.empty()) {
        return "";
    }

    AssetId mostTradedAsset;
    size_t maxTrades = 0;

    for (const auto& pair : assetStats_) {
        if (pair.second.tradeCount > maxTrades) {
            maxTrades = pair.second.tradeCount;
            mostTradedAsset = pair.first;
        }
    }

    return mostTradedAsset;
}

AssetId AssetAnalytics::getHighestVolumeAsset() const {
    if (assetStats_.empty()) {
        return "";
    }

    AssetId highestVolumeAsset;
    int64_t maxVolume = 0;

    for (const auto& pair : assetStats_) {
        if (pair.second.totalVolume > maxVolume) {
            maxVolume = pair.second.totalVolume;
            highestVolumeAsset = pair.first;
        }
    }

    return highestVolumeAsset;
}

std::vector<AssetId> AssetAnalytics::getAllAssets() const {
    std::vector<AssetId> assets;
    assets.reserve(assetStats_.size());

    for (const auto& pair : assetStats_) {
        assets.push_back(pair.first);
    }

    return assets;
}

std::vector<AssetStats> AssetAnalytics::getAssetStatsByPnL() const {
    std::vector<AssetStats> stats;
    stats.reserve(assetStats_.size());

    for (const auto& pair : assetStats_) {
        stats.push_back(pair.second);
    }

    // Sort by PnL descending
    std::sort(stats.begin(), stats.end(),
              [](const AssetStats& a, const AssetStats& b) {
                  return a.totalPnL > b.totalPnL;
              });

    return stats;
}

std::vector<AssetStats> AssetAnalytics::getAssetStatsByVolume() const {
    std::vector<AssetStats> stats;
    stats.reserve(assetStats_.size());

    for (const auto& pair : assetStats_) {
        stats.push_back(pair.second);
    }

    // Sort by volume descending
    std::sort(stats.begin(), stats.end(),
              [](const AssetStats& a, const AssetStats& b) {
                  return a.totalVolume > b.totalVolume;
              });

    return stats;
}

} // namespace trading

#include "PriceHistory.h"

namespace trading {

void PriceHistory::recordPrice(const AssetId& assetId, Timestamp timestamp, Price price) {
    auto& history = priceData_[assetId];

    // Ensure vector is large enough to hold the timestamp index
    // Timestamps are 0-indexed discrete time steps
    if (timestamp >= static_cast<Timestamp>(history.size())) {
        history.resize(static_cast<size_t>(timestamp) + 1, 0);
    }

    history[static_cast<size_t>(timestamp)] = price;
}

Price PriceHistory::getPrice(const AssetId& assetId, Timestamp timestamp) const {
    auto it = priceData_.find(assetId);
    if (it == priceData_.end()) {
        return 0;
    }

    const auto& history = it->second;
    if (timestamp < 0 || timestamp >= static_cast<Timestamp>(history.size())) {
        return 0;
    }

    return history[static_cast<size_t>(timestamp)];
}

Price PriceHistory::getLatestPrice(const AssetId& assetId) const {
    auto it = priceData_.find(assetId);
    if (it == priceData_.end() || it->second.empty()) {
        return 0;
    }

    return it->second.back();
}

std::vector<Price> PriceHistory::getPriceHistory(const AssetId& assetId) const {
    auto it = priceData_.find(assetId);
    if (it == priceData_.end()) {
        return {};
    }

    return it->second;
}

bool PriceHistory::hasAsset(const AssetId& assetId) const {
    return priceData_.find(assetId) != priceData_.end();
}

void PriceHistory::clear() {
    priceData_.clear();
}

} // namespace trading

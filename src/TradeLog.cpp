#include "TradeLog.h"

namespace trading {

void TradeLog::recordTrade(const TradeRecord& trade) {
    trades_.push_back(trade);
}

std::vector<TradeRecord> TradeLog::getTradesByAsset(const AssetId& assetId) const {
    std::vector<TradeRecord> result;
    for (const auto& trade : trades_) {
        if (trade.assetId == assetId) {
            result.push_back(trade);
        }
    }
    return result;
}

int64_t TradeLog::getTotalVolume() const {
    int64_t volume = 0;
    for (const auto& trade : trades_) {
        volume += static_cast<int64_t>(trade.quantity) * trade.price;
    }
    return volume;
}

int TradeLog::getWinRate() const {
    size_t closingTrades = getClosingTradeCount();
    if (closingTrades == 0) {
        return 0;
    }

    size_t wins = getWinningTradeCount();
    // Return as basis points (0-10000 for 0-100.00%)
    return static_cast<int>((wins * 10000) / closingTrades);
}

Price TradeLog::getTotalRealizedPnL() const {
    Price total = 0;
    for (const auto& trade : trades_) {
        total += trade.pnlRealized;
    }
    return total;
}

size_t TradeLog::getClosingTradeCount() const {
    size_t count = 0;
    for (const auto& trade : trades_) {
        if (trade.hasRealizedPnL()) {
            ++count;
        }
    }
    return count;
}

size_t TradeLog::getWinningTradeCount() const {
    size_t count = 0;
    for (const auto& trade : trades_) {
        if (trade.isProfitable()) {
            ++count;
        }
    }
    return count;
}

size_t TradeLog::getLosingTradeCount() const {
    size_t count = 0;
    for (const auto& trade : trades_) {
        if (trade.isLoss()) {
            ++count;
        }
    }
    return count;
}

} // namespace trading

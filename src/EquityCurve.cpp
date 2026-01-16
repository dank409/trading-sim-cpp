#include "EquityCurve.h"

namespace trading {

void EquityCurve::recordEquity(Timestamp timestamp, Price equity) {
    history_.emplace_back(timestamp, equity);

    // Update peak if new high
    if (equity > peakEquity_) {
        peakEquity_ = equity;
        peakTimestamp_ = timestamp;
    }

    // Calculate current drawdown and update max if needed
    if (peakEquity_ > 0) {
        int currentDD = static_cast<int>(
            (static_cast<int64_t>(peakEquity_ - equity) * 10000) / peakEquity_
        );
        if (currentDD > maxDrawdown_) {
            maxDrawdown_ = currentDD;
            maxDrawdownTimestamp_ = timestamp;
        }
    }
}

Price EquityCurve::getStartingEquity() const {
    if (history_.empty()) {
        return 0;
    }
    return history_.front().equity;
}

Price EquityCurve::getEndingEquity() const {
    if (history_.empty()) {
        return 0;
    }
    return history_.back().equity;
}

Price EquityCurve::getNetChange() const {
    return getEndingEquity() - getStartingEquity();
}

Price EquityCurve::getPeakEquity() const {
    return peakEquity_;
}

int EquityCurve::getCurrentDrawdown() const {
    if (history_.empty() || peakEquity_ == 0) {
        return 0;
    }
    Price current = history_.back().equity;
    return static_cast<int>(
        (static_cast<int64_t>(peakEquity_ - current) * 10000) / peakEquity_
    );
}

int EquityCurve::getMaxDrawdown() const {
    return maxDrawdown_;
}

Timestamp EquityCurve::getMaxDrawdownTimestamp() const {
    return maxDrawdownTimestamp_;
}

int EquityCurve::getDrawdownDuration() const {
    if (history_.empty()) {
        return 0;
    }
    Price current = history_.back().equity;
    // If we're at or above peak, no drawdown duration
    if (current >= peakEquity_) {
        return 0;
    }
    // Duration is current timestamp minus peak timestamp
    return history_.back().timestamp - peakTimestamp_;
}

void EquityCurve::clear() {
    history_.clear();
    peakEquity_ = 0;
    peakTimestamp_ = 0;
    maxDrawdown_ = 0;
    maxDrawdownTimestamp_ = 0;
}

} // namespace trading

#pragma once

#include <cstdint>

namespace trading {

/// Configuration for market simulation behavior
/// All numeric values use integer arithmetic for precision
struct MarketConfig {
    /// Random seed for reproducible price paths
    uint64_t seed = 12345;

    /// Price drift in basis points per step (can be negative for bearish markets)
    int drift = 0;

    /// Volatility in basis points - controls magnitude of random moves
    int volatility = 50;

    /// Correlation strength 0-100 - how much assets move together via shared shocks
    int correlationStrength = 0;

    /// Default constructor with sensible defaults
    MarketConfig() = default;

    /// Builder-style setters for named parameter pattern
    MarketConfig& withSeed(uint64_t s) {
        seed = s;
        return *this;
    }

    MarketConfig& withDrift(int d) {
        drift = d;
        return *this;
    }

    MarketConfig& withVolatility(int v) {
        volatility = v;
        return *this;
    }

    MarketConfig& withCorrelationStrength(int c) {
        correlationStrength = c;
        return *this;
    }
};

} // namespace trading

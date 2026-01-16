#pragma once

#include "MarketConfig.h"
#include "RiskConfig.h"
#include "Asset.h"
#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace trading {

/// Mode of simulation execution
enum class SimulationMode {
    Interactive,  // User-driven menu interface
    AutoRun       // Automated strategy execution
};

/// Strategy for auto-run mode
enum class AutoRunStrategy {
    Random,        // Random buy/sell decisions
    BuyAndHold,    // Buy on day 1, hold to end
    MeanReversion, // Buy when price drops, sell when rises
    Momentum       // Buy on up days, sell on down days
};

/// Master configuration combining all simulation parameters
struct SimulationConfig {
    MarketConfig marketConfig;           // Market dynamics configuration
    RiskConfig riskConfig;               // Risk management parameters
    int numDays = 30;                    // Simulation horizon (days)
    Price initialCash = 1000000;         // Starting cash (in cents: $10,000)
    std::vector<Asset> assets;           // Assets with initial prices
    std::map<AssetId, Price> initialPrices; // Map of asset ID to initial price
    SimulationMode mode = SimulationMode::Interactive;
    AutoRunStrategy autoRunStrategy = AutoRunStrategy::Random;
    uint64_t randomSeed = 12345;         // Random seed for reproducibility

    /// Default constructor with sensible defaults
    SimulationConfig();

    /// Create a default configuration with sample assets
    /// @return SimulationConfig with AAPL, GOOG, MSFT at default prices
    static SimulationConfig createDefault();

    /// Parse command line arguments and update configuration
    /// @param argc Argument count
    /// @param argv Argument vector
    /// @return true if parsing succeeded
    bool parseCommandLine(int argc, char* argv[]);
};

} // namespace trading

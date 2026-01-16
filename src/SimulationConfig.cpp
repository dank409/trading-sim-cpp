#include "SimulationConfig.h"
#include <sstream>
#include <algorithm>
#include <cstdlib>

namespace trading {

SimulationConfig::SimulationConfig()
    : marketConfig()
    , riskConfig()
    , numDays(30)
    , initialCash(1000000)  // $10,000 in cents
    , mode(SimulationMode::Interactive)
    , autoRunStrategy(AutoRunStrategy::Random)
    , randomSeed(12345)
{
    // Default assets
    assets.push_back(Asset("AAPL", "Apple Inc."));
    assets.push_back(Asset("GOOG", "Alphabet Inc."));
    assets.push_back(Asset("MSFT", "Microsoft Corporation"));
    
    initialPrices["AAPL"] = 10000;  // $100.00
    initialPrices["GOOG"] = 280000; // $2800.00
    initialPrices["MSFT"] = 32000;  // $320.00
    
    marketConfig.seed = randomSeed;
}

SimulationConfig SimulationConfig::createDefault() {
    return SimulationConfig();
}

bool SimulationConfig::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Parse --mode=interactive|auto
        if (arg.find("--mode=") == 0) {
            std::string modeStr = arg.substr(7);
            if (modeStr == "interactive") {
                mode = SimulationMode::Interactive;
            } else if (modeStr == "auto") {
                mode = SimulationMode::AutoRun;
            } else {
                return false;
            }
        }
        // Parse --days=N
        else if (arg.find("--days=") == 0) {
            std::string daysStr = arg.substr(7);
            numDays = std::stoi(daysStr);
            if (numDays <= 0) return false;
        }
        // Parse --cash=N
        else if (arg.find("--cash=") == 0) {
            std::string cashStr = arg.substr(7);
            int cashDollars = std::stoi(cashStr);
            initialCash = cashDollars * 100;  // Convert to cents
        }
        // Parse --seed=N
        else if (arg.find("--seed=") == 0) {
            std::string seedStr = arg.substr(7);
            randomSeed = std::stoull(seedStr);
            marketConfig.seed = randomSeed;
        }
        // Parse --strategy=random|hold|meanrev|momentum
        else if (arg.find("--strategy=") == 0) {
            std::string stratStr = arg.substr(11);
            if (stratStr == "random") {
                autoRunStrategy = AutoRunStrategy::Random;
            } else if (stratStr == "hold") {
                autoRunStrategy = AutoRunStrategy::BuyAndHold;
            } else if (stratStr == "meanrev") {
                autoRunStrategy = AutoRunStrategy::MeanReversion;
            } else if (stratStr == "momentum") {
                autoRunStrategy = AutoRunStrategy::Momentum;
            } else {
                return false;
            }
        }
        // Ignore --report for now (handled by UI)
    }
    
    return true;
}

} // namespace trading

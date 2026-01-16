#pragma once

#include "Simulation.h"
#include "SimulationConfig.h"
#include "Order.h"
#include "Position.h"
#include "Types.h"
#include <random>
#include <optional>
#include <unordered_map>

namespace trading {

/// Automated strategy runner for batch simulations
/// Implements various trading strategies that execute orders automatically
class AutoRunner {
public:
    /// Construct AutoRunner with a simulation and strategy
    /// @param simulation Reference to the simulation (non-owning)
    /// @param strategy The strategy to use
    explicit AutoRunner(Simulation& simulation, AutoRunStrategy strategy);

    /// Run the simulation to completion
    /// Executes strategy decisions and advances simulation until complete
    void run();

    /// Get a trading decision from the strategy
    /// @param state Current simulation state
    /// @return Optional Order to place, or empty if no action
    std::optional<Order> getDecision(const SimulationState& state);

private:
    Simulation& simulation_;
    AutoRunStrategy strategy_;
    std::mt19937 rng_;
    
    // Track previous prices for momentum/mean reversion strategies
    std::unordered_map<AssetId, Price> previousPrices_;
    bool firstDay_ = true;

    /// Get decision for Random strategy
    std::optional<Order> getRandomDecision(const SimulationState& state);

    /// Get decision for BuyAndHold strategy
    std::optional<Order> getBuyAndHoldDecision(const SimulationState& state);

    /// Get decision for MeanReversion strategy
    std::optional<Order> getMeanReversionDecision(const SimulationState& state);

    /// Get decision for Momentum strategy
    std::optional<Order> getMomentumDecision(const SimulationState& state);
};

} // namespace trading

#pragma once

#include "Simulation.h"
#include "Order.h"
#include "OrderBook.h"
#include <string>
#include <limits>

namespace trading {

/// Interactive user interface for step-by-step simulation
/// Provides menu-driven interface for placing orders and viewing state
class InteractiveUI {
public:
    /// Construct UI with a simulation instance
    /// @param simulation Reference to the simulation (non-owning)
    explicit InteractiveUI(Simulation& simulation);

    /// Run the interactive interface
    /// Displays menu and processes user input until exit
    void run();

private:
    Simulation& simulation_;

    /// Display the main menu and current state
    void displayMenu() const;

    /// Handle menu choice
    /// @param choice User's menu choice (1-8)
    void handleMenuChoice(int choice);

    /// Place an order through the UI
    void placeOrder();

    /// Display portfolio details
    void displayPortfolio() const;

    /// Display order book for an asset
    void displayOrderBook() const;

    /// Display risk metrics
    void displayRiskMetrics() const;

    /// Advance to next day (hold position)
    void nextDay();

    /// Fast forward to end of simulation
    void fastForward();

    /// Generate and display report
    void generateReport() const;

    /// Get validated integer input from user
    /// @param min Minimum allowed value
    /// @param max Maximum allowed value
    /// @param prompt Prompt to display
    /// @return Validated input value
    int getValidatedInput(int min, int max, const std::string& prompt) const;

    /// Get string input from user
    /// @param prompt Prompt to display
    /// @return User's input string
    std::string getStringInput(const std::string& prompt) const;

    /// Get yes/no input from user
    /// @param prompt Prompt to display
    /// @return true if yes, false if no
    bool getYesNoInput(const std::string& prompt) const;

    /// Select an asset from available assets
    /// @return Asset ID selected by user, or empty string if cancelled
    std::string selectAsset() const;
};

} // namespace trading

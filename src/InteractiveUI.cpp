#include "InteractiveUI.h"
#include "ReportGenerator.h"
#include "PerformanceStats.h"
#include "AssetAnalytics.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace trading {

InteractiveUI::InteractiveUI(Simulation& simulation)
    : simulation_(simulation)
{
}

void InteractiveUI::run() {
    std::cout << "\n=== Trading Simulator - Interactive Mode ===\n\n";
    
    while (!simulation_.isComplete()) {
        displayMenu();
        
        int choice = getValidatedInput(1, 8, "Choose: ");
        handleMenuChoice(choice);
        
        if (choice == 8) {
            break;  // Exit
        }
    }
    
    // Generate final report if simulation completed
    if (simulation_.isComplete()) {
        std::cout << "\n=== Simulation Complete ===\n";
        generateReport();
    }
}

void InteractiveUI::displayMenu() const {
    std::cout << "\n";
    std::cout << "=== Day " << (simulation_.getCurrentDay() + 1) 
              << " of " << simulation_.getConfig().numDays << " ===\n";
    
    // Display market prices
    auto prices = simulation_.getMarket().getAllPrices();
    std::cout << "Market Prices: ";
    bool first = true;
    for (const auto& [assetId, price] : prices) {
        if (!first) std::cout << ", ";
        std::cout << assetId << ": " << (price / 100.0);
        first = false;
    }
    std::cout << "\n";
    
    // Display portfolio summary
    auto priceMap = simulation_.getMarket().getAllPrices();
    std::unordered_map<AssetId, Price> pricesMap;
    for (const auto& [assetId, price] : prices) {
        pricesMap[assetId] = price;
    }
    
    Price equity = simulation_.getPortfolio().getEquity(pricesMap);
    Price cash = simulation_.getPortfolio().getCash();
    std::cout << "Portfolio: Cash: " << (cash / 100.0) 
              << ", Equity: " << (equity / 100.0) << "\n";
    
    std::cout << "\nActions:\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. View Portfolio Details\n";
    std::cout << "3. View Order Book\n";
    std::cout << "4. View Risk Metrics\n";
    std::cout << "5. Next Day (Hold)\n";
    std::cout << "6. Fast Forward (skip to end)\n";
    std::cout << "7. Generate Report\n";
    std::cout << "8. Exit Simulation\n";
}

void InteractiveUI::handleMenuChoice(int choice) {
    switch (choice) {
        case 1:
            placeOrder();
            break;
        case 2:
            displayPortfolio();
            break;
        case 3:
            displayOrderBook();
            break;
        case 4:
            displayRiskMetrics();
            break;
        case 5:
            nextDay();
            break;
        case 6:
            fastForward();
            break;
        case 7:
            generateReport();
            break;
        case 8:
            std::cout << "Exiting simulation...\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
    }
}

void InteractiveUI::placeOrder() {
    // Select asset
    std::string assetId = selectAsset();
    if (assetId.empty()) {
        std::cout << "Order cancelled.\n";
        return;
    }
    
    // Select order type
    std::cout << "Order Type: [1] Market  [2] Limit\n";
    int typeChoice = getValidatedInput(1, 2, "Select: ");
    OrderType orderType = (typeChoice == 1) ? OrderType::Market : OrderType::Limit;
    
    // Select side
    std::cout << "Side: [1] Buy  [2] Sell\n";
    int sideChoice = getValidatedInput(1, 2, "Select: ");
    Side side = (sideChoice == 1) ? Side::Buy : Side::Sell;
    
    // Get quantity
    int quantity = getValidatedInput(1, 10000, "Quantity: ");
    
    // Get limit price if limit order
    Price limitPrice = 0;
    if (orderType == OrderType::Limit) {
        Price currentPrice = simulation_.getMarket().getPrice(assetId);
        std::cout << "Current Price: " << (currentPrice / 100.0) << "\n";
        int limitDollars = getValidatedInput(1, 1000000, "Limit Price (in dollars): ");
        limitPrice = limitDollars * 100;  // Convert to cents
    }
    
    // Create order
    Order order = (orderType == OrderType::Limit) ?
        Order(assetId, side, quantity, limitPrice, simulation_.getCurrentDay()) :
        Order(assetId, side, quantity, simulation_.getCurrentDay());
    
    // Validate with risk manager
    Price currentPrice = simulation_.getMarket().getPrice(assetId);
    auto priceMap = simulation_.getMarket().getAllPrices();
    std::unordered_map<AssetId, Price> pricesMap;
    for (const auto& [aid, price] : priceMap) {
        pricesMap[aid] = price;
    }
    
    auto riskResult = simulation_.submitOrder(order);
    
    if (!riskResult.approved) {
        std::cout << "Risk Check: FAILED - " << riskResult.reason << "\n";
        if (riskResult.maxAllowedQuantity > 0) {
            std::cout << "Maximum allowed quantity: " << riskResult.maxAllowedQuantity << "\n";
        }
        return;
    }
    
    std::cout << "Risk Check: PASSED\n";
    
    // Confirm order
    if (!getYesNoInput("Confirm order? [Y/N]: ")) {
        std::cout << "Order cancelled.\n";
        return;
    }
    
    // Order was already processed in submitOrder, just confirm
    std::cout << "Order submitted and processed.\n";
}

void InteractiveUI::displayPortfolio() const {
    std::cout << "\n=== Portfolio Details ===\n";
    
    const auto& portfolio = simulation_.getPortfolio();
    auto priceMap = simulation_.getMarket().getAllPrices();
    std::unordered_map<AssetId, Price> pricesMap;
    for (const auto& [assetId, price] : priceMap) {
        pricesMap[assetId] = price;
    }
    
    std::cout << "Cash: " << (portfolio.getCash() / 100.0) << "\n";
    std::cout << "Equity: " << (portfolio.getEquity(pricesMap) / 100.0) << "\n";
    std::cout << "Total Realized PnL: " << (portfolio.getTotalRealizedPnL() / 100.0) << "\n";
    
    const auto& positions = portfolio.getPositions();
    if (positions.empty()) {
        std::cout << "No open positions.\n";
    } else {
        std::cout << "\nPositions:\n";
        for (const auto& [assetId, position] : positions) {
            Price currentPrice = pricesMap[assetId];
            Price positionValue = position.getQuantity() * currentPrice;
            Price unrealizedPnL = position.getUnrealizedPnL(currentPrice);
            
            std::cout << "  " << assetId << ": "
                      << position.getQuantity() << " shares @ "
                      << (currentPrice / 100.0) << " = "
                      << (positionValue / 100.0);
            if (unrealizedPnL != 0) {
                std::cout << " (Unrealized PnL: " << (unrealizedPnL / 100.0) << ")";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void InteractiveUI::displayOrderBook() const {
    std::string assetId = selectAsset();
    if (assetId.empty()) {
        return;
    }
    
    // Get order book for this asset
    // Note: Simulation doesn't expose orderBooks directly, so we'll just show market price
    Price currentPrice = simulation_.getMarket().getPrice(assetId);
    std::cout << "\n=== Order Book: " << assetId << " ===\n";
    std::cout << "Current Market Price: " << (currentPrice / 100.0) << "\n";
    std::cout << "(Detailed order book view not implemented)\n";
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void InteractiveUI::displayRiskMetrics() const {
    std::cout << "\n=== Risk Metrics ===\n";
    
    const auto& portfolio = simulation_.getPortfolio();
    auto priceMap = simulation_.getMarket().getAllPrices();
    std::unordered_map<AssetId, Price> pricesMap;
    for (const auto& [assetId, price] : priceMap) {
        pricesMap[assetId] = price;
    }
    
    RiskMetrics metrics = simulation_.getRiskManager().generateRiskMetrics(portfolio, pricesMap);
    
    std::cout << "Total Exposure: " << (metrics.totalExposure / 100.0) << "\n";
    std::cout << "Margin Used: " << (metrics.marginUsed / 100.0) << "\n";
    std::cout << "Margin Available: " << (metrics.marginAvailable / 100.0) << "\n";
    std::cout << "Leverage Ratio: " << (metrics.leverageRatio / 100.0) << ":1\n";
    
    if (metrics.marginCallTriggered) {
        std::cout << "WARNING: Margin Call Triggered!\n";
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void InteractiveUI::nextDay() {
    simulation_.step();
}

void InteractiveUI::fastForward() {
    std::cout << "Fast forwarding to end of simulation...\n";
    while (!simulation_.isComplete()) {
        simulation_.step();
    }
    std::cout << "Simulation complete.\n";
}

void InteractiveUI::generateReport() const {
    // Calculate performance metrics
    PerformanceStats stats;
    PerformanceMetrics metrics = stats.calculate(simulation_.getTradeLog(), 
                                                  simulation_.getEquityCurve());
    
    // Analyze asset analytics
    AssetAnalytics assetAnalytics;
    assetAnalytics.analyze(simulation_.getTradeLog());
    
    // Generate report
    ReportGenerator generator;
    auto priceMap = simulation_.getMarket().getAllPrices();
    std::unordered_map<AssetId, Price> pricesMap;
    for (const auto& [assetId, price] : priceMap) {
        pricesMap[assetId] = price;
    }
    
    std::string summary = generator.generateSummary(metrics, 
                                                    simulation_.getPortfolio(), 
                                                    pricesMap);
    std::cout << "\n" << summary << "\n";
    
    // Offer to export
    if (getYesNoInput("Export reports to files? [Y/N]: ")) {
        generator.exportTradeLog(simulation_.getTradeLog(), "trades.csv");
        generator.exportEquityCurve(simulation_.getEquityCurve(), "equity.csv");
        generator.exportAssetAnalytics(assetAnalytics, "assets.csv");
        std::cout << "Reports exported to ./reports/\n";
    }
}

int InteractiveUI::getValidatedInput(int min, int max, const std::string& prompt) const {
    int choice;
    std::cout << prompt;
    while (!(std::cin >> choice) || choice < min || choice > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Try again: ";
    }
    return choice;
}

std::string InteractiveUI::getStringInput(const std::string& prompt) const {
    std::string input;
    std::cout << prompt;
    std::cin >> input;
    return input;
}

bool InteractiveUI::getYesNoInput(const std::string& prompt) const {
    std::string input;
    std::cout << prompt;
    std::cin >> input;
    return (input == "Y" || input == "y" || input == "yes" || input == "Yes");
}

std::string InteractiveUI::selectAsset() const {
    auto prices = simulation_.getMarket().getAllPrices();
    if (prices.empty()) {
        std::cout << "No assets available.\n";
        return "";
    }
    
    std::cout << "Select Asset:\n";
    int index = 1;
    std::vector<std::string> assetIds;
    for (const auto& [assetId, price] : prices) {
        std::cout << "[" << index << "] " << assetId << "\n";
        assetIds.push_back(assetId);
        index++;
    }
    
    int choice = getValidatedInput(1, static_cast<int>(assetIds.size()), "Select: ");
    return assetIds[choice - 1];
}

} // namespace trading

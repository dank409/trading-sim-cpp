#include "SimulationConfig.h"
#include "Simulation.h"
#include "InteractiveUI.h"
#include "AutoRunner.h"
#include "ReportGenerator.h"
#include "PerformanceStats.h"
#include "AssetAnalytics.h"
#include <iostream>

using namespace trading;

int main(int argc, char* argv[]) {
    // Parse command line arguments or use defaults
    SimulationConfig config = SimulationConfig::createDefault();
    
    if (!config.parseCommandLine(argc, argv)) {
        std::cerr << "Error parsing command line arguments.\n";
        std::cerr << "Usage: trading_sim [--mode=interactive|auto] [--days=N] [--cash=N] [--seed=N] [--strategy=random|hold|meanrev|momentum]\n";
        return 1;
    }
    
    // Create simulation
    Simulation simulation(config);
    
    // Run in appropriate mode
    if (config.mode == SimulationMode::Interactive) {
        InteractiveUI ui(simulation);
        ui.run();
    } else {
        // Auto-run mode
        AutoRunner runner(simulation, config.autoRunStrategy);
        runner.run();
        
        // Generate report at end
        PerformanceStats stats;
        PerformanceMetrics metrics = stats.calculate(simulation.getTradeLog(), 
                                                      simulation.getEquityCurve());
        
        AssetAnalytics assetAnalytics;
        assetAnalytics.analyze(simulation.getTradeLog());
        
        ReportGenerator generator;
        auto priceMap = simulation.getMarket().getAllPrices();
        std::unordered_map<AssetId, Price> pricesMap;
        for (const auto& [assetId, price] : priceMap) {
            pricesMap[assetId] = price;
        }
        
        std::string summary = generator.generateSummary(metrics, 
                                                        simulation.getPortfolio(), 
                                                        pricesMap);
        std::cout << "\n=== Simulation Complete ===\n\n";
        std::cout << summary << "\n";
        
        // Export reports
        generator.exportTradeLog(simulation.getTradeLog(), "trades.csv");
        generator.exportEquityCurve(simulation.getEquityCurve(), "equity.csv");
        generator.exportAssetAnalytics(assetAnalytics, "assets.csv");
        std::cout << "Reports exported to ./reports/\n";
    }
    
    return 0;
}

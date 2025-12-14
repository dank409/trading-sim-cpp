#include <iostream>
#include <random>
#include <string>

int main() {
    const int entryPrice = 100;

    std::cout << "=== One-Day Trading Simulator (V1) ===\n";
    std::cout << "Current price: " << entryPrice << "\n";
    std::cout << "Choose action: (B)uy or (S)ell: ";

    char action;
    std::cin >> action;

    std::cout << "Choose quantity (e.g., 1, 5, 10): ";
    int qty;
    std::cin >> qty;

    // input check
    if (!(action == 'B' || action == 'b' || action == 'S' || action == 's')) {
        std::cout << "Invalid action. Use B or S.\n";
        return 0;
    }
    if (qty <= 0) {
        std::cout << "Quantity must be positive.\n";
        return 0;
    }

    // Randomly choose exit price: 99 or 101 (50/50)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> coinFlip(0, 1);

    int exitPrice = (coinFlip(gen) == 0) ? 99 : 101;

    // Calculate PnL
    int pnl = 0;
    bool isBuy = (action == 'B' || action == 'b');

    if (isBuy) {
        pnl = (exitPrice - entryPrice) * qty;
    } else {
        pnl = (entryPrice - exitPrice) * qty;
    }

    // Print results
    std::cout << "\n--- Result ---\n";
    std::cout << "You " << (isBuy ? "BOUGHT" : "SOLD") << " " << qty
              << " @ " << entryPrice << "\n";
    std::cout << "End-of-day price: " << exitPrice << "\n";
    std::cout << "PnL: " << pnl << "\n";

    if (pnl > 0) std::cout << "You made money.\n";
    else if (pnl < 0) std::cout << "You lost money.\n";
    else std::cout << "You broke even.\n";

    return 0;
}

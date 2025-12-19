#include <iostream>
#include <random>
#include <string>

// Portfolio class to track cash, position, and equity
class Portfolio {
private:
    int cash;
    int position;  // positive = long, negative = short
    int avgEntryPrice;  // average entry price for current position
    int realizedPnL;  // cumulative realized PnL

public:
    Portfolio(int initialCash) : cash(initialCash), position(0), avgEntryPrice(0), realizedPnL(0) {}

    int getCash() const { return cash; }
    int getPosition() const { return position; }
    int getAvgEntryPrice() const { return avgEntryPrice; }
    int getRealizedPnL() const { return realizedPnL; }

    // Calculate total equity (cash + position value at current price)
    int getEquity(int currentPrice) const {
        return cash + (position * currentPrice);
    }

    // Calculate unrealized PnL at current price
    int getUnrealizedPnL(int currentPrice) const {
        if (position == 0) return 0;
        return (currentPrice - avgEntryPrice) * position;
    }

    // Buy shares
    bool buy(int quantity, int price) {
        int cost = quantity * price;
        if (cost > cash) {
            return false;  // insufficient funds
        }

        if (position >= 0) {
            // Adding to long or opening long
            int totalCost = (position * avgEntryPrice) + cost;
            position += quantity;
            avgEntryPrice = (position > 0) ? totalCost / position : 0;
        } else {
            // Reducing short position
            if (quantity <= -position) {
                // Closing part or all of short
                int closedQty = quantity;
                int closedPnL = (avgEntryPrice - price) * closedQty;
                realizedPnL += closedPnL;
                position += quantity;  // position becomes less negative
                if (position == 0) {
                    avgEntryPrice = 0;
                }
            } else {
                // Closing short and opening long
                int closedPnL = (avgEntryPrice - price) * (-position);
                realizedPnL += closedPnL;
                int remainingQty = quantity + position;  // positive
                avgEntryPrice = price;
                position = remainingQty;
            }
        }

        cash -= cost;
        return true;
    }

    // Sell shares
    bool sell(int quantity, int price) {
        if (quantity <= 0) return false;

        if (position <= 0) {
            // Adding to short or opening short
            int proceeds = quantity * price;
            if (position < 0) {
                // Adding to short
                int totalProceeds = (-position * avgEntryPrice) + proceeds;
                position -= quantity;
                avgEntryPrice = (position < 0) ? totalProceeds / (-position) : 0;
            } else {
                // Opening short
                avgEntryPrice = price;
                position = -quantity;
            }
            cash += proceeds;
        } else {
            // Reducing long position
            if (quantity <= position) {
                // Closing part or all of long
                int closedQty = quantity;
                int closedPnL = (price - avgEntryPrice) * closedQty;
                realizedPnL += closedPnL;
                position -= quantity;
                if (position == 0) {
                    avgEntryPrice = 0;
                }
                cash += quantity * price;
            } else {
                // Closing long and opening short
                int closedPnL = (price - avgEntryPrice) * position;
                realizedPnL += closedPnL;
                int remainingQty = quantity - position;  // positive
                avgEntryPrice = price;
                position = -remainingQty;
                cash += quantity * price;
            }
        }

        return true;
    }
};

// Market class to handle price evolution
class Market {
private:
    int currentPrice;
    std::mt19937 gen;
    std::uniform_int_distribution<int> priceMove;

public:
    Market(int initialPrice) : currentPrice(initialPrice), gen(std::random_device{}()), priceMove(-1, 1) {}

    int getPrice() const { return currentPrice; }

    // Update price with simple random walk: -1, 0, or +1
    void updatePrice() {
        currentPrice += priceMove(gen);
        // Keep price positive
        if (currentPrice < 1) currentPrice = 1;
    }
};

int main() {
    const int initialPrice = 100;
    const int initialCash = 10000;
    const int numDays = 5;

    std::cout << "=== Multi-Day Trading Simulator (V2) ===\n";
    std::cout << "Starting cash: " << initialCash << "\n";
    std::cout << "Starting price: " << initialPrice << "\n";
    std::cout << "Simulation will run for " << numDays << " days\n\n";

    Portfolio portfolio(initialCash);
    Market market(initialPrice);

    // Main simulation loop
    for (int day = 1; day <= numDays; day++) {
        std::cout << "--- Day " << day << " ---\n";
        std::cout << "Current price: " << market.getPrice() << "\n";
        std::cout << "Cash: " << portfolio.getCash() << "\n";
        std::cout << "Position: " << portfolio.getPosition() << " shares";
        if (portfolio.getPosition() != 0) {
            std::cout << " @ avg " << portfolio.getAvgEntryPrice();
        }
        std::cout << "\n";
        std::cout << "Equity: " << portfolio.getEquity(market.getPrice()) << "\n";
        std::cout << "Realized PnL: " << portfolio.getRealizedPnL() << "\n";
        std::cout << "Unrealized PnL: " << portfolio.getUnrealizedPnL(market.getPrice()) << "\n";
        std::cout << "\n";

        // Trading decision
        std::cout << "Choose action: (B)uy, (S)ell, or (H)old: ";
        char action;
        std::cin >> action;

        if (action == 'B' || action == 'b' || action == 'S' || action == 's') {
            std::cout << "Choose quantity (e.g., 1, 5, 10): ";
            int qty;
            std::cin >> qty;

            if (qty <= 0) {
                std::cout << "Quantity must be positive. Skipping trade.\n\n";
            } else {
                bool success = false;
                if (action == 'B' || action == 'b') {
                    success = portfolio.buy(qty, market.getPrice());
                    if (success) {
                        std::cout << "Bought " << qty << " @ " << market.getPrice() << "\n";
                    } else {
                        std::cout << "Insufficient funds to buy " << qty << " shares.\n";
                    }
                } else {
                    success = portfolio.sell(qty, market.getPrice());
                    if (success) {
                        std::cout << "Sold " << qty << " @ " << market.getPrice() << "\n";
                    }
                }
                std::cout << "\n";
            }
        } else if (action == 'H' || action == 'h') {
            std::cout << "Holding position.\n\n";
        } else {
            std::cout << "Invalid action. Holding position.\n\n";
        }

        // Update price for next day
        if (day < numDays) {
            market.updatePrice();
        }
    }

    // Final summary
    std::cout << "=== Final Summary ===\n";
    std::cout << "Final price: " << market.getPrice() << "\n";
    std::cout << "Final cash: " << portfolio.getCash() << "\n";
    std::cout << "Final position: " << portfolio.getPosition() << " shares";
    if (portfolio.getPosition() != 0) {
        std::cout << " @ avg " << portfolio.getAvgEntryPrice();
    }
    std::cout << "\n";
    std::cout << "Final equity: " << portfolio.getEquity(market.getPrice()) << "\n";
    std::cout << "Total realized PnL: " << portfolio.getRealizedPnL() << "\n";
    std::cout << "Final unrealized PnL: " << portfolio.getUnrealizedPnL(market.getPrice()) << "\n";
    
    int totalPnL = portfolio.getRealizedPnL() + portfolio.getUnrealizedPnL(market.getPrice());
    int netChange = portfolio.getEquity(market.getPrice()) - initialCash;
    std::cout << "Total PnL: " << totalPnL << "\n";
    std::cout << "Net change: " << netChange << " (from initial " << initialCash << ")\n";

    if (netChange > 0) std::cout << "You made money.\n";
    else if (netChange < 0) std::cout << "You lost money.\n";
    else std::cout << "You broke even.\n";

    return 0;
}

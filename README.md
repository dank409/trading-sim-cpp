# trading-sim-cpp-v1
A minimal C++ trading simulator that models a single buy/sell decision and simulates PnL from a one-day price movement.

## How it works
- Initial price is fixed at 100
- User chooses to buy or sell at that price
- User selects a quantity
- The market price moves to either 99 or 101
- Profit or loss is calculated based on the position

## Build & Run
```bash
g++ -std=c++17 -Wall main.cpp -o sim
./sim

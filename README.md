# trading-sim-cpp-v2
A minimal C++ trading simulator that models trading decisions, portfolio state, and profit & loss (PnL) over multiple simulated days.

Version history and notable changes are documented in CHANGELOG.md.

## How it works
- Start with a fixed amount of cash and an initial price
- Each day: choose Buy, Sell, or Hold
- Trades update portfolio cash, position, and average entry price
- Market price evolves via a simple random walk each day
- Equity is marked to market daily
- Realized and unrealized PnL are tracked throughout the run

## Example Run
```text
=== Multi-Day Trading Simulator (V2) ===
Starting cash: 10000
Starting price: 100
Simulation will run for 5 days

--- Day 1 ---
Current price: 100
Cash: 10000
Position: 0 shares
Equity: 10000
Realized PnL: 0
Unrealized PnL: 0

Choose action: (B)uy, (S)ell, or (H)old: B
Choose quantity (e.g., 1, 5, 10): 10
Bought 10 @ 100
```

## Planned Features (WIP)
- Multi-asset market simulation
- Deterministic / configurable market dynamics (seeded runs, drift/vol controls)
- Order objects and order-based execution
- Basic order book per asset (limit orders + matching)
- Risk constraints / realism improvements (e.g., margin rules for shorts)

## Build & Run

### **Prerequisites:**  
A C++ compiler (GCC or Clang) supporting C++17 or later.

### Compile
```bash
g++ -std=c++17 -Wall main.cpp -o sim
```

### Run
```bash
./sim

# trading-sim-cpp-v1
A minimal C++ trading simulator that models a single buy/sell decision and simulates PnL from a one-day price movement.

## How it works
- **Entry:** Initial price is fixed at 100.
- **Action:** User chooses to Buy (long) or Sell (short) and selects a quantity.
- **Simulation:** The market price randomly moves to either 99 or 101 (50/50 chance).
- **Result:** Profit or loss (PnL) is calculated based on the position direction.

## Example Run
```text
=== One-Day Trading Simulator (V1) ===
Current price: 100
Choose action: (B)uy or (S)ell: B
Choose quantity (e.g., 1, 5, 10): 10

--- Result ---
You BOUGHT 10 @ 100
End-of-day price: 101
PnL: 10
You made money.
```

## Planned Features (WIP)
- Portfolio, position, and cash tracking (persistent state)
- Time-based market simulation with daily price evolution
- Support for multiple assets
- Interactive and passive simulation modes
- Order-based execution via a basic order book

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

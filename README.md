# Trading Simulator

A comprehensive C++ trading simulator that models multi-asset markets, order books, risk management, and portfolio analytics. This simulator provides both interactive and automated trading modes with full trade history, equity curves, and performance reporting.

## Features

- **Multi-Asset Support**: Trade multiple assets simultaneously with configurable price dynamics
- **Order Book System**: Price-time priority order matching with limit and market orders
- **Risk Management**: Margin requirements, position limits, leverage constraints, and margin call detection
- **Portfolio Analytics**: Real-time equity tracking, drawdown analysis, Sharpe ratio calculation, and trade logging
- **Interactive Mode**: Menu-driven interface for step-by-step simulation control
- **Auto-Run Mode**: Automated strategies (random, buy-and-hold, mean reversion, momentum) with reproducible results
- **Performance Reporting**: Comprehensive reports with per-asset breakdowns and CSV exports

## Architecture

The simulator is built with a modular design separating concerns:

- **Market Simulation**: Multi-asset price evolution with configurable drift, volatility, and correlation
- **Order Processing**: Matching engine with FIFO execution and price-time priority
- **Portfolio Management**: Position tracking with average cost accounting and PnL calculation
- **Risk Management**: Pre-trade validation, margin calculations, and position monitoring
- **Analytics**: Trade logs, equity curves, performance metrics, and reporting

## Quick Start

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.16 or later

### Build

```bash
# Configure and build
cmake -B build
cmake --build build
```

The executable will be created at `./build/trading_sim`.

### Run

**Interactive Mode** (default):
```bash
./build/trading_sim
```

**Auto-Run Mode**:
```bash
./build/trading_sim --mode=auto --days=100 --strategy=random
```

**With Custom Seed** (for reproducible results):
```bash
./build/trading_sim --mode=auto --days=50 --seed=12345
```

## Usage

### Interactive Mode

Interactive mode provides a menu-driven interface for controlling the simulation step-by-step:

```
=== Day 1 of 30 ===
Market Prices: AAPL: 100, GOOG: 2800, MSFT: 320
Portfolio: Cash: 10000, Equity: 10000

Actions:
1. Place Order
2. View Portfolio Details
3. View Order Book
4. View Risk Metrics
5. Next Day (Hold)
6. Fast Forward (skip to end)
7. Generate Report
8. Exit Simulation

Choose: _
```

When placing an order, the system will:
1. Validate the order against risk limits
2. Match the order against the order book
3. Update your portfolio with fills
4. Record trades in the trade log

### Auto-Run Mode

Auto-run mode executes a predefined strategy automatically until the simulation completes.

**Command Line Options:**
- `--mode=interactive|auto` - Simulation mode (default: interactive)
- `--days=N` - Number of days to simulate (default: 30)
- `--cash=N` - Starting cash in dollars (default: 10000)
- `--seed=N` - Random seed for reproducible results (default: 12345)
- `--strategy=random|hold|meanrev|momentum` - Auto-run strategy (default: random)

**Strategies:**
- `random` - Random buy/sell decisions each day
- `hold` - Buy on day 1, hold to end
- `meanrev` - Buy when price drops, sell when price rises
- `momentum` - Buy on up days, sell on down days

**Example:**
```bash
./build/trading_sim --mode=auto --days=100 --strategy=momentum --seed=42
```

### Configuration

Default assets are AAPL ($100), GOOG ($2800), and MSFT ($320). The simulation uses integer arithmetic with prices in cents for precision.

Key configuration parameters:
- **Initial Cash**: Starting portfolio value (default: $10,000)
- **Simulation Days**: Number of time steps (default: 30)
- **Market Seed**: Random seed for price generation (default: 12345)
- **Market Drift**: Price trend in basis points per step (default: 0)
- **Market Volatility**: Price volatility in basis points (default: 50)

## Example Session

```bash
$ ./build/trading_sim --mode=interactive --days=10 --seed=42

=== Day 1 of 10 ===
Market Prices: AAPL: 100, GOOG: 2800, MSFT: 320
Portfolio: Cash: 10000, Equity: 10000

Choose: 1 (Place Order)
Select Asset: [1] AAPL  [2] GOOG  [3] MSFT
Select: 1
Order Type: [1] Market  [2] Limit
Select: 1
Side: [1] Buy  [2] Sell
Select: 1
Quantity: 50

Risk Check: PASSED
Confirm order? [Y/N]: Y
Order submitted and processed.

Choose: 5 (Next Day)

=== Day 2 of 10 ===
Market Prices: AAPL: 103, GOOG: 2795, MSFT: 318
Portfolio: Cash: 5000, Position: 50 AAPL, Equity: 10150 (+1.5%)

Choose: 7 (Generate Report)

=== Trading Simulation Report ===

Performance Summary:
- Starting Equity: 10000
- Ending Equity: 10150
- Total Return: 1.50%
- Max Drawdown: 0.00%
- Sharpe Ratio: 0.00

Trading Statistics:
- Total Trades: 1
- Win Rate: 0.00%
- Profit Factor: 0.00

...
```

## Project Structure

```
trading-sim-cpp-main/
├── include/              # Header files
│   ├── Asset.h          # Asset definition
│   ├── Market.h         # Market simulation
│   ├── Portfolio.h      # Portfolio management
│   ├── Order.h          # Order types
│   ├── OrderBook.h      # Order book implementation
│   ├── MatchingEngine.h # Order matching logic
│   ├── RiskManager.h    # Risk validation
│   ├── TradeLog.h       # Trade history
│   ├── EquityCurve.h    # Equity tracking
│   ├── PerformanceStats.h # Performance metrics
│   ├── ReportGenerator.h # Report generation
│   ├── Simulation.h     # Simulation engine
│   ├── InteractiveUI.h  # Interactive interface
│   └── AutoRunner.h     # Auto-run strategies
├── src/                 # Implementation files
├── build/               # Build output (generated)
├── reports/             # Generated reports (created at runtime)
└── CMakeLists.txt       # Build configuration
```

## Design Philosophy

This simulator is built from first principles with a focus on learning and understanding:

- **Modular Design**: Components are decoupled and independently testable
- **Integer Arithmetic**: All prices and calculations use integers for precision and reproducibility
- **Explicit State**: Portfolio state, positions, and prices are tracked explicitly
- **Incremental Complexity**: Features are added incrementally as needed
- **Clean Interfaces**: Clear separation between market simulation, order execution, and portfolio management

The simulator models:
- **Price-Time Priority**: Orders are matched by price first, then by time
- **Average Cost Accounting**: Positions track average entry price for PnL calculation
- **Risk Limits**: Pre-trade validation ensures margin and position limits are respected
- **Real-Time Analytics**: Equity curves and trade logs provide insight into performance

## License

See LICENSE file for details.

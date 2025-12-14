# Design Notes

## Project Intent
This project is a learning-focused trading simulator built incrementally from first principles.
The goal is to understand the mechanics of trading, price movement, and portfolio evolution
before introducing more realistic market structures.

---

## Core Mental Model
- Trading is exposure to price movement
- PnL depends on direction, price change, and position size
- A market can be simulated independently from how trades are executed
- Complexity should only be added when it introduces a new idea

---

## Current Model (V1)
- Single asset
- Fixed entry price (100)
- Binary one-day price movement (99 or 101)
- One trade per simulation
- No persistent portfolio state
- PnL computed directly from price change

This version exists to establish a baseline and validate intuition.

---

## Key Design Decisions
- Discrete time steps instead of continuous time
- Integer prices for simplicity
- Minimal user input
- No order book or execution logic yet
- Randomness kept simple and interpretable

---

## Planned Directions (High-Level)

### Portfolio & Accounting
- Introduce persistent cash and position tracking
- Mark positions to market daily
- Track equity and cumulative PnL

### Time Simulation
- Fixed start date
- Daily time step loop
- Ability to step day-by-day or fast-forward

### Market Model
- Multiple assets
- Independent price evolution per asset
- Deterministic randomness where possible

### Execution Model
- Introduce orders as explicit objects
- Add a basic order book per asset
- Compare execution-based PnL with price-based PnL

---

## Non-Goals (For Now)
- Real-world market accuracy
- High-frequency or low-latency systems
- Networking or live data
- Advanced financial instruments

---

## Reading List

- **Larry Harris — _Trading and Exchanges_**  
  Market microstructure fundamentals: order books, FIFO, bid/ask, and price formation.

- **Joel Hasbrouck — _Empirical Market Microstructure_**  
  Order flow, execution mechanics, price impact, and trading market structure.
  
- **Averill M. Law — _Simulation Modeling and Analysis_**  
  Discrete-time and event-driven simulation methodologies.

- **Robert Kissell — _The Science of Algorithmic Trading and Portfolio Management_**  
  Practical perspectives on trading systems and execution logic.

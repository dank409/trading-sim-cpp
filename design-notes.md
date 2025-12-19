# Design Notes

## Project Intent
This project is a learning-focused trading simulator built incrementally from first principles.
The goal is to understand trading mechanics, portfolio accounting, and time-based market
simulation before introducing more realistic execution models such as order books.

---

## Core Mental Model
- Trading is exposure to price movement
- PnL depends on direction, price change, and position size
- A market can be simulated independently from how trades are executed
- Complexity should only be added when it introduces a new idea

---

## Current Model (V2)
- Single asset
- Fixed initial price
- Discrete multi-day simulation
- Persistent portfolio state
- Daily mark-to-market equity calculation
- Market price evolves via a simple random walk
  
This version establishes correct portfolio accounting and time evolution as a foundation
for more advanced market structures.

---

## Key Design Decisions
- Discrete time steps instead of continuous time
- Integer prices for simplicity
- Explicit portfolio accounting (cash, position, equity)
- Short positions allowed at the accounting level
- Market dynamics kept simple and interpretable
- Execution occurs at the observed market price (no order book yet)

---

## Planned Directions (High-Level)

### Portfolio & Accounting
- Add explicit risk constraints (e.g. margin or leverage limits)
- Improve reporting (equity curve, drawdowns, summary stats)

### Time Simulation
- Configurable simulation horizon
- Step-by-step versus fast-forward simulation modes

### Market Model
- Multiple assets
- Configurable and deterministic price dynamics (seeded randomness)
- Optional shared market shocks for correlation

### Execution Model
- Introduce orders as explicit objects
- Add a basic order book per asset
- Compare execution-based outcomes with price-based outcomes

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

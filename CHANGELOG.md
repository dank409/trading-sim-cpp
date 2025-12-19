# Changelog

All notable changes to this project are documented here.

## V2 – Multi-Day Portfolio Simulation
- Added persistent portfolio state (cash and position)
- Introduced multi-day simulation loop with buy/sell/hold decisions
- Implemented average entry price tracking
- Added realized and unrealized PnL accounting
- Daily mark-to-market equity calculation
- Introduced a simple random-walk market model

## V1 – One-Day Trade Simulation
- Single buy/sell decision at a fixed entry price (100)
- One-day price movement to 99 or 101
- PnL calculated directly from position direction and price change

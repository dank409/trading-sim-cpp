#include "Position.h"
#include <cstdlib> // for std::abs

namespace trading {

Position::Position(const AssetId& assetId)
    : assetId_(assetId)
    , quantity_(0)
    , avgEntryPrice_(0)
    , realizedPnL_(0)
{
}

void Position::addToPosition(Quantity quantity, Price price) {
    if (quantity == 0) return;

    if (quantity_ == 0) {
        // Opening a new position - simply set the entry price
        quantity_ = quantity;
        avgEntryPrice_ = price;
    } else if ((quantity_ > 0 && quantity > 0) || (quantity_ < 0 && quantity < 0)) {
        // Adding to existing position in same direction
        // Calculate weighted average entry price
        // Using integer arithmetic: (oldQty * oldPrice + newQty * newPrice) / totalQty
        Quantity absOldQty = std::abs(quantity_);
        Quantity absNewQty = std::abs(quantity);
        Quantity totalQty = absOldQty + absNewQty;

        // Compute new average price using integer division
        // Multiply first to maintain precision, then divide
        avgEntryPrice_ = (absOldQty * avgEntryPrice_ + absNewQty * price) / totalQty;
        quantity_ += quantity;
    } else {
        // Adding in opposite direction - this is actually reducing/closing/reversing
        // Treat as reducing position first, then potentially opening opposite
        Quantity absQuantity = std::abs(quantity);
        Quantity absPosition = std::abs(quantity_);

        if (absQuantity <= absPosition) {
            // Reducing or closing current position
            // Realize PnL on the closed portion
            Price pnlPerShare = (quantity_ > 0) ? (price - avgEntryPrice_) : (avgEntryPrice_ - price);
            realizedPnL_ += pnlPerShare * absQuantity;
            quantity_ += quantity;

            // If fully closed, reset entry price
            if (quantity_ == 0) {
                avgEntryPrice_ = 0;
            }
        } else {
            // Reversing position - close current, then open opposite
            // First, realize PnL on closing current position
            Price pnlPerShare = (quantity_ > 0) ? (price - avgEntryPrice_) : (avgEntryPrice_ - price);
            realizedPnL_ += pnlPerShare * absPosition;

            // Open new position in opposite direction
            Quantity remainingQty = absQuantity - absPosition;
            quantity_ = (quantity > 0) ? remainingQty : -remainingQty;
            avgEntryPrice_ = price;
        }
    }
}

void Position::reducePosition(Quantity quantity, Price price) {
    if (quantity == 0 || quantity_ == 0) return;

    // Determine the reduction direction based on current position
    // If long, reducing means selling (negative quantity)
    // If short, reducing means buying (positive quantity)
    Quantity adjustedQty = (quantity_ > 0) ? -quantity : quantity;

    // Delegate to addToPosition which handles the logic
    addToPosition(adjustedQty, price);
}

Price Position::getUnrealizedPnL(Price currentPrice) const {
    if (quantity_ == 0) return 0;

    // For long positions: gain when price goes up
    // For short positions: gain when price goes down
    Price pnlPerShare = currentPrice - avgEntryPrice_;
    return pnlPerShare * quantity_;
}

} // namespace trading

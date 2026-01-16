#include "Order.h"

namespace trading {

// Initialize static counter
std::atomic<OrderId> Order::nextOrderId_{1};

Order::Order(const AssetId& assetId, Side side, Quantity quantity,
             Price limitPrice, Timestamp timestamp)
    : orderId_(generateOrderId())
    , assetId_(assetId)
    , side_(side)
    , orderType_(OrderType::Limit)
    , quantity_(quantity)
    , filledQuantity_(0)
    , limitPrice_(limitPrice)
    , timestamp_(timestamp)
    , status_(OrderStatus::Pending)
{
}

Order::Order(const AssetId& assetId, Side side, Quantity quantity, Timestamp timestamp)
    : orderId_(generateOrderId())
    , assetId_(assetId)
    , side_(side)
    , orderType_(OrderType::Market)
    , quantity_(quantity)
    , filledQuantity_(0)
    , limitPrice_(0)
    , timestamp_(timestamp)
    , status_(OrderStatus::Pending)
{
}

OrderId Order::generateOrderId() {
    return nextOrderId_.fetch_add(1);
}

void Order::resetIdCounter() {
    nextOrderId_.store(1);
}

Quantity Order::getRemainingQuantity() const {
    return quantity_ - filledQuantity_;
}

bool Order::isFilled() const {
    return filledQuantity_ >= quantity_;
}

void Order::cancel() {
    if (status_ != OrderStatus::Filled) {
        status_ = OrderStatus::Cancelled;
    }
}

void Order::fill(Quantity fillQuantity) {
    filledQuantity_ += fillQuantity;

    if (filledQuantity_ >= quantity_) {
        status_ = OrderStatus::Filled;
    } else if (filledQuantity_ > 0) {
        status_ = OrderStatus::PartiallyFilled;
    }
}

} // namespace trading

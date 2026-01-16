#include "Asset.h"

namespace trading {

Asset::Asset(const AssetId& id, const std::string& symbol, Price tickSize, Quantity lotSize)
    : id_(id)
    , symbol_(symbol)
    , tickSize_(tickSize)
    , lotSize_(lotSize)
{
}

Asset Asset::createSimple(const AssetId& id, const std::string& symbol) {
    return Asset(id, symbol, 1, 1);
}

bool Asset::operator==(const Asset& other) const {
    return id_ == other.id_;
}

bool Asset::operator!=(const Asset& other) const {
    return !(*this == other);
}

bool Asset::operator<(const Asset& other) const {
    return id_ < other.id_;
}

} // namespace trading

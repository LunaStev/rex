#include "StyleValue.h"

namespace rex::ui::core {

bool ThemeSet::has(const std::string& token) const {
    return values_.find(token) != values_.end();
}

std::optional<StyleValue> ThemeSet::find(const std::string& token) const {
    const auto it = values_.find(token);
    if (it == values_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void ThemeSet::set(const std::string& token, const StyleValue& value) {
    values_[token] = value;
}

} // namespace rex::ui::core


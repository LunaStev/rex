#include "ElementId.h"

namespace rex::ui::core {

ElementId ElementIdGenerator::next() {
    return nextId_++;
}

void ElementIdGenerator::reset(ElementId seed) {
    nextId_ = seed == kInvalidElementId ? 1 : seed;
}

} // namespace rex::ui::core


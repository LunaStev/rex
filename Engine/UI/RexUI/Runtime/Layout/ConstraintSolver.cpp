#include "ConstraintSolver.h"

#include <algorithm>

namespace rex::ui::runtime::layout {

core::Size ConstraintSolver::solve(const core::LayoutConstraints& constraints, const core::Size& desired) const {
    float minW = constraints.min.w;
    float minH = constraints.min.h;
    float maxW = constraints.max.w;
    float maxH = constraints.max.h;

    if (maxW <= 0.0f) maxW = std::max(minW, desired.w);
    if (maxH <= 0.0f) maxH = std::max(minH, desired.h);
    if (maxW < minW) std::swap(maxW, minW);
    if (maxH < minH) std::swap(maxH, minH);

    return {
        std::clamp(desired.w, minW, maxW),
        std::clamp(desired.h, minH, maxH)
    };
}

} // namespace rex::ui::runtime::layout


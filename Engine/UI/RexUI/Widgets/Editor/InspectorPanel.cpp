#include "InspectorPanel.h"

#include <utility>

namespace rex::ui::widgets::editor {

void InspectorPanel::setFields(std::vector<InspectorField> fields) {
    fields_ = std::move(fields);
}

void InspectorPanel::setOnCommit(CommitHandler handler) {
    onCommit_ = std::move(handler);
}

} // namespace rex::ui::widgets::editor


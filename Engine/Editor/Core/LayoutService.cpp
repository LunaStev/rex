#include "LayoutService.h"

namespace rex::editor::core {

std::string LayoutService::save(const ui::framework::docking::DockManager& dockManager) const {
    return serializer_.serialize(dockManager);
}

bool LayoutService::restore(const std::string& blob, ui::framework::docking::DockManager& dockManager) const {
    return serializer_.deserialize(blob, dockManager);
}

} // namespace rex::editor::core


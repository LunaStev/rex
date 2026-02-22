#include "EditorSession.h"

namespace rex::editor::core {

bool EditorSession::open(const EditorWorkspace& workspace) {
    if (open_) return false;
    workspace_ = workspace;
    open_ = true;
    ++sessionId_;
    return true;
}

void EditorSession::close() {
    open_ = false;
    workspace_ = {};
}

bool EditorSession::isOpen() const {
    return open_;
}

std::uint64_t EditorSession::sessionId() const {
    return sessionId_;
}

const EditorWorkspace& EditorSession::workspace() const {
    return workspace_;
}

} // namespace rex::editor::core


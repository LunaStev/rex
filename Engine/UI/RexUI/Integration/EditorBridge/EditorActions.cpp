#include "EditorActions.h"

#include <memory>
#include <string>
#include <utility>

#include "../../Framework/Commands/UICommand.h"

namespace rex::ui::integration::editorbridge {

namespace {
class SimpleEditorCommand final : public framework::commands::UICommand {
public:
    explicit SimpleEditorCommand(std::string name)
        : name_(std::move(name)) {}

    const char* name() const override {
        return name_.c_str();
    }

    bool execute(framework::commands::UICommandContext& ctx) override {
        (void)ctx;
        return true;
    }

    bool undo(framework::commands::UICommandContext& ctx) override {
        (void)ctx;
        return true;
    }

    bool redo(framework::commands::UICommandContext& ctx) override {
        (void)ctx;
        return true;
    }

private:
    std::string name_;
};
} // namespace

std::shared_ptr<framework::commands::UICommand> EditorActions::makeAddEntityAction(const std::string& archetype) const {
    return std::make_shared<SimpleEditorCommand>("AddEntity:" + archetype);
}

std::shared_ptr<framework::commands::UICommand> EditorActions::makeDeleteSelectedAction() const {
    return std::make_shared<SimpleEditorCommand>("DeleteSelected");
}

std::shared_ptr<framework::commands::UICommand> EditorActions::makeSetGizmoModeAction(const std::string& mode) const {
    return std::make_shared<SimpleEditorCommand>("SetGizmoMode:" + mode);
}

std::shared_ptr<framework::commands::UICommand> EditorActions::makeApplyTransformAction(
    std::uint64_t entityId,
    const std::string& position,
    const std::string& rotation,
    const std::string& scale) const {
    return std::make_shared<SimpleEditorCommand>(
        "ApplyTransform:" + std::to_string(entityId) + ":" + position + ":" + rotation + ":" + scale);
}

} // namespace rex::ui::integration::editorbridge

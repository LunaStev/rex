#include "ShortcutMap.h"

namespace rex::editor::plugin {

namespace {
std::uint64_t chordHash(const ShortcutChord& chord) {
    std::uint64_t h = static_cast<std::uint64_t>(static_cast<std::uint32_t>(chord.keyCode));
    h |= static_cast<std::uint64_t>(chord.ctrl ? 1u : 0u) << 32;
    h |= static_cast<std::uint64_t>(chord.shift ? 1u : 0u) << 33;
    h |= static_cast<std::uint64_t>(chord.alt ? 1u : 0u) << 34;
    return h;
}
}

void ShortcutMap::bind(const std::string& context, const ShortcutChord& chord, const std::string& commandId) {
    bindings_[context][chordHash(chord)] = commandId;
}

void ShortcutMap::unbind(const std::string& context, const ShortcutChord& chord) {
    auto it = bindings_.find(context);
    if (it == bindings_.end()) return;
    it->second.erase(chordHash(chord));
}

std::optional<std::string> ShortcutMap::resolve(const std::string& context, const ShortcutChord& chord) const {
    auto it = bindings_.find(context);
    if (it == bindings_.end()) return std::nullopt;
    auto cmdIt = it->second.find(chordHash(chord));
    if (cmdIt == it->second.end()) return std::nullopt;
    return cmdIt->second;
}

} // namespace rex::editor::plugin


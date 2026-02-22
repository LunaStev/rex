#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rex::editor::plugin {

class EditorPanelRegistry;
class EditorToolRegistry;
class InspectorWidgetRegistry;
class CommandRegistry;
class ShortcutMap;

class IEditorModule {
public:
    virtual ~IEditorModule() = default;

    virtual const char* name() const = 0;
    virtual std::uint32_t version() const = 0;

    virtual bool startup(
        EditorPanelRegistry& panels,
        EditorToolRegistry& tools,
        InspectorWidgetRegistry& inspectors,
        CommandRegistry& commands,
        ShortcutMap& shortcuts) = 0;

    virtual void shutdown(
        EditorPanelRegistry& panels,
        EditorToolRegistry& tools,
        InspectorWidgetRegistry& inspectors,
        CommandRegistry& commands,
        ShortcutMap& shortcuts) = 0;
};

// TODO [Editor-Plugin-006]:
// 책임: 에디터 모듈 ABI 인터페이스 제공
// 요구사항:
//  - startup/shutdown 수명주기
//  - 버전 정보 제공
//  - 레지스트리 기반 확장 등록
// 의존성:
//  - Editor/Plugin/*
// 구현 단계: Phase C
// 성능 고려사항:
//  - 모듈 로드 시 초기화 비용 제어
//  - 실패 모듈 격리
// 테스트 전략:
//  - 정상/실패 모듈 로드 테스트
//  - shutdown 정리 테스트

} // namespace rex::editor::plugin

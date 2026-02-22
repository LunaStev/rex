#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "PanelRegistry.h"
#include "WidgetRegistry.h"

namespace rex::ui::framework::plugins {

class IUIPlugin {
public:
    virtual ~IUIPlugin() = default;

    virtual const char* name() const = 0;
    virtual bool onLoad(WidgetRegistry& widgetRegistry, PanelRegistry& panelRegistry) = 0;
    virtual void onUnload(WidgetRegistry& widgetRegistry, PanelRegistry& panelRegistry) = 0;
};

class PluginHost {
public:
    bool load(const std::shared_ptr<IUIPlugin>& plugin);
    bool unload(const std::string& pluginName);
    bool isLoaded(const std::string& pluginName) const;

    WidgetRegistry& widgetRegistry();
    PanelRegistry& panelRegistry();

private:
    WidgetRegistry widgetRegistry_{};
    PanelRegistry panelRegistry_{};
    std::unordered_map<std::string, std::shared_ptr<IUIPlugin>> loadedPlugins_;
};

// TODO [RexUI-Framework-Plugins-003]:
// 책임: UI 플러그인 라이프사이클 관리
// 요구사항:
//  - 플러그인 로드/언로드/조회 API
//  - 레지스트리 등록 자원 정리 보장
//  - 플러그인 실패 격리
// 의존성:
//  - Plugins/WidgetRegistry
//  - Plugins/PanelRegistry
// 구현 단계: Phase D
// 성능 고려사항:
//  - 로드 시 심볼 탐색 비용 관리
//  - 핫 리로드 시 메모리 파편화 최소화
// 테스트 전략:
//  - 정상/실패 플러그인 로드 테스트
//  - 언로드 후 재로드 안정성 테스트

} // namespace rex::ui::framework::plugins

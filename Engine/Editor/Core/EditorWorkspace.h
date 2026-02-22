#pragma once

#include <string>

namespace rex::editor::core {

struct EditorWorkspace {
    std::string projectRoot;
    std::string contentRoot;
    std::string configRoot;
    std::string cacheRoot;
};

// TODO [Editor-Core-007]:
// 책임: 에디터 워크스페이스 경로/컨텍스트 정보 보관
// 요구사항:
//  - 프로젝트/컨텐츠/설정/캐시 루트 보관
//  - 세션 시작 시 경로 유효성 검증 포인트
//  - 다중 프로젝트 전환 지원
// 의존성:
//  - 없음
// 구현 단계: Phase A
// 성능 고려사항:
//  - 경로 문자열 재할당 최소화
//  - 경로 정규화 캐시
// 테스트 전략:
//  - 프로젝트 전환 테스트
//  - 경로 유효성 테스트

} // namespace rex::editor::core


#pragma once

#include <string>

#include "UIBuilder.h"

namespace rex::ui::framework::declarative {

class MarkupParser {
public:
    ViewNodeDesc parseText(const std::string& markupText) const;
    ViewNodeDesc parseFile(const std::string& filePath) const;
};

// TODO [RexUI-Framework-Declarative-002]:
// 책임: 마크업/DSL 입력 파싱
// 요구사항:
//  - 위젯 타입/속성/자식 노드 파싱
//  - 문법 오류 진단 정보(line/column)
//  - 안전한 기본값 규칙 제공
// 의존성:
//  - Declarative/UIBuilder
// 구현 단계: Phase C
// 성능 고려사항:
//  - 파서 단일 패스 처리
//  - 대형 문서 스트리밍 파싱 옵션
// 테스트 전략:
//  - 정상/오류 문법 테스트 세트
//  - 역직렬화 안정성 테스트

} // namespace rex::ui::framework::declarative

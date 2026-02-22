#pragma once

#include <string>

namespace rex::ui::framework::binding {

enum class BindingMode {
    OneWay,
    TwoWay,
    OneTime
};

struct BindingExpression {
    std::string sourcePath;
    std::string targetProperty;
    BindingMode mode = BindingMode::OneWay;
    std::string converter;
    std::string validator;
};

// TODO [RexUI-Framework-Binding-001]:
// 책임: 바인딩 표현식 스키마 정의
// 요구사항:
//  - source/target 경로 규칙
//  - mode(OneWay/TwoWay/OneTime) 계약
//  - converter/validator 확장 포인트
// 의존성:
//  - 없음
// 구현 단계: Phase C
// 성능 고려사항:
//  - 표현식 파싱 캐시
//  - 런타임 문자열 할당 최소화
// 테스트 전략:
//  - 표현식 파서/검증기 단위 테스트
//  - 잘못된 경로 오류 테스트

} // namespace rex::ui::framework::binding

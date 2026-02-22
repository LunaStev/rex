#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace rex::core::platform {

class FileSystem {
public:
    static bool exists(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    }

    static std::optional<std::string> readText(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) return std::nullopt;
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    static bool writeText(const std::filesystem::path& path, const std::string& content) {
        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << content;
        return true;
    }
};

// TODO [Core-Platform-002]:
// 책임: 파일 시스템 공통 API 제공
// 요구사항:
//  - 존재 확인/텍스트 읽기/텍스트 쓰기
//  - 플랫폼 독립 경로 타입 사용
//  - 비동기 IO 확장 포인트
// 의존성:
//  - 없음
// 구현 단계: Phase D
// 성능 고려사항:
//  - 대용량 파일 스트리밍 지원 확장
//  - 경로 정규화 비용 제어
// 테스트 전략:
//  - 읽기/쓰기 라운드트립 테스트
//  - 없는 파일 경계 테스트

} // namespace rex::core::platform


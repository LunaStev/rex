#include "SSAOPass.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <string>

namespace rex::gfx {

namespace {

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

} // namespace

SSAOPass::SSAOPass() {
    const std::string vs = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;

        out vec2 vUV;

        void main() {
            vUV = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    const std::string ssaoFs = R"(
        #version 450 core
        in vec2 vUV;
        out float FragOcclusion;

        uniform sampler2D uGPosition;
        uniform sampler2D uGNormal;
        uniform sampler2D uNoise;

        uniform vec3 uSamples[16];
        uniform mat4 uProjection;
        uniform vec3 uNoiseScale;

        void main() {
            vec3 fragPos = texture(uGPosition, vUV).xyz;
            vec3 normal = normalize(texture(uGNormal, vUV).xyz * 2.0 - 1.0);
            vec3 randomVec = normalize(texture(uNoise, vUV * uNoiseScale.xy).xyz * 2.0 - 1.0);

            vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
            vec3 bitangent = cross(normal, tangent);
            mat3 TBN = mat3(tangent, bitangent, normal);

            float occlusion = 0.0;
            float radius = 1.1;

            for (int i = 0; i < 16; ++i) {
                vec3 samplePos = fragPos + (TBN * uSamples[i]) * radius;

                vec4 offset = uProjection * vec4(samplePos, 1.0);
                offset.xyz /= max(offset.w, 0.0001);
                offset.xyz = offset.xyz * 0.5 + 0.5;

                if (offset.x < 0.0 || offset.x > 1.0 || offset.y < 0.0 || offset.y > 1.0) {
                    continue;
                }

                vec3 sampledPos = texture(uGPosition, offset.xy).xyz;
                float dist = length(sampledPos - fragPos);
                float rangeWeight = smoothstep(0.0, 1.0, radius / (dist + 0.0001));
                occlusion += dist < radius ? rangeWeight : 0.0;
            }

            occlusion = 1.0 - (occlusion / 16.0);
            FragOcclusion = clamp(occlusion, 0.0, 1.0);
        }
    )";

    const std::string blurFs = R"(
        #version 450 core
        in vec2 vUV;
        out float FragOcclusion;

        uniform sampler2D uAO;
        uniform vec3 uTexel;

        void main() {
            float result = 0.0;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    vec2 offset = vec2(float(x), float(y)) * uTexel.xy;
                    result += texture(uAO, vUV + offset).r;
                }
            }
            FragOcclusion = result / 9.0;
        }
    )";

    m_ssaoShader = std::make_unique<Shader>(vs, ssaoFs);
    m_blurShader = std::make_unique<Shader>(vs, blurFs);

    buildKernelAndNoise();
}

SSAOPass::~SSAOPass() {
    if (m_noiseTex) {
        glDeleteTextures(1, &m_noiseTex);
        m_noiseTex = 0;
    }
}

void SSAOPass::buildKernelAndNoise() {
    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> rand01(0.0f, 1.0f);

    m_kernel.clear();
    m_kernel.reserve(16);
    for (int i = 0; i < 16; ++i) {
        Vec3 sample{
            rand01(rng) * 2.0f - 1.0f,
            rand01(rng) * 2.0f - 1.0f,
            rand01(rng)
        };

        const float len = std::sqrt(dot(sample, sample));
        if (len > 1e-6f) {
            sample = sample * (1.0f / len);
        }

        sample = sample * rand01(rng);
        const float scale = float(i) / 16.0f;
        const float bias = lerp(0.1f, 1.0f, scale * scale);
        sample = sample * bias;

        m_kernel.push_back(sample);
    }

    Vec3 noiseData[16];
    for (auto& n : noiseData) {
        n = {rand01(rng) * 2.0f - 1.0f, rand01(rng) * 2.0f - 1.0f, 0.0f};
    }

    glGenTextures(1, &m_noiseTex);
    glBindTexture(GL_TEXTURE_2D, m_noiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, noiseData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool SSAOPass::ensure(int width, int height) {
    if (width <= 0 || height <= 0) return false;

    if (m_width == width && m_height == height && m_ssaoBuffer.isValid() && m_blurBuffer.isValid()) {
        return true;
    }

    m_width = width;
    m_height = height;

    std::vector<ColorAttachmentDesc> aoAttachment;
    ColorAttachmentDesc ao;
    ao.internalFormat = GL_R16F;
    ao.format = GL_RED;
    ao.type = GL_FLOAT;
    ao.minFilter = GL_LINEAR;
    ao.magFilter = GL_LINEAR;
    aoAttachment.push_back(ao);

    if (!m_ssaoBuffer.create(width, height, aoAttachment, DepthAttachmentMode::None)) return false;
    if (!m_blurBuffer.create(width, height, aoAttachment, DepthAttachmentMode::None)) return false;

    return true;
}

uint32_t SSAOPass::execute(uint32_t gPosition,
                           uint32_t gNormal,
                           const Mat4& projection,
                           uint32_t screenVAO,
                           int width,
                           int height) {
    if (!ensure(width, height)) {
        return 0;
    }

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBuffer.id());
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_ssaoShader->bind();
    m_ssaoShader->setUniform("uGPosition", 0);
    m_ssaoShader->setUniform("uGNormal", 1);
    m_ssaoShader->setUniform("uNoise", 2);
    m_ssaoShader->setUniform("uProjection", projection);
    m_ssaoShader->setUniform("uNoiseScale", Vec3{float(width) / 4.0f, float(height) / 4.0f, 0.0f});

    for (int i = 0; i < static_cast<int>(m_kernel.size()); ++i) {
        const std::string name = "uSamples[" + std::to_string(i) + "]";
        m_ssaoShader->setUniform(name, m_kernel[i]);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noiseTex);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindFramebuffer(GL_FRAMEBUFFER, m_blurBuffer.id());
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    m_blurShader->bind();
    m_blurShader->setUniform("uAO", 0);
    m_blurShader->setUniform("uTexel", Vec3{1.0f / float(width), 1.0f / float(height), 0.0f});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBuffer.colorTexture(0));

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    return m_blurBuffer.colorTexture(0);
}

} // namespace rex::gfx

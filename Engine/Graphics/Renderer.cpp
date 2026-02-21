#include "Renderer.h"
#include "../Core/Components.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include <iostream>

namespace rex {

Renderer::Renderer() {
    std::string pbrVS = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        out vec3 WorldPos;
        out vec3 Normal;

        void main() {
            vec4 world = model * vec4(aPos, 1.0);
            WorldPos = world.xyz;
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = proj * view * world;
        }
    )";
    
    std::string pbrFS = R"(
        #version 450 core
        out vec4 FragColor;

        in vec3 WorldPos;
        in vec3 Normal;

        uniform vec3 albedo;
        uniform float metallic;
        uniform float roughness;
        uniform float ao;

        uniform vec3 viewPos;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform float lightIntensity;

        const float PI = 3.14159265359;

        float DistributionGGX(vec3 N, vec3 H, float r) {
            float a = r * r;
            float a2 = a * a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;

            float num = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = PI * denom * denom;
            return num / max(denom, 0.0001);
        }

        float GeometrySchlickGGX(float NdotV, float r) {
            float k = (r + 1.0);
            k = (k * k) / 8.0;
            float num = NdotV;
            float denom = NdotV * (1.0 - k) + k;
            return num / denom;
        }

        float GeometrySmith(vec3 N, vec3 V, vec3 L, float r) {
            float NdotV = max(dot(N, V), 0.0);
            float NdotL = max(dot(N, L), 0.0);
            float ggx2 = GeometrySchlickGGX(NdotV, r);
            float ggx1 = GeometrySchlickGGX(NdotL, r);
            return ggx1 * ggx2;
        }

        vec3 fresnelSchlick(float cosTheta, vec3 F0) {
            return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
        }

        void main() {
            vec3 N = normalize(Normal);
            vec3 V = normalize(viewPos - WorldPos);
            vec3 L = normalize(lightPos - WorldPos);
            vec3 H = normalize(V + L);

            vec3 radiance = lightColor * lightIntensity;

            vec3 F0 = vec3(0.04);
            F0 = mix(F0, albedo, metallic);

            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 nominator    = NDF * G * F;
            float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = nominator / denom;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            float NdotL = max(dot(N, L), 0.0);
            vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

            vec3 ambient = vec3(0.12) * albedo * ao;
            vec3 color = ambient + Lo;

            FragColor = vec4(color, 1.0);
        }
    )";

    std::string tonemapVS = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;

        out vec2 TexCoords;

        void main() {
            TexCoords = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    std::string tonemapFS = R"(
        #version 450 core
        out vec4 FragColor;
        in vec2 TexCoords;

        uniform sampler2D hdrBuffer;
        uniform float exposure;

        vec3 ACESFilm(vec3 x) {
            const float a = 2.51;
            const float b = 0.03;
            const float c = 2.43;
            const float d = 0.59;
            const float e = 0.14;
            return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
        }

        void main() {
            vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
            vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
            mapped = ACESFilm(mapped);
            mapped = pow(mapped, vec3(1.0/2.2));
            FragColor = vec4(mapped, 1.0);
        }
    )";

    m_pbrShader = std::make_unique<Shader>(pbrVS, pbrFS);
    m_tonemapShader = std::make_unique<Shader>(tonemapVS, tonemapFS);
    initScreenTriangle();
}

Renderer::~Renderer() {
    if (m_hdrFBO) glDeleteFramebuffers(1, &m_hdrFBO);
    if (m_hdrColor) glDeleteTextures(1, &m_hdrColor);
    if (m_hdrDepth) glDeleteRenderbuffers(1, &m_hdrDepth);
    if (m_screenVBO) glDeleteBuffers(1, &m_screenVBO);
    if (m_screenVAO) glDeleteVertexArrays(1, &m_screenVAO);
}

void Renderer::initScreenTriangle() {
    float verts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         3.0f, -1.0f, 2.0f, 0.0f,
        -1.0f,  3.0f, 0.0f, 2.0f
    };

    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::ensureHDRResources(int w, int h) {
    if (w <= 0 || h <= 0) return;
    if (m_hdrFBO && w == m_hdrWidth && h == m_hdrHeight) return;

    if (m_hdrFBO) glDeleteFramebuffers(1, &m_hdrFBO);
    if (m_hdrColor) glDeleteTextures(1, &m_hdrColor);
    if (m_hdrDepth) glDeleteRenderbuffers(1, &m_hdrDepth);

    m_hdrWidth = w;
    m_hdrHeight = h;

    glGenFramebuffers(1, &m_hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);

    glGenTextures(1, &m_hdrColor);
    glBindTexture(GL_TEXTURE_2D, m_hdrColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrColor, 0);

    glGenRenderbuffers(1, &m_hdrDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_hdrDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hdrDepth);

    GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[Renderer] HDR framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render(Scene& scene, const Camera& camera, const Mat4& viewMatrix, const Vec3& viewPos,
                      int targetWidth, int targetHeight, uint32_t backbufferFBO,
                      const std::function<void(const Mat4&, const Mat4&)>& extraDraw) {
    if (!glUseProgram) return;

    ensureHDRResources(targetWidth, targetHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
    glViewport(0, 0, targetWidth, targetHeight);
    glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_pbrShader->bind();
    m_pbrShader->setUniform("proj", camera.getProjection());
    m_pbrShader->setUniform("view", viewMatrix);
    m_pbrShader->setUniform("viewPos", viewPos);

    Vec3 lightPos{5, 10, 5};
    Vec3 lightColor{1, 1, 1};
    float lightIntensity = 5.0f;

    scene.each<Light>([&](EntityId id, Light& l) {
        auto* t = scene.getComponent<Transform>(id);
        if (t) {
            lightPos = t->position;
            lightColor = l.color;
            lightIntensity = l.intensity;
        }
    });

    m_pbrShader->setUniform("lightPos", lightPos);
    m_pbrShader->setUniform("lightColor", lightColor);
    m_pbrShader->setUniform("lightIntensity", lightIntensity);

    scene.each<MeshRenderer>([&](EntityId id, MeshRenderer& mr) {
        auto* transform = scene.getComponent<Transform>(id);
        Mat4 model = transform ? transform->getMatrix() : Mat4::identity();
        
        m_pbrShader->setUniform("model", model);
        m_pbrShader->setUniform("albedo", mr.color);
        m_pbrShader->setUniform("metallic", mr.metallic);
        m_pbrShader->setUniform("roughness", mr.roughness);
        m_pbrShader->setUniform("ao", mr.ao);
        
        if (mr.model) {
            mr.model->draw();
        } else if (mr.mesh) {
            mr.mesh->draw();
        }
    });

    if (extraDraw) {
        extraDraw(viewMatrix, camera.getProjection());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, backbufferFBO);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_tonemapShader->bind();
    m_tonemapShader->setUniform("hdrBuffer", 0);
    m_tonemapShader->setUniform("exposure", 1.2f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdrColor);

    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

}

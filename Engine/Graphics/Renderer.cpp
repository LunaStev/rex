#include "Renderer.h"
#include "../Core/Components.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

namespace rex {

Renderer::Renderer() {
    std::string vSrc = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aTexCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        out vec3 FragPos;
        out vec3 Normal;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = proj * view * vec4(FragPos, 1.0);
        }
    )";
    
    std::string fSrc = R"(
        #version 450 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;

        uniform vec3 uColor;
        uniform vec3 viewPos;

        // Simple single light for now
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform float lightIntensity;

        void main() {
            // Ambient
            float ambientStrength = 0.2;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * lightIntensity;
            
            // Specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;  
                
            vec3 result = (ambient + diffuse + specular) * uColor;
            FragColor = vec4(result, 1.0);
        }
    )";
    
    m_defaultShader = std::make_unique<Shader>(vSrc, fSrc);
}

Renderer::~Renderer() = default;

void Renderer::render(Scene& scene, const Camera& camera, const Mat4& viewMatrix) {
    if (!glUseProgram) return;

    m_defaultShader->bind();
    m_defaultShader->setUniform("proj", camera.getProjection());
    m_defaultShader->setUniform("view", viewMatrix);
    m_defaultShader->setUniform("viewPos", Vec3{0, 0, 0}); // Simplified

    // Default light if none in scene
    Vec3 lightPos{5, 10, 5};
    Vec3 lightColor{1, 1, 1};
    float lightIntensity = 1.0f;

    // Find a light in the scene
    scene.each<Light>([&](EntityId id, Light& l) {
        auto* t = scene.getComponent<Transform>(id);
        if (t) {
            lightPos = t->position;
            lightColor = l.color;
            lightIntensity = l.intensity;
        }
    });

    m_defaultShader->setUniform("lightPos", lightPos);
    m_defaultShader->setUniform("lightColor", lightColor);
    m_defaultShader->setUniform("lightIntensity", lightIntensity);

    scene.each<MeshRenderer>([&](EntityId id, MeshRenderer& mr) {
        auto* transform = scene.getComponent<Transform>(id);
        Mat4 model = transform ? transform->getMatrix() : Mat4::identity();
        
        m_defaultShader->setUniform("model", model);
        m_defaultShader->setUniform("uColor", mr.color);
        
        if (mr.model) {
            mr.model->draw();
        } else if (mr.mesh) {
            mr.mesh->draw();
        }
    });
}

}

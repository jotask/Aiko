#include "modules/render_module.h"

#include <string>
#include <fstream>

#include <core/libs.h>

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/camera.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko
{

    void RenderModule::initMesh( Mesh* mesh )
    {
        glGenVertexArrays(1, &mesh->m_data.vao);
        glGenBuffers(1, &mesh->m_data.vbo);
        glGenBuffers(1, &mesh->m_data.ebo);
        refreshMesh( mesh );
    }

    void RenderModule::refreshMesh(Mesh* mesh)
    {

        glBindVertexArray(mesh->m_data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->m_data.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(float), mesh->m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

    }

    void RenderModule::renderMesh(Camera* cam,  Transform* transform, Mesh* mesh, Shader* shader)
    {

        glUseProgram(shader->m_shaderData.id);

        auto projection = cam->getProjectionMatrix();
        shader->setMat4("projection", projection);

        auto view = cam->getViewMatrix();
        shader->setMat4( "view", view);

        mat4 model = transform->getMatrix();
        shader->setMat4("model", model);

        glBindVertexArray(mesh->m_data.vao);
        glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

}


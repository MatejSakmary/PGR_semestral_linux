#include "mesh.h"


void checkGLError(const char *where = 0, int line = 0) {
  GLenum err = glGetError();
  if(err == GL_NONE)
    return;

  std::string errString = "<unknown>";
  switch(err) {
    case GL_INVALID_ENUM:
      errString = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      errString = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      errString = "GL_INVALID_OPERATION";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      errString = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      errString = "GL_OUT_OF_MEMORY";
      break;
    default:;
  }
  if(where == 0 || *where == 0)
    std::cerr << "GL error occurred: " << errString << std::endl;
  else
    std::cerr << "GL error occurred in " << where << ":" << line << ": " << errString << std::endl;
}

#define CHECK_GL_ERROR() do { checkGLError(__FUNCTION__, __LINE__); } while(0)

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    CHECK_GL_ERROR();
    glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader)
{
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;

    shader.use();
    /* go through all the loaded textures*/
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        /* activate each texture GL_TEXTURE0 + n = GL_TEXTUREN*/
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNum++);
        }else if (name == "texture_specular")
        {
            number = std::to_string(specularNum++);
        }
        // this sets texture sampler with name and corresponding number in the shader
        shader.setFloat(("material."+ name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR();

    glBindVertexArray(VAO);
    CHECK_GL_ERROR();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

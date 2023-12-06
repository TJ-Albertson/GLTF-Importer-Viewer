/*-------------------------------------------------------------------------------\
model.h

Functions:

\-------------------------------------------------------------------------------*/

#ifndef MODEL_H
#define MODEL_H

#define MAX_BONE_INFLUENCE 4

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <gltf/gltf_structures.h>

#include <gltf/gltf_gl.h>
#include <shader_m.h>

typedef struct {
    unsigned int m_VAO;
    int m_NumIndices;

    unsigned int materialIndex;
} Primitive;

typedef struct Mesh {
    int numPrimitives;
    Primitive* primitives;
} Mesh;

typedef struct {

    int meshIndex;
    int jointIndex;
    int nodeIndex;

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    int numChildren;
    int* childrenIndices;
} Node;

typedef struct {
    int numRootNodes;
    int* rootNodeIndices;

    Node* nodes;

    int numMaterials;
    Material* materials;

    int numMeshes;
    Mesh* meshes;
} Model;

glm::mat4* FinalBoneMatrices;
glm::mat4* inverseBindMatrices;



void create_nodes(gltfNode* gltf_nodes, int numNodes)
{
    Node* nodes = (Node*)malloc(numNodes * sizeof(Node));

    int i;
    for (i = 0; i < numNodes; ++i)
    {
        gltfNode gltf_node = gltf_nodes[i];
        
        nodes[i].meshIndex = gltf_node.m_MeshIndex;
        nodes[i].jointIndex = -1;
        nodes[i].nodeIndex = i;

        nodes[i].translation = gltf_node.translation;
        nodes[i].rotation = gltf_node.rotation;
        nodes[i].scale = gltf_node.scale;

        int numChildren = gltf_node.m_NumChildren;

        nodes[i].numChildren = numChildren;

        nodes[i].childrenIndices = (int*)malloc(numChildren * sizeof(int));

        int j;
        for (j = 0; j < numChildren; ++j)
        {
            nodes[i].childrenIndices[j] = gltf_node.m_ChildrenIndexes[j];
        }
    }

}

void mark_joint_nodes(Node* nodes, int* joints, int numJoints)
{
    int i;
    for (i = 0; i < numJoints; ++i) {
        nodes[joints[i]].jointIndex = i;
    }
}

glm::mat4 getNodeTransform(Node node)
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), node.translation);
    glm::mat4 rotationMatrix = glm::mat4_cast(node.rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), node.scale);

    glm::mat4 localTransform = translationMatrix * rotationMatrix * scaleMatrix;

    return localTransform;
}

void DrawModelNode(Node* nodes, Node node, glm::mat4 parentTransform, unsigned int shaderId)
{
    glm::mat4 globalTransform;

    if (node.jointIndex >= 0) {
        glm::mat4 inverseBindMatrix = inverseBindMatrices[node.jointIndex];
        glm::mat4 boneTransform = getBoneTransform(node.jointIndex);

        globalTransform = boneTransform * parentTransform * inverseBindMatrix;

        FinalBoneMatrices[node.jointIndex] = globalTransform;

    } else {
        glm::mat4 nodeTransform = getNodeTransform(node);

        globalTransform = nodeTransform * parentTransform;
    }

    glUseProgram(shaderId);
    setShaderMat4(shaderId, "model", globalTransform);
    
    int i;
    for (i = 0; i < node.numChildren; ++i) {
        DrawModelNode(nodes, nodes[node.childrenIndices[i]], globalTransform, shaderId);
    }
}
#endif
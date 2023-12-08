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
#include <gltf/gltf_animation.h>

#include <gltf/gltf_gl.h>
#include <shader_m.h>



glm::mat4* FinalBoneMatrices;
glm::mat4* inverseBindMatrices;


Node* create_nodes(gltfNode* gltf_nodes, int numNodes)
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


    return nodes;
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

void DrawModelNode(unsigned int shaderId, Node* nodes, Node node, glm::mat4 parentTransform, Animation animation, Skin* skin)
{
    glm::mat4 globalTransform;

    if (node.jointIndex >= 0) {
        glm::mat4 inverseBindMatrix = skin->inverseBindMatrices[node.jointIndex];
        glm::mat4 boneTransform = animateBone(node.nodeIndex, 0.001f, animation);

        globalTransform = boneTransform * parentTransform * inverseBindMatrix;

        skin->finalBoneMatrices[node.jointIndex] = globalTransform;

    } else {
        glm::mat4 nodeTransform = getNodeTransform(node);

        globalTransform = nodeTransform * parentTransform;
    }

    //glUseProgram(shaderId);
    //setShaderMat4(shaderId, "model", globalTransform);
    
    int i;
    for (i = 0; i < node.numChildren; ++i) {
        DrawModelNode(shaderId, nodes, nodes[node.childrenIndices[i]], globalTransform, animation, skin);
    }
}


void printNodeArray(const Node* nodes, int numNodes)
{
    for (int i = 0; i < numNodes; ++i) {
        printf("Node %d:\n", i);
        printf("  Mesh Index: %d\n", nodes[i].meshIndex);
        printf("  Joint Index: %d\n", nodes[i].jointIndex);
        printf("  Node Index: %d\n", nodes[i].nodeIndex);
        printf("  Translation: (%f, %f, %f)\n", nodes[i].translation.x, nodes[i].translation.y, nodes[i].translation.z);
        printf("  Rotation: (%f, %f, %f, %f)\n", nodes[i].rotation.x, nodes[i].rotation.y, nodes[i].rotation.z, nodes[i].rotation.w);
        printf("  Scale: (%f, %f, %f)\n", nodes[i].scale.x, nodes[i].scale.y, nodes[i].scale.z);
        printf("  Number of Children: %d\n", nodes[i].numChildren);

        printf("  Children Indices:");
        for (int j = 0; j < nodes[i].numChildren; ++j) {
            printf(" %d", nodes[i].childrenIndices[j]);
        }
        printf("\n\n");
    }
}


#endif
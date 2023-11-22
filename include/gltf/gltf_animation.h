/*-------------------------------------------------------------------------------\
gltf_animation.h

animation, skeletons
\-------------------------------------------------------------------------------*/
#ifndef GLTF_ANIMATION_H
#define GLTF_ANIMATION_H

#include <glm/gtx/quaternion.hpp>

#include "gltf/gltf_structures.h"

typedef struct KeyframePosition {
    float m_Time;
    glm::vec3 m_Position;
} KeyframePosition;

typedef struct KeyframeRotation {
    float m_Time;
    glm::quat m_Rotation;
} KeyframeRotation;

typedef struct KeyframeScale {
    float m_Time;
    glm::vec3 Scale;
};

typedef struct Channel {

	int m_NodeIndex;
	
	int m_NumKeyframePosition;
    KeyframePosition* m_KeyframePositions;

	int m_NumKeyframeRotations;
    KeyframeRotation* m_KeyframeRotations;

	int m_NumKeyframeScales;
    KeyframeScale* m_KeyframeScales;
} Channel;

typedef struct Node {
    float m_Translation[3];
    float m_Rotation[4];
    float m_Scale[3];

    int m_MeshIndex;
    int m_BoneId;

    int m_NumChildren;
    Node* m_Children;
} Node;

typedef struct Animation {
	
	char m_Name[256];

	int m_NumChannels;
    Channel* m_Channels;
} Animation;

typedef struct Skin {
    char m_Name[256];

    int m_NumJoints;

    int* m_Joints;
    glm::mat4* m_InverseBindMatrices;
} Skin;

typedef struct gltfSceneNode {

} gltfSceneNode;


glm::mat4 GetNodeMatrix(Node node) {
    
    glm::mat4 matrix;
    
    return matrix;
}

/*
void TraverseNode(Animation animation, Node node, glm::mat4* FinalBoneMatrix, glm::mat4 parentTransform)
{
    glm::mat4 nodeTransform = GetNodeMatrix(node);

    if (node.m_BoneId >= 0) {
        nodeTransform = FindBoneAndGetTransform(animation, node.m_NodeName, m_CurrentTime);
        glm::mat4 finalBoneMatrix = parentTransform * nodeTransform * node.m_Offset;
        FinalBoneMatrix[node.m_BoneId] = finalBoneMatrix;
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    if (node.m_MeshIndex >= 0) {
       // draw mesh
        //gltf_draw_mesh(int meshIndex, mat4 matrix);
    }

    for (int i = 0; i < node.m_NumChildren; ++i) {
        TraverseNode(animation, node.m_Children[i], FinalBoneMatrix, globalTransformation);
    }
}
*/

#endif
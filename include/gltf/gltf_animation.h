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

typedef struct Bone {
	glm::mat3 m_Transformation;

	int m_NumChildren;
    Bone* m_Children;
} Bone;

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


void CalculateNodeTransform(Animation animation, Bone* node, glm::mat4* FinalBoneMatrix, glm::mat4 parentTransform)
{
    glm::mat4 nodeTransform = node->m_Transformation;

    bool isBoneNode = (node->id >= 0);
    if (isBoneNode) {
        nodeTransform = FindBoneAndGetTransform(animation, node->m_NodeName, m_CurrentTime);
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    if (isBoneNode) {
        glm::mat4 finalBoneMatrix = globalTransformation * node->m_Offset;
        FinalBoneMatrix[node->id] = finalBoneMatrix;
    }

    for (int i = 0; i < node->m_NumChildren; ++i) {
        CalculateNodeTransform(animation, node->m_Children[i], FinalBoneMatrix, globalTransformation);
    }
}

#endif
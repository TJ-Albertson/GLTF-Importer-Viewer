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
	


	// get from sampler
	int m_NumKeyframes;
    Keyframe* m_Keyframes;


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

#endif
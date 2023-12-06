/*-------------------------------------------------------------------------------\
gltf_animation.h

animation, skeletons
\-------------------------------------------------------------------------------*/
#ifndef GLTF_ANIMATION_H
#define GLTF_ANIMATION_H

#include <glm/gtx/quaternion.hpp>

#include "gltf/gltf_structures.h"


typedef enum {
    STEP,
    LINEAR,
    CUBICSPLINE
} Interpolation;

typedef enum {
    Translation,
    Rotation,
    Scale
} Path;

typedef struct {
    uint8_t path;

    int samplerIndex;
    int nodeIndex;
} Channel;

typedef struct {
    uint8_t interpolation;

    int numKeyFrames;
    float* timeStamps;
    glm::vec4* keyFrames;
} Sampler;

typedef struct {
    glm::vec4 in_tangent;
    glm::vec4 keyframe;
    glm::vec4 out_tangent;
} CubicKeyFrame;

typedef struct {
    int numChannels;
    Channel* channels;

    int numSamplers;
    Sampler* samplers;

} Animation;

typedef struct {
    char name[32];
    int id;

    int numChildren;
    Node* children;
} Node;

Channel* channels;

void apply_transformation(glm::mat4* matrix, glm::vec4 keyframe, Path path)
{
    switch (path) {
    case Translation: {
        (*matrix) = glm::translate((*matrix), glm::vec3(keyframe.x, keyframe.y, keyframe.x));
    } break;

    case Rotation: {

    } break;

    case Scale: {
        (*matrix) = glm::scale((*matrix), glm::vec3(keyframe.x, keyframe.y, keyframe.x));
    } break;

    default:
        break;
    }
}

void interpolate_linear(glm::mat4* matrix, glm::vec4 keyframe, glm::vec4 keyframe_next, Path path)
{
}

void interpolate_cubic(glm::mat4* matrix, CubicKeyFrame keyframe, CubicKeyFrame keyframe_next, Path path)
{
}

/*

glm::mat4 getBoneTransform(int nodeId, float currentTime, Animation animation)
{
    glm::mat4 boneTransform = glm::mat4(1.0f);

    int i;
    for (i = 0; i < animation.numChannels; ++i) {
        Channel channel = animation.channels[i];

        if (channel.nodeIndex = nodeId) {
            Sampler sampler = animation.samplers[channel.samplerIndex];

            switch (sampler.interpolation) {
            case STEP: {
                glm::vec4 keyframe = sampler.keyFrames[keyframeIndex];

                apply_transformation(boneTransform, keyframe, channel.path)
            } break;

            case LINEAR: {
                Vector4D keyframe_curr = sampler.keyFrames[keyframeIndex];
                Vector4D keyframe_next = sampler.keyFrames[(keyframeIndex + 1) % sampler.numKeyFrames];

                interpolate_linear();
            } break;

            case CUBICSPLINE: {
                /* size of keyframes = 3 * numKeyframes

                Keyframe keyFrame_in = sampler.keyFrames[(keyframeIndex - 1) % sampler.numKeyFrames];
                Keyframe keyFrame_out = sampler.keyFrames[(keyframeIndex + 1) % sampler.numKeyFrames];

                interpolate_cubic();
            } break;

            default:
                break
            }
        }
    }
}
   */ /*

void mark_joint_nodes(Node* nodes, int* joints, int numJoints)
{
    int i;
    for (i = 0; i < numJoints; ++i) {
        nodes[joints[i]].id = joints[i];
    }
}

void transform_node(Node* node, glm::mat4 parentTransform)
{
    glm::mat4 globalTransform;

    if (node->id >= 0) {
        glm::mat4 inverseBindMatrix = inverseBindMatrices[node->id];
        glm::mat4 boneTransform = getBoneTransform(node->id);

        globalTransform = boneTransform * parentTransform * inverseBindMatrix;

        FinalBoneMatrix[node->id] = globalTransform;

    } else {
        globalTransform = node->transform * parentTransform;
    }

    int i;
    for (i = 0; i < node->numChildren; ++i) {
        transform_node(node->children[i], globalTransform);
    }
}

*/

#endif
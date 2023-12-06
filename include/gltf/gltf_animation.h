/*-------------------------------------------------------------------------------\
gltf_animation.h

animation, skeletons
\-------------------------------------------------------------------------------*/
#ifndef GLTF_ANIMATION_H
#define GLTF_ANIMATION_H

#include <string.h>

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

float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;

    float midWayLength = animationTime - lastTimeStamp;

    float framesDiff = nextTimeStamp - lastTimeStamp;

    scaleFactor = midWayLength / framesDiff;

    return scaleFactor;
}

void interpolate_linear(glm::mat4* matrix, glm::vec4 keyframe_curr, glm::vec4 keyframe_next, float scaleFactor, Path path)
{
    switch (path) {
    case Translation: {
        glm::vec3 translate = glm::mix(keyframe_curr, keyframe_next, scaleFactor);

        (*matrix) = glm::translate((*matrix), glm::vec3(translate.x, translate.y, translate.x));
    } break;

    case Rotation: {

    } break;

    case Scale: {
        glm::vec4 scale = glm::mix(keyframe_curr, keyframe_next, scaleFactor);

        (*matrix) = glm::scale((*matrix), glm::vec3(scale.x, scale.y, scale.x));
    } break;

    default:
        break;
    }   
}

void interpolate_cubic(glm::mat4* matrix, CubicKeyFrame keyframe, CubicKeyFrame keyframe_next, Path path)
{
}

Path getPath(int path)
{
    switch (path) {
    case 0:
        return Translation;
    
    case 1:
        return Rotation;

    case 2:
        return Scale;

    default:
        break;
    }
}

glm::mat4 getBoneTransform(int nodeIndex, float currentTime, Animation animation)
{
    glm::mat4 boneTransform = glm::mat4(1.0f);

    int i;
    for (i = 0; i < animation.numChannels; ++i) 
    {
        Channel channel = animation.channels[i];

        if (channel.nodeIndex = nodeIndex) 
        {
            Sampler sampler = animation.samplers[channel.samplerIndex];

            int keyframeIndex = 0;

            int j;
            for (j = 0; j < sampler.numKeyFrames - 1; ++j)
            {
                if (currentTime < sampler.timeStamps[j + 1])
                    keyframeIndex = j;
            }

            switch (sampler.interpolation) {
            case STEP: {
                glm::vec4 keyframe = sampler.keyFrames[keyframeIndex];

                Path path = getPath(channel.path);

                apply_transformation(&boneTransform, keyframe, path);
            } break;

            case LINEAR: {
                glm::vec4 keyframe_curr = sampler.keyFrames[keyframeIndex];
                glm::vec4 keyframe_next = sampler.keyFrames[(keyframeIndex + 1) % sampler.numKeyFrames];

                float timestamp_curr = sampler.timeStamps[keyframeIndex];
                float timestamp_next = sampler.timeStamps[keyframeIndex];
                float scaleFactor = getScaleFactor(timestamp_curr, timestamp_next, currentTime);

                Path path = getPath(channel.path);

                interpolate_linear(&boneTransform, keyframe_curr, keyframe_next, scaleFactor, path);
            } break;

            case CUBICSPLINE: {
                /* size of keyframes = 3 * numKeyframes */

                glm::vec4 keyFrame_in = sampler.keyFrames[(keyframeIndex - 1) % sampler.numKeyFrames];
                glm::vec4 keyFrame_out = sampler.keyFrames[(keyframeIndex + 1) % sampler.numKeyFrames];

                //interpolate_cubic();
            } break;

            default:
                break;
            }
        }
    }

    return boneTransform;
}

typedef struct {
    unsigned int count;
    int componentSize;
    char* data;
    
} Buffer;

Buffer getBuffer(unsigned int accessorIndex, gltfAccessor* gltfAccessors, gltfBufferView* gltfBufferViews, char** allocatedBuffers)
{
    Buffer buffer;

    gltfAccessor accessor = gltfAccessors[accessorIndex];

    int count = accessor.m_Count;

    gltfBufferView bufferView = gltfBufferViews[accessor.m_BufferViewIndex];

    int bufferIndex = bufferView.m_BufferIndex;

    int offset = 0;

    if (bufferView.m_ByteOffset > 0) {
        offset += bufferView.m_ByteOffset;
    }

    if (accessor.m_ByteOffset > 0) {
        offset += accessor.m_ByteOffset;
    }

    char* offsetBuffer = allocatedBuffers[bufferIndex] + offset;

    int componentSize = component_size(accessor.m_ComponentType);

    buffer.count = count;
    buffer.componentSize = componentSize;
    buffer.data = offsetBuffer;

    return buffer;
}

Animation load_animation(gltfAnimation gltf_animation, gltfAccessor* gltfAccessors, gltfBufferView* gltfBufferViews, char** allocatedBuffers)
{
    Animation animation;

    animation.numChannels = gltf_animation.m_NumChannels;
    animation.numSamplers = gltf_animation.m_NumSamplers;

    int i;
    for (i = 0; i < gltf_animation.m_NumChannels; ++i)
    {
        gltfChannel gltf_channel = gltf_animation.m_Channels[i];

        if (strcmp(gltf_channel.m_Target.m_Path, "translation") == 0)
            animation.channels[i].path = 0;

        if (strcmp(gltf_channel.m_Target.m_Path, "rotation") == 0)
            animation.channels[i].path = 1;

        if (strcmp(gltf_channel.m_Target.m_Path, "scale") == 0)
            animation.channels[i].path = 2;

        animation.channels[i].samplerIndex = gltf_channel.m_AnimationSamplerIndex;
        animation.channels[i].nodeIndex = gltf_channel.m_Target.m_NodeIndex;
    }

    for (i = 0; i < gltf_animation.m_NumSamplers; ++i) {
        gltfAnimationSampler gltf_sampler = gltf_animation.m_Samplers[i];

        if (strcmp(gltf_sampler.m_Interpolation, "STEP") == 0)
            animation.samplers[i].interpolation = 0;

        if (strcmp(gltf_sampler.m_Interpolation, "LINEAR") == 0)
            animation.samplers[i].interpolation = 1;

        if (strcmp(gltf_sampler.m_Interpolation, "CUBICSPLINE") == 0)
            animation.samplers[i].interpolation = 2;


        Buffer input_buffer = getBuffer(gltf_sampler.m_Input, gltfAccessors, gltfBufferViews, allocatedBuffers);

        int numKeyframes = input_buffer.count;

        animation.samplers[i].numKeyFrames = numKeyframes;
        animation.samplers[i].timeStamps = (float*)malloc(numKeyframes * sizeof(float));

        for (int k = 0; k < numKeyframes; ++k) {
            float x;
            memcpy(&x, input_buffer.data + k * input_buffer.componentSize, input_buffer.componentSize);

            animation.samplers[i].timeStamps[k] = x;
        }
        
        Buffer output_buffer = getBuffer(gltf_sampler.m_Output, gltfAccessors, gltfBufferViews, allocatedBuffers);
        
        animation.samplers[i].keyFrames = (glm::vec4*)malloc(numKeyframes * sizeof(glm::vec4));

        int size = gltf_get_size(gltfAccessors[gltf_sampler.m_Output].m_Type);

        if (size == 3)
        {
            for (int k = 0; k < numKeyframes; ++k) {
                float x, y, z;
                memcpy(&x, output_buffer.data + k * output_buffer.componentSize, output_buffer.componentSize);
                memcpy(&y, output_buffer.data + k * output_buffer.componentSize + 4, output_buffer.componentSize);
                memcpy(&z, output_buffer.data + k * output_buffer.componentSize + 8, output_buffer.componentSize);
                
                animation.samplers[i].keyFrames[k].x = x;
                animation.samplers[i].keyFrames[k].y = y;
                animation.samplers[i].keyFrames[k].z = z;
                animation.samplers[i].keyFrames[k].w = 0.0f;
            }
        } else /* size == 4 */ {
            for (int k = 0; k < numKeyframes; ++k) {
                float x, y, z, w;
                memcpy(&x, output_buffer.data + k * output_buffer.componentSize, output_buffer.componentSize);
                memcpy(&y, output_buffer.data + k * output_buffer.componentSize + 4, output_buffer.componentSize);
                memcpy(&z, output_buffer.data + k * output_buffer.componentSize + 8, output_buffer.componentSize);
                memcpy(&w, output_buffer.data + k * output_buffer.componentSize + 12, output_buffer.componentSize);

                animation.samplers[i].keyFrames[k].x = x;
                animation.samplers[i].keyFrames[k].y = y;
                animation.samplers[i].keyFrames[k].z = z;
                animation.samplers[i].keyFrames[k].w = w;
            }
        }

        
        
    }
}











/*

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
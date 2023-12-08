/*-------------------------------------------------------------------------------\
gltf_animation.h

animation, skeletons
\-------------------------------------------------------------------------------*/
#ifndef GLTF_ANIMATION_H
#define GLTF_ANIMATION_H

#include <string.h>

#include <glm/gtx/quaternion.hpp>

#include "gltf/gltf_structures.h"


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

Animation globalAnimation;
float m_CurrentTime;

glm::mat4 getBoneTransform(int nodeIndex, float currentTime, Animation animation);

glm::mat4 animateBone(int nodeIndex, float dt, Animation animation)
{
    int numKeyframes = animation.samplers[0].numKeyFrames;
    float duration = animation.samplers[0].timeStamps[numKeyframes];

    m_CurrentTime += dt;
    m_CurrentTime = fmod(m_CurrentTime, duration);

    return getBoneTransform(nodeIndex, m_CurrentTime, animation);
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

    int numChannels = gltf_animation.m_NumChannels;
    int numSamplers = gltf_animation.m_NumSamplers;

    animation.numChannels = numChannels;
    animation.numSamplers = numSamplers;

    animation.channels = (Channel*)malloc(numChannels * sizeof(Channel));
    animation.samplers = (Sampler*)malloc(numSamplers * sizeof(Sampler));

    int i;
    for (i = 0; i < numChannels; ++i)
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

    for (i = 0; i < numSamplers; ++i) {
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
        
        animation.samplers[i].keyFrames = (glm::vec4*)malloc(output_buffer.count * sizeof(glm::vec4));

        int size = gltf_get_size(gltfAccessors[gltf_sampler.m_Output].m_Type);

        if (size == 3)
        {
            int count = 0;
            for (int j = 0; j < output_buffer.count; j++) 
            {
                for (int k = 0; k < size; k++)
                {
                    float f;
                    memcpy(&f, output_buffer.data + count * 4, 4);
                    animation.samplers[i].keyFrames[j][k] = f;

                    count++;
                }

                animation.samplers[i].keyFrames[j][3] = 0.0f;
            }

        } else /* size == 4 */ {
            int count = 0;
            for (int j = 0; j < output_buffer.count; j++) {
                for (int k = 0; k < size; k++) {
                    float f;
                    memcpy(&f, output_buffer.data + count * 4, 4);
                    animation.samplers[i].keyFrames[j][k] = f;

                    count++;
                }
            }
        }
    }

    return animation;
}

Skin load_skin(gltfSkin gltf_skin, gltfAccessor* gltfAccessors, gltfBufferView* gltfBufferViews, char** allocatedBuffers)
{
    Skin skin;

    int numJoints = gltf_skin.m_NumJoints;

    skin.numJoints = numJoints;
    skin.finalBoneMatrices = (glm::mat4*)malloc(numJoints * sizeof(glm::mat4));
    skin.inverseBindMatrices = (glm::mat4*)malloc(numJoints * sizeof(glm::mat4));

     Buffer buffer = getBuffer(gltf_skin.m_InverseBindMatrices, gltfAccessors, gltfBufferViews, allocatedBuffers);

    skin.inverseBindMatrices[i] = (glm::mat4*)malloc(buffer.count * sizeof(glm::mat4));

    int size = gltf_get_size(gltfAccessors[gltf_skin.m_InverseBindMatrices].m_Type);

    for (int i = 0; i < numJoints; ++i) {

        int count = 0;
        for (int j = 0; j < buffer.count; j++) {
            for (int k = 0; k < size; k++) {
                float f;
                memcpy(&f, buffer.data + count * 4, 4);
                animation.samplers[i].keyFrames[j][k] = f;

                count++;
            }
        }



        skin.inverseBindMatrices[i] = gltf_skin.m_InverseBindMatrices[i];
    }

    return skin;
}


void printChannel(Channel* channel)
{
    printf("Channel:\n");
    printf("  Path: %u\n", channel->path);
    printf("  Sampler Index: %d\n", channel->samplerIndex);
    printf("  Node Index: %d\n", channel->nodeIndex);
}

void printSampler(Sampler* sampler)
{
    printf("Sampler:\n");
    printf("  Interpolation: %u\n", sampler->interpolation);
    printf("  Number of Key Frames: %d\n", sampler->numKeyFrames);

    printf("  Time Stamps:\n");
    for (int i = 0; i < sampler->numKeyFrames; ++i) {
        printf("    %f\n", sampler->timeStamps[i]);
    }

    printf("  Key Frames:\n");
    int keyFrameSize = (sampler->interpolation == 2) ? 3 : 1;

    if (keyFrameSize == 3)
    {
        for (int i = 0; i < sampler->numKeyFrames; ++i) {
            // Assuming glm::vec4 has x, y, z, and w components
            for (int j = 0; j < keyFrameSize; ++j) {
                printf("    (%f, %f, %f, %f)\n", sampler->keyFrames[i * keyFrameSize + j].x, sampler->keyFrames[i * keyFrameSize + j].y, sampler->keyFrames[i * keyFrameSize + j].z, sampler->keyFrames[i * keyFrameSize + j].w);
            }
        }
    } else {
        for (int i = 0; i < sampler->numKeyFrames; ++i) 
        { 
            printf("    (%f, %f, %f, %f)\n", sampler->keyFrames[i].x, sampler->keyFrames[i].y, sampler->keyFrames[i].z, sampler->keyFrames[i].w);
        }
    }
    
}


void printAnimation(Animation* animation)
{
    printf("Animation:\n");
    printf("Number of Channels: %d\n", animation->numChannels);
    for (int i = 0; i < animation->numChannels; ++i) {
        printChannel(&animation->channels[i]);
    }

    printf("Number of Samplers: %d\n", animation->numSamplers);
    for (int i = 0; i < animation->numSamplers; ++i) {
        printSampler(&animation->samplers[i]);
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
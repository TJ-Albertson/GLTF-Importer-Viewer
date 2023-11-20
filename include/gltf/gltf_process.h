/*-------------------------------------------------------------------------------\
gltf_process.h

all the functions for processing gltf objects
\-------------------------------------------------------------------------------*/
#ifndef GLTF_PROCESS_H
#define GLTF_PROCESS_H

#include <cjson/cJSON.h>
#include <string.h>

#include "gltf_structures.h"

gltfSkin gltf_process_skin(cJSON* skinNode)
{
    gltfSkin gltf_skin;

    if (cJSON_GetObjectItem(skinNode, "name")) {
        strncpy(gltf_skin.m_Name, cJSON_GetObjectItem(skinNode, "name")->valuestring, sizeof(gltf_skin.m_Name));
    } else {
        strncpy(gltf_skin.m_Name, "null\0", sizeof(gltf_skin.m_Name));
    }

    if (cJSON_GetObjectItem(skinNode, "inverseBindMatrices")) {
        gltf_skin.m_InverseBindMatrices = cJSON_GetObjectItem(skinNode, "inverseBindMatrices")->valueint;
    } else {
        gltf_skin.m_InverseBindMatrices = -1;
    }

    if (cJSON_GetObjectItem(skinNode, "joints")) {
        cJSON* joints = cJSON_GetObjectItem(skinNode, "joints");
        int numJoints = cJSON_GetArraySize(joints);
        int* gltf_joints = (int*)malloc(numJoints * sizeof(int));

        for (int i = 0; i < numJoints; ++i) {
            gltf_joints[i] = cJSON_GetArrayItem(joints, i)->valueint;
        }

        gltf_skin.m_NumJoints = numJoints;
        gltf_skin.m_Joints = gltf_joints;
    } else {
        gltf_skin.m_NumJoints = -1;
        gltf_skin.m_Joints = NULL;
    }

    return gltf_skin;
}

gltfAnimationTarget gltf_process_animation_target(cJSON* targetNode) 
{
    gltfAnimationTarget gltf_target;

    if (cJSON_GetObjectItem(targetNode, "node")) {
        gltf_target.m_NodeIndex = cJSON_GetObjectItem(targetNode, "input")->valueint;
    } else {
        gltf_target.m_NodeIndex = -1;
    }

    if (cJSON_GetObjectItem(targetNode, "path")) {
        strncpy(gltf_target.m_Path, cJSON_GetObjectItem(targetNode, "path")->valuestring, sizeof(gltf_target.m_Path));
    } else {
        strncpy(gltf_target.m_Path, "null\0", sizeof(gltf_target.m_Path));
    }

    return gltf_target;
}

gltfChannel gltf_process_channel(cJSON* channelNode)
{
    gltfChannel gltf_channel;

    if (cJSON_GetObjectItem(channelNode, "sampler")) {
        gltf_channel.m_AnimationSamplerIndex = cJSON_GetObjectItem(channelNode, "input")->valueint;
    } else {
        gltf_channel.m_AnimationSamplerIndex = -1;
    }

    if (cJSON_GetObjectItem(channelNode, "target")) {
        cJSON* target = cJSON_GetObjectItem(channelNode, "target");

        gltf_channel.m_Target = gltf_process_animation_target(target);
    }

    return gltf_channel;
}

gltfAnimationSampler gltf_process_animation_sampler(cJSON* samplerNode)
{
    gltfAnimationSampler gltf_sampler;

    if (cJSON_GetObjectItem(samplerNode, "input")) {
        gltf_sampler.m_Input = cJSON_GetObjectItem(samplerNode, "input")->valueint;
    } else {
        gltf_sampler.m_Input = -1;
    }

    if (cJSON_GetObjectItem(samplerNode, "interpolation")) {
        strncpy(gltf_sampler.m_Interpolation, cJSON_GetObjectItem(samplerNode, "interpolation")->valuestring, sizeof(gltf_sampler.m_Interpolation));
    } else {
        strncpy(gltf_sampler.m_Interpolation, "null\0", sizeof(gltf_sampler.m_Interpolation));
    }

    if (cJSON_GetObjectItem(samplerNode, "output")) {
        gltf_sampler.m_Output = cJSON_GetObjectItem(samplerNode, "output")->valueint;
    } else {
        gltf_sampler.m_Output = -1;
    }

    return gltf_sampler;
}

gltfAnimation gltf_process_animation(cJSON* animationNode)
{
    gltfAnimation gltf_animation;

    if (cJSON_GetObjectItem(animationNode, "name")) {
        strncpy(gltf_animation.m_Name, cJSON_GetObjectItem(animationNode, "name")->valuestring, sizeof(gltf_animation.m_Name));
    } else {
        strncpy(gltf_animation.m_Name, "null\0", sizeof(gltf_animation.m_Name));
    }

    if (cJSON_GetObjectItem(animationNode, "channels")) {

        cJSON* channels = cJSON_GetObjectItem(animationNode, "channels");
        int numChannels = cJSON_GetArraySize(channels);

        gltf_animation.m_NumChannels = numChannels;
        gltf_animation.m_Channels = (gltfChannel*)malloc(numChannels * sizeof(gltfChannel));

        for (int i = 0; i < numChannels; ++i) {
            cJSON* channel = cJSON_GetArrayItem(channels, i);

            gltf_animation.m_Channels[i] = gltf_process_channel(channel);
        }

    } else {
        fprintf(stderr, "Error: 'channels' field not found in animation.\n");
    }

    if (cJSON_GetObjectItem(animationNode, "samplers")) {

        cJSON* samplers = cJSON_GetObjectItem(animationNode, "samplers");
        int numSamplers = cJSON_GetArraySize(samplers);

        gltf_animation.m_NumSamplers = numSamplers;
        gltf_animation.m_Samplers = (gltfAnimationSampler*)malloc(numSamplers * sizeof(gltfAnimationSampler));

        for (int i = 0; i < numSamplers; ++i) {
            cJSON* sampler = cJSON_GetArrayItem(samplers, i);

            gltf_animation.m_Samplers[i] = gltf_process_animation_sampler(sampler);
        }

    } else {
        fprintf(stderr, "Error: 'samplers' field not found in animation.\n");
    }

    return gltf_animation;
}

gltfBuffer gltf_process_buffer(cJSON* bufferNode)
{
    gltfBuffer gltf_buffer;

    if (cJSON_GetObjectItem(bufferNode, "byteLength")) {
        gltf_buffer.m_ByteLength = cJSON_GetObjectItem(bufferNode, "byteLength")->valueint;
    } else {
        gltf_buffer.m_ByteLength = -1;
    }

    if (cJSON_GetObjectItem(bufferNode, "uri")) {
        strncpy(gltf_buffer.m_URI, cJSON_GetObjectItem(bufferNode, "uri")->valuestring, sizeof(gltf_buffer.m_URI));
    } else {
        strncpy(gltf_buffer.m_URI, "null", sizeof(gltf_buffer.m_URI));
    }

    return gltf_buffer;
}

gltfSampler gltf_process_sampler(cJSON* samplerNode)
{
    gltfSampler gltf_sampler;

    if (cJSON_GetObjectItem(samplerNode, "magFilter")) {
        gltf_sampler.m_MagFilter = cJSON_GetObjectItem(samplerNode, "magFilter")->valueint;
    } else {
        gltf_sampler.m_MagFilter = -1;
    }

    if (cJSON_GetObjectItem(samplerNode, "minFilter")) {
        gltf_sampler.m_MinFilter = cJSON_GetObjectItem(samplerNode, "minFilter")->valueint;
    } else {
        gltf_sampler.m_MinFilter = -1;
    }

    return gltf_sampler;
}

gltfBufferView gltf_process_bufferView(cJSON* node)
{
    gltfBufferView gltf_bufferView;

    if (cJSON_GetObjectItem(node, "buffer")) {
        gltf_bufferView.m_BufferIndex = cJSON_GetObjectItem(node, "buffer")->valueint;
    } else {
        gltf_bufferView.m_BufferIndex = -1;
    }

    if (cJSON_GetObjectItem(node, "byteLength")) {
        gltf_bufferView.m_ByteLength = cJSON_GetObjectItem(node, "byteLength")->valueint;
    } else {
        gltf_bufferView.m_ByteLength = -1;
    }

    if (cJSON_GetObjectItem(node, "byteOffset")) {
        gltf_bufferView.m_ByteOffset = cJSON_GetObjectItem(node, "byteOffset")->valueint;
    } else {
        gltf_bufferView.m_ByteOffset = -1;
    }

    if (cJSON_GetObjectItem(node, "target")) {
        gltf_bufferView.m_Target = cJSON_GetObjectItem(node, "target")->valueint;
    } else {
        gltf_bufferView.m_Target = -1;
    }

    return gltf_bufferView;
}

gltfAccessor gltf_process_acccessor(cJSON* node)
{
    gltfAccessor gltf_accessor;

    if (cJSON_GetObjectItem(node, "bufferView")) {
        gltf_accessor.m_BufferViewIndex = cJSON_GetObjectItem(node, "bufferView")->valueint;
    } else {
        gltf_accessor.m_BufferViewIndex = -1;
    }

    if (cJSON_GetObjectItem(node, "byteOffset")) {
        gltf_accessor.m_ByteOffset = cJSON_GetObjectItem(node, "byteOffset")->valueint;
    } else {
        gltf_accessor.m_ByteOffset = -1;
    }

    if (cJSON_GetObjectItem(node, "componentType")) {
        gltf_accessor.m_ComponentType = cJSON_GetObjectItem(node, "componentType")->valueint;
    } else {
        gltf_accessor.m_ComponentType = -1;
    }

    if (cJSON_GetObjectItem(node, "count")) {
        gltf_accessor.m_Count = cJSON_GetObjectItem(node, "count")->valueint;
    } else {
        gltf_accessor.m_Count = -1;
    }

    if (cJSON_GetObjectItem(node, "type")) {
        strncpy(gltf_accessor.m_Type, cJSON_GetObjectItem(node, "type")->valuestring, sizeof(gltf_accessor.m_Type));
    } else {
        strncpy(gltf_accessor.m_Type, "null\0", sizeof(gltf_accessor.m_Type));
    }

    return gltf_accessor;
}

gltfImage gltf_process_image(cJSON* imageNode)
{

    gltfImage gltf_image;

    if (cJSON_GetObjectItem(imageNode, "name")) {
        strncpy(gltf_image.m_Name, cJSON_GetObjectItem(imageNode, "name")->valuestring, sizeof(gltf_image.m_Name));
    } else {
        strncpy(gltf_image.m_Name, "null\0", sizeof(gltf_image.m_Name));
    }

    if (cJSON_GetObjectItem(imageNode, "mimeType")) {
        strncpy(gltf_image.m_MimeType, cJSON_GetObjectItem(imageNode, "mimeType")->valuestring, sizeof(gltf_image.m_MimeType));
    } else {
        strncpy(gltf_image.m_MimeType, "null\0", sizeof(gltf_image.m_MimeType));
    }

    if (cJSON_GetObjectItem(imageNode, "uri")) {
        strncpy(gltf_image.m_URI, cJSON_GetObjectItem(imageNode, "uri")->valuestring, sizeof(gltf_image.m_URI));
    } else {
        strncpy(gltf_image.m_URI, "null\0", sizeof(gltf_image.m_URI));
    }

    return gltf_image;
}

gltfTexture gltf_process_texture(cJSON* textureNode)
{
    gltfTexture gltf_texture;

    if (cJSON_GetObjectItem(textureNode, "sampler")) {
        gltf_texture.m_SamplerIndex = cJSON_GetObjectItem(textureNode, "sampler")->valueint;
    } else {
        gltf_texture.m_SamplerIndex = -1;
    }

    if (cJSON_GetObjectItem(textureNode, "source")) {
        gltf_texture.m_SourceIndex = cJSON_GetObjectItem(textureNode, "source")->valueint;
    } else {
        gltf_texture.m_SourceIndex = -1;
    }

    return gltf_texture;
}

gltfPrimitiveAttributes gltf_process_primitive_attributes(cJSON* primAttr)
{
    gltfPrimitiveAttributes gltf_primitive_attr;

    if (cJSON_GetObjectItem(primAttr, "POSITION")) {
        gltf_primitive_attr.m_PositionIndex = cJSON_GetObjectItem(primAttr, "POSITION")->valueint;
    } else {
        gltf_primitive_attr.m_PositionIndex = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "NORMAL")) {
        gltf_primitive_attr.m_NormalIndex = cJSON_GetObjectItem(primAttr, "NORMAL")->valueint;
    } else {
        gltf_primitive_attr.m_NormalIndex = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "TANGENT")) {
        gltf_primitive_attr.m_TangentIndex = cJSON_GetObjectItem(primAttr, "TANGENT")->valueint;
    } else {
        gltf_primitive_attr.m_TangentIndex = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "TEXCOORD_0")) {
        gltf_primitive_attr.m_TexCoord_0_Index = cJSON_GetObjectItem(primAttr, "TEXCOORD_0")->valueint;
    } else {
        gltf_primitive_attr.m_TexCoord_0_Index = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "TEXCOORD_1")) {
        gltf_primitive_attr.m_TexCoord_1_Index = cJSON_GetObjectItem(primAttr, "TEXCOORD_1")->valueint;
    } else {
        gltf_primitive_attr.m_TexCoord_1_Index = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "COLOR_0")) {
        gltf_primitive_attr.m_Color_0_Index = cJSON_GetObjectItem(primAttr, "COLOR_0")->valueint;
    } else {
        gltf_primitive_attr.m_Color_0_Index = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "JOINTS_0")) {
        gltf_primitive_attr.m_Joints_0_Index = cJSON_GetObjectItem(primAttr, "JOINTS_0")->valueint;
    } else {
        gltf_primitive_attr.m_Joints_0_Index = -1;
    }

    if (cJSON_GetObjectItem(primAttr, "WEIGHTS_0")) {
        gltf_primitive_attr.m_Weights_0_Index = cJSON_GetObjectItem(primAttr, "WEIGHTS_0")->valueint;
    } else {
        gltf_primitive_attr.m_Weights_0_Index = -1;
    }

    return gltf_primitive_attr;
}

gltfPrimitive gltf_process_primitive(cJSON* primNode)
{
    gltfPrimitive gltf_primitive;

    if (cJSON_GetObjectItem(primNode, "attributes")) {
        cJSON* attributes = cJSON_GetObjectItem(primNode, "attributes");

        gltf_primitive.m_Attributes = gltf_process_primitive_attributes(attributes);

    } else {
        fprintf(stderr, "Error: 'attributes' field not found in Primitive.\n");
    }

    if (cJSON_GetObjectItem(primNode, "indices")) {
        gltf_primitive.m_IndicesIndex = cJSON_GetObjectItem(primNode, "indices")->valueint;
    } else {
        gltf_primitive.m_IndicesIndex = -1;
    }

    if (cJSON_GetObjectItem(primNode, "material")) {
        gltf_primitive.m_MaterialIndex = cJSON_GetObjectItem(primNode, "material")->valueint;
    } else {
        gltf_primitive.m_MaterialIndex = -1;
    }

    if (cJSON_GetObjectItem(primNode, "mode")) {
        gltf_primitive.m_Mode = cJSON_GetObjectItem(primNode, "mode")->valueint;
    } else {
        gltf_primitive.m_Mode = -1;
    }

    return gltf_primitive;
}

gltfMesh gltf_process_mesh(cJSON* meshNode)
{
    gltfMesh gltf_mesh;

    if (cJSON_GetObjectItem(meshNode, "name")) {
        strncpy(gltf_mesh.m_Name, cJSON_GetObjectItem(meshNode, "name")->valuestring, sizeof(gltf_mesh.m_Name));
    } else {
        fprintf(stderr, "Error: 'name' field not found in Mesh.\n");
    }

    if (cJSON_GetObjectItem(meshNode, "primitives")) {

        cJSON* primitives = cJSON_GetObjectItem(meshNode, "primitives");
        int numPrimitives = cJSON_GetArraySize(primitives);

        gltf_mesh.m_NumPrimitives = numPrimitives;
        gltf_mesh.m_Primitives = (gltfPrimitive*)malloc(numPrimitives * sizeof(gltfPrimitive));

        for (int i = 0; i < numPrimitives; ++i) {
            cJSON* primitive = cJSON_GetArrayItem(primitives, i);

            gltf_mesh.m_Primitives[i] = gltf_process_primitive(primitive);
        }

    } else {
        fprintf(stderr, "Error: 'primitives' field not found in Mesh.\n");
    }

    return gltf_mesh;
}

/*
 *   Proccess material start
 */
gltfMetallicRoughness* gltf_process_metallic_roughness(cJSON* pbrNode)
{
    gltfMetallicRoughness* gltf_metal_rough = (gltfMetallicRoughness*)malloc(sizeof(gltfMetallicRoughness));

    if (cJSON_GetObjectItem(pbrNode, "baseColorTexture")) {
        cJSON* baseColorTextureNode = cJSON_GetObjectItem(pbrNode, "baseColorTexture");

        gltfBaseColorTexture* gltf_base_color_texture = (gltfBaseColorTexture*)malloc(sizeof(gltfBaseColorTexture));

        if (cJSON_GetObjectItem(baseColorTextureNode, "index")) {
            gltf_base_color_texture->m_Index = cJSON_GetObjectItem(baseColorTextureNode, "index")->valueint;
        } else {
            gltf_base_color_texture->m_Index = -1;
        }

        if (cJSON_GetObjectItem(baseColorTextureNode, "texCoord")) {
            gltf_base_color_texture->m_TexCoord = cJSON_GetObjectItem(baseColorTextureNode, "texCoord")->valueint;
        } else {
            gltf_base_color_texture->m_TexCoord = -1;
        }

        gltf_metal_rough->m_BaseColorTexture = gltf_base_color_texture;

    } else {
        gltf_metal_rough->m_BaseColorTexture = NULL;
    }

    if (cJSON_GetObjectItem(pbrNode, "baseColorFactor")) {
        cJSON* baseColorFactor = cJSON_GetObjectItem(pbrNode, "baseColorFactor");

        float x = cJSON_GetArrayItem(baseColorFactor, 0)->valuedouble;
        float y = cJSON_GetArrayItem(baseColorFactor, 1)->valuedouble;
        float z = cJSON_GetArrayItem(baseColorFactor, 2)->valuedouble;
        float w = cJSON_GetArrayItem(baseColorFactor, 3)->valuedouble;

        gltf_metal_rough->m_BaseColorFactor = glm::vec4(x, y, z, w);

    } else {
        gltf_metal_rough->m_BaseColorFactor = glm::vec4(1.0f);
    }

    if (cJSON_GetObjectItem(pbrNode, "metallicRoughnessTexture")) {

        cJSON* metallicRoughnessTexture = cJSON_GetObjectItem(pbrNode, "metallicRoughnessTexture");

        gltfMetallicRoughnessTexture* gltf_metal_rough_texture = (gltfMetallicRoughnessTexture*)malloc(sizeof(gltfMetallicRoughnessTexture));

        if (cJSON_GetObjectItem(metallicRoughnessTexture, "index")) {
            gltf_metal_rough_texture->m_Index = cJSON_GetObjectItem(metallicRoughnessTexture, "index")->valueint;
        } else {
            gltf_metal_rough_texture->m_Index = -1;
        }

        if (cJSON_GetObjectItem(metallicRoughnessTexture, "texCoord")) {
            gltf_metal_rough_texture->m_TexCoord = cJSON_GetObjectItem(metallicRoughnessTexture, "texCoord")->valueint;
        } else {
            gltf_metal_rough_texture->m_TexCoord = -1;
        }

        gltf_metal_rough->m_MetallicRoughnessTexture = gltf_metal_rough_texture;

    } else {
        gltf_metal_rough->m_MetallicRoughnessTexture = NULL;
    }

    if (cJSON_GetObjectItem(pbrNode, "metallicFactor")) {
        gltf_metal_rough->m_MetallicFactor = cJSON_GetObjectItem(pbrNode, "metallicFactor")->valuedouble;
    } else {
        gltf_metal_rough->m_MetallicFactor = 0.0f;
    }

    if (cJSON_GetObjectItem(pbrNode, "roughnessFactor")) {
        gltf_metal_rough->m_RoughnessFactor = cJSON_GetObjectItem(pbrNode, "roughnessFactor")->valuedouble;
    } else {
        gltf_metal_rough->m_RoughnessFactor = 0.0f;
    }

    return gltf_metal_rough;
}

gltfNormalTexture* gltf_process_normalTexture(cJSON* node)
{
    gltfNormalTexture* gltf_normal_texture = (gltfNormalTexture*)malloc(sizeof(gltfNormalTexture));

    if (cJSON_GetObjectItem(node, "scale")) {
        gltf_normal_texture->m_Scale = cJSON_GetObjectItem(node, "scale")->valuedouble;
    } else {
        gltf_normal_texture->m_Scale = -1;
    }

    if (cJSON_GetObjectItem(node, "index")) {
        gltf_normal_texture->m_Index = cJSON_GetObjectItem(node, "index")->valueint;
    } else {
        gltf_normal_texture->m_Index = -1;
    }

    if (cJSON_GetObjectItem(node, "texCoord")) {
        gltf_normal_texture->m_TexCoord = cJSON_GetObjectItem(node, "texCoord")->valueint;
    } else {
        gltf_normal_texture->m_TexCoord = -1;
    }

    return gltf_normal_texture;
}

gltfOcclusionTexture* gltf_process_occlusionTexture(cJSON* node)
{
    gltfOcclusionTexture* gltf_occlusion_texture = (gltfOcclusionTexture*)malloc(sizeof(gltfOcclusionTexture));

    if (cJSON_GetObjectItem(node, "scale")) {
        gltf_occlusion_texture->m_Strength = cJSON_GetObjectItem(node, "scale")->valuedouble;
    } else {
        gltf_occlusion_texture->m_Strength = -1;
    }

    if (cJSON_GetObjectItem(node, "index")) {
        gltf_occlusion_texture->m_Index = cJSON_GetObjectItem(node, "index")->valueint;
    } else {
        gltf_occlusion_texture->m_Index = -1;
    }

    if (cJSON_GetObjectItem(node, "texCoord")) {
        gltf_occlusion_texture->m_TexCoord = cJSON_GetObjectItem(node, "texCoord")->valueint;
    } else {
        gltf_occlusion_texture->m_TexCoord = -1;
    }

    return gltf_occlusion_texture;
}

gltfEmissiveTexture* gltf_process_emissiveTexture(cJSON* node)
{
    gltfEmissiveTexture* gltf_emissive_texture = (gltfEmissiveTexture*)malloc(sizeof(gltfEmissiveTexture));

    if (cJSON_GetObjectItem(node, "index")) {
        gltf_emissive_texture->m_Index = cJSON_GetObjectItem(node, "index")->valueint;
    } else {
        gltf_emissive_texture->m_Index = -1;
    }

    if (cJSON_GetObjectItem(node, "texCoord")) {
        gltf_emissive_texture->m_TexCoord = cJSON_GetObjectItem(node, "texCoord")->valueint;
    } else {
        gltf_emissive_texture->m_TexCoord = -1;
    }

    return gltf_emissive_texture;
}

gltfMaterial gltf_process_material(cJSON* materialNode)
{
    gltfMaterial gltf_material;

    if (cJSON_GetObjectItem(materialNode, "name")) {
        strncpy(gltf_material.m_Name, cJSON_GetObjectItem(materialNode, "name")->valuestring, sizeof(gltf_material.m_Name));
    } else {
        fprintf(stderr, "Error: 'name' field not found in Material.\n");
    }

    if (cJSON_GetObjectItem(materialNode, "pbrMetallicRoughness")) {
        cJSON* pbrNode = cJSON_GetObjectItem(materialNode, "pbrMetallicRoughness");

        gltf_material.m_MetalicRoughness = gltf_process_metallic_roughness(pbrNode);

    } else {
        fprintf(stderr, "Error: 'pbrMetallicRoughness' field not found in Material.\n");
    }

    if (cJSON_GetObjectItem(materialNode, "normalTexture")) {
        cJSON* normalTexture = cJSON_GetObjectItem(materialNode, "normalTexture");

        gltf_material.m_NormalTexture = gltf_process_normalTexture(normalTexture);

    } else {
        gltf_material.m_NormalTexture = NULL;
    }

    if (cJSON_GetObjectItem(materialNode, "occlusionTexture")) {
        cJSON* occlusionTexture = cJSON_GetObjectItem(materialNode, "occlusionTexture");

        gltf_material.m_OcclusionTexture = gltf_process_occlusionTexture(occlusionTexture);

    } else {
        gltf_material.m_OcclusionTexture = NULL;
    }

    if (cJSON_GetObjectItem(materialNode, "emissiveTexture")) {
        cJSON* emissiveTexture = cJSON_GetObjectItem(materialNode, "emissiveTexture");

        gltf_material.m_EmissiveTexture = gltf_process_emissiveTexture(emissiveTexture);

    } else {
        gltf_material.m_EmissiveTexture = NULL;
    }

    if (cJSON_GetObjectItem(materialNode, "emissiveFactor")) {
        cJSON* emmissiveFactor = cJSON_GetObjectItem(materialNode, "emissiveFactor");

        float x = cJSON_GetArrayItem(emmissiveFactor, 0)->valuedouble;
        float y = cJSON_GetArrayItem(emmissiveFactor, 1)->valuedouble;
        float z = cJSON_GetArrayItem(emmissiveFactor, 2)->valuedouble;

        gltf_material.m_EmissiveFactor = glm::vec3(x, y, z);

    } else {
        gltf_material.m_EmissiveFactor = glm::vec3(0.0f);
    }

    return gltf_material;
}

gltfNode gltf_process_node(cJSON* node)
{
    gltfNode gltf_node;

    if (cJSON_GetObjectItem(node, "name")) {
        strncpy(gltf_node.m_Name, cJSON_GetObjectItem(node, "name")->valuestring, sizeof(gltf_node.m_Name));
    } else {
        fprintf(stderr, "Error: 'name' field not found in Node.\n");
    }

    if (cJSON_GetObjectItem(node, "mesh")) {
        gltf_node.m_MeshIndex = cJSON_GetObjectItem(node, "mesh")->valueint;
    } else {
        gltf_node.m_MeshIndex = -1;
    }

    if (cJSON_GetObjectItem(node, "camera")) {
        gltf_node.m_CameraIndex = cJSON_GetObjectItem(node, "camera")->valueint;
    } else {
        gltf_node.m_CameraIndex = -1;
    }

    if (cJSON_GetObjectItem(node, "skin")) {
        gltf_node.m_SkinIndex = cJSON_GetObjectItem(node, "skin")->valueint;
    } else {
        gltf_node.m_SkinIndex = -1;
    }

    return gltf_node;
}

gltfNode gltf_traverse_node(cJSON* node)
{
    gltfNode gltf_node = gltf_process_node(node);

    if (cJSON_GetObjectItem(node, "children")) {
        cJSON* children = cJSON_GetObjectItem(node, "children");
        int numChildren = cJSON_GetArraySize(children);

        gltf_node.m_NumChildren = numChildren;
        gltf_node.m_Children = (gltfNode*)malloc(numChildren * sizeof(gltfNode));

        for (int i = 0; i < numChildren; ++i) {
            cJSON* child = cJSON_GetArrayItem(children, i);

            gltf_node.m_Children[i] = gltf_traverse_node(child);
        }

    } else {
        gltf_node.m_NumChildren = -1;
    }

    return gltf_node;
}


#endif
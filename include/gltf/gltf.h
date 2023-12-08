/*-------------------------------------------------------------------------------\
gltf.h

gltf file importer
\-------------------------------------------------------------------------------*/
#ifndef GLTF_H
#define GLTF_H

#include <glm/gtc/matrix_transform.hpp>

#include <cjson/cJSON.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gltf/gltf_gl.h"
#include "gltf/gltf_memory.h"
#include "gltf/gltf_print.h"
#include "gltf/gltf_process.h"
#include "gltf/gltf_structures.h"
#include "gltf/gltf_animation.h"

#include <model.h>

char* gltf_load_file(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

// Checks gltf version number and extensions
void gltf_pre_check(cJSON* root)
{
    cJSON* asset = cJSON_GetObjectItem(root, "asset");
    if (asset != NULL) {
        cJSON* generator = cJSON_GetObjectItem(asset, "generator");
        if (generator != NULL && cJSON_IsString(generator)) {
            printf("generator: %s\n", generator->valuestring);
        } else {
            fprintf(stderr, "Error: 'generator' field not found or not a string.\n");
        }

        cJSON* version = cJSON_GetObjectItem(asset, "version");
        if (version != NULL && cJSON_IsString(version)) {
            printf("version: %s\n", version->valuestring);
        } else {
            fprintf(stderr, "Error: 'version' field not found or not a string.\n");
        }
    } else {
        fprintf(stderr, "Error: 'asset' field not found.\n");
    }

    cJSON* extensionsUsed = cJSON_GetObjectItem(root, "extensionsUsed");
    if (extensionsUsed != NULL) {
        printf("extensionsUsed:\n");
        int numExtensions = cJSON_GetArraySize(extensionsUsed);

        for (int i = 0; i < numExtensions; ++i) {
            cJSON* extension = cJSON_GetArrayItem(extensionsUsed, i);
            printf("    %s\n", extension->valuestring);
        }

    } else {
        fprintf(stderr, "Error: 'extensionsUsed' field not found.\n");
    }
}

int gltf_parse(const char* jsonString, Model& model)
{
    cJSON* root = cJSON_Parse(jsonString);
    if (!root) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return 0;
    }

    gltf_pre_check(root);

    int defaultSceneIndex = -1;
    if (cJSON_GetObjectItem(root, "scene")) {
        defaultSceneIndex = cJSON_GetObjectItem(root, "scene")->valueint;
    }

    // "scenes": []
    cJSON* scenes = cJSON_GetObjectItem(root, "scenes");

    cJSON* defaultScene = cJSON_GetArrayItem(scenes, defaultSceneIndex);
    cJSON* name = cJSON_GetObjectItem(defaultScene, "name");

    cJSON* rootNodeIndexes = cJSON_GetObjectItem(defaultScene, "nodes");

    int numRootNodes = cJSON_GetArraySize(rootNodeIndexes);

    gltfScene gltf_scene;
    strncpy(gltf_scene.m_Name, name->valuestring, sizeof(gltf_scene.m_Name));
    gltf_scene.m_NumNodes = numRootNodes;
    gltf_scene.m_Nodes = (gltfNode*)malloc(numRootNodes * sizeof(gltfNode));

    cJSON* nodes = cJSON_GetObjectItem(root, "nodes");

    for (int i = 0; i < numRootNodes; ++i) {
        int rootNodeIndex = cJSON_GetArrayItem(rootNodeIndexes, i)->valueint;
        cJSON* rootNode = cJSON_GetArrayItem(nodes, rootNodeIndex);
        gltfNode gltfNode = gltf_traverse_node(rootNode);
        gltf_scene.m_Nodes[i] = gltfNode;
    }

    // "nodes": []
    int numNodes = cJSON_GetArraySize(nodes);
    gltfNode* gltfNodes = (gltfNode*)malloc(numNodes * sizeof(gltfNode));

    for (int i = 0; i < numNodes; ++i) {
        cJSON* node = cJSON_GetArrayItem(nodes, i);
        gltfNodes[i] = gltf_process_node(node);
    }

    // "animations": []
    cJSON* animations = cJSON_GetObjectItem(root, "animations");
    int numAnimations = cJSON_GetArraySize(animations);
    gltfAnimation* gltfAnimations = (gltfAnimation*)malloc(numAnimations * sizeof(gltfAnimation));

    for (int i = 0; i < numAnimations; ++i) {
        cJSON* animation = cJSON_GetArrayItem(animations, i);
        gltfAnimations[i] = gltf_process_animation(animation);
    }

    // "skins": []
    cJSON* skins = cJSON_GetObjectItem(root, "skins");
    int numSkins = cJSON_GetArraySize(skins);
    gltfSkin* gltfSkins = (gltfSkin*)malloc(numSkins * sizeof(gltfSkin));

    for (int i = 0; i < numSkins; ++i) {
        cJSON* skin = cJSON_GetArrayItem(skins, i);
        gltfSkins[i] = gltf_process_skin(skin);
    }

    // "materials: []
    cJSON* materials = cJSON_GetObjectItem(root, "materials");
    int numMaterials = cJSON_GetArraySize(materials);
    gltfMaterial* gltfMaterials = (gltfMaterial*)malloc(numMaterials * sizeof(gltfMaterial));

    for (int i = 0; i < numMaterials; ++i) {
        cJSON* material = cJSON_GetArrayItem(materials, i);
        gltfMaterials[i] = gltf_process_material(material);
    }

    // "meshes": []
    cJSON* meshes = cJSON_GetObjectItem(root, "meshes");
    int numMeshes = cJSON_GetArraySize(meshes);
    gltfMesh* gltfMeshes = (gltfMesh*)malloc(numMeshes * sizeof(gltfMesh));

    for (int i = 0; i < numMeshes; ++i) {
        cJSON* mesh = cJSON_GetArrayItem(meshes, i);
        gltfMeshes[i] = gltf_process_mesh(mesh);
    }

    // "textures": []
    cJSON* textures = cJSON_GetObjectItem(root, "textures");
    int numTextures = cJSON_GetArraySize(textures);
    gltfTexture* gltfTextures = (gltfTexture*)malloc(numTextures * sizeof(gltfTexture));

    for (int i = 0; i < numTextures; ++i) {
        cJSON* texture = cJSON_GetArrayItem(textures, i);
        gltfTextures[i] = gltf_process_texture(texture);
    }

    // "images": []
    cJSON* images = cJSON_GetObjectItem(root, "images");
    int numImages = cJSON_GetArraySize(images);
    gltfImage* gltfImages = (gltfImage*)malloc(numImages * sizeof(gltfImage));

    for (int i = 0; i < numImages; ++i) {
        cJSON* image = cJSON_GetArrayItem(images, i);
        gltfImages[i] = gltf_process_image(image);
    }

    // "accessors": []
    cJSON* accessors = cJSON_GetObjectItem(root, "accessors");
    int numAccessors = cJSON_GetArraySize(accessors);
    gltfAccessor* gltfAccessors = (gltfAccessor*)malloc(numAccessors * sizeof(gltfAccessor));

    for (int i = 0; i < numAccessors; ++i) {
        cJSON* accesssor = cJSON_GetArrayItem(accessors, i);
        gltfAccessors[i] = gltf_process_acccessor(accesssor);
    }

    // "bufferViews: []
    cJSON* bufferViews = cJSON_GetObjectItem(root, "bufferViews");
    int numbufferViews = cJSON_GetArraySize(bufferViews);
    
    gltfBufferView* gltfBufferViews = (gltfBufferView*)malloc(numbufferViews * sizeof(gltfBufferView));

    for (int i = 0; i < numbufferViews; ++i) {
        cJSON* bufferView = cJSON_GetArrayItem(bufferViews, i);
        gltfBufferViews[i] = gltf_process_bufferView(bufferView);
    }

    // "samplers: []
    cJSON* samplers = cJSON_GetObjectItem(root, "samplers");
    int numSamplers = cJSON_GetArraySize(samplers);
    gltfSampler* gltfSamplers = (gltfSampler*)malloc(numSamplers * sizeof(gltfSampler));

    for (int i = 0; i < numSamplers; ++i) {
        cJSON* sampler = cJSON_GetArrayItem(samplers, i);
        gltfSamplers[i] = gltf_process_sampler(sampler);
    }

    // "buffers: []
    cJSON* buffers = cJSON_GetObjectItem(root, "buffers");
    int numBuffers = cJSON_GetArraySize(buffers);
    gltfBuffer* gltfBuffers = (gltfBuffer*)malloc(numBuffers * sizeof(gltfBuffer));

    for (int i = 0; i < numBuffers; ++i) {
        cJSON* buffer = cJSON_GetArrayItem(buffers, i);
        gltfBuffers[i] = gltf_process_buffer(buffer);
    }


    print_gltf_scene(gltf_scene);
    gltf_print_nodes(gltfNodes, numNodes);
    //print_gltf_materials(gltfMaterials, numMaterials);
    print_gltf_meshes(gltfMeshes, numMeshes);
    // print_gltf_textures(gltfTextures, numTextures);
    // print_gltf_images(gltfImages, numImages);
    // print_gltf_accessors(gltfAccessors, numAccessors);
    // print_gltf_bufferViews(gltfBufferViews, numbufferViews);
    // print_gltf_samplers(gltfSamplers, numSamplers);
    // print_gltf_buffers(gltfBuffers, numBuffers);
    gltf_print_animations(gltfAnimations, numAnimations);
    gltf_print_skins(gltfSkins, numSkins);

    // Reading binary files into buffers
    int totalBufferSize = 0;

    for (int i = 0; i < numBuffers; ++i) {
        gltfBuffer gltf_buffer = gltfBuffers[i];

        totalBufferSize += gltf_buffer.m_ByteLength;
    }

    char** allocatedBuffers = (char**)malloc(totalBufferSize);

    for (int i = 0; i < numBuffers; ++i) {
        gltfBuffer gltf_buffer = gltfBuffers[i];

        char* buffer = (char*)malloc(gltf_buffer.m_ByteLength);

        char relativePath[FILENAME_MAX];
        sprintf(relativePath, "%s/%s", currentDirectory, gltf_buffer.m_URI);

        printf("relativePath: %s\n", relativePath);

        FILE* ptr;
        ptr = fopen(relativePath, "rb");

        if (ptr == NULL) {
            fprintf(stderr, "Error: Unable to open file %s\n", relativePath);
            break;
        }

        fread(buffer, 1, gltf_buffer.m_ByteLength, ptr);

        allocatedBuffers[i] = buffer;

        fclose(ptr);
    }

    // Loading Meshes/Materials
    model.numMeshes = numMeshes;
    model.numMaterials = numMaterials;
    model.numAnimations = numAnimations;
    model.numRootNodes = numRootNodes;

    model.meshes = (Mesh*)malloc(numMeshes * sizeof(Mesh));
    model.materials = (Material*)malloc(numMaterials * sizeof(Material));
    model.animations = (Animation*)malloc(numAnimations * sizeof(Animation));

    //model.nodes = (Node*)malloc(numNodes * sizeof(Node));
    model.nodes = create_nodes(gltfNodes, numNodes);

    int numJoints = gltfSkins[0].m_NumJoints;

    mark_joint_nodes(model.nodes, gltfSkins[0].m_Joints, numJoints);

    model.skin.numJoints = numJoints;
    model.skin.finalBoneMatrices = (glm::mat4*)malloc(numJoints * sizeof(glm::mat4));
    model.skin.inverseBindMatrices = (glm::mat4*)malloc(numJoints * sizeof(glm::mat4));

    for (int i = 0; i < numJoints; ++i)
    {
        model.skin.inverseBindMatrices[i] = gltfSkins[0].m_InverseBindMatrices[i];
    }


    for (int i = 0; i < numMaterials; ++i) {
        model.materials[i] = gltf_load_material(gltfMaterials[i], gltfImages, gltfSamplers, gltfTextures);
    }

    for (int i = 0; i < numMeshes; ++i) {
        model.meshes[i] = gltf_load_mesh(gltfMeshes[i], gltfAccessors, gltfBufferViews, allocatedBuffers);
    }

    for (int i = 0; i < numAnimations; ++i)
    {
        model.animations[i] = load_animation(gltfAnimations[i], gltfAccessors, gltfBufferViews, allocatedBuffers);
    }

    if (numAnimations > 0) {
        globalAnimation = load_animation(gltfAnimations[0], gltfAccessors, gltfBufferViews, allocatedBuffers);
    }
  

    gltf_free_materials(gltfMaterials, numMaterials);
    gltf_free_meshes(gltfMeshes, numMeshes);

    free(gltfTextures);
    free(gltfImages);
    free(gltfAccessors);
    free(gltfBufferViews);
    free(gltfSamplers);
    free(gltfAnimations);
    free(gltfSkins);

    for (int i = 0; i < numBuffers; ++i) {
        free(allocatedBuffers[i]);
    }
    free(allocatedBuffers);
    
    cJSON_Delete(root);

    return 1;
}

int gltf_load_model(const char* filename, Model& model)
{
    char* jsonString = gltf_load_file(filename);

    const char* lastSlash = strrchr(filename, '/');

    if (lastSlash != NULL) {
        size_t length = lastSlash - filename;

        strncpy(currentDirectory, filename, length);
        currentDirectory[length] = '\0';
    } else {
        printf("No '/' found in filename.\n");
    }

    if (jsonString) {
        gltf_parse(jsonString, model);
        free(jsonString);
    }

    return 0;
}

#endif
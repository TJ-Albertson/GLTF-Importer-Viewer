/*-------------------------------------------------------------------------------\
gltf_structures.h

data structures
\-------------------------------------------------------------------------------*/
#ifndef GLTF_STRUCTURES_H
#define GLTF_STRUCTURES_H

char currentDirectory[FILENAME_MAX];

#include <glm/gtc/matrix_transform.hpp>
#include "gltf/gltf_structures.h"

typedef struct Material {
    unsigned int m_BaseColorTextureId;
    unsigned int m_MetallicTextureId;
    unsigned int m_RoughnessTextureId;
    unsigned int m_NormalTextureId;
    unsigned int m_OcclusionTextureId;
    unsigned int m_EmissiveTextureId;

    glm::vec3 m_EmissiveFactor;
    glm::vec4 m_BaseColorFactor;

    float m_MetallicFactor;
    float m_RoughnessFactor;
} Material;

enum PrimitiveAttribute {
    POSITION,
    NORMAL,
    TEXCOORD,
    JOINTS,
    WEIGHTS,
    TANGENT
};

enum ComponentType {
    signed_byte = 5120,
    unsigned_byte = 5121,
    signed_short = 5122,
    unsigned_short = 5123,
    unsigned_int = 5125,
    t_float = 5126
};

// takes componentType returns number of bytes
int component_size(int type)
{
    if (type == signed_byte || type == unsigned_byte) {
        return 1;
    }
    if (type == signed_short || type == unsigned_short) {
        return 2;
    }
    if (type == unsigned_int || type == t_float) {
        return 4;
    }
    return -1;
}

// returns size of attribute based on accessor.type
int gltf_get_size(const char* type)
{
    if (strcmp("SCALAR", type) == 0) {
        return 1;
    }
    if (strcmp("VEC2", type) == 0) {
        return 2;
    }
    if (strcmp("VEC3", type) == 0) {
        return 3;
    }
    if (strcmp("VEC4", type) == 0) {
        return 4;
    }
    if (strcmp("MAT2", type) == 0) {
        return 4;
    }
    if (strcmp("MAT3", type) == 0) {
        return 9;
    }
    if (strcmp("MAT4", type) == 0) {
        return 16;
    }
}

// Can be camera sk
typedef struct gltfNode {
    char m_Name[256];

    int m_MeshIndex;
    int m_CameraIndex;
    int m_SkinIndex;

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    int m_NumChildren;
    int* m_ChildrenIndexes;
} gltfNode;

typedef struct gltfScene {
    char m_Name[256];
    int m_NumNodes;
    gltfNode* m_Nodes;

    int* m_RootNodeIndices;
} gltfScene;

/*
 *   Mesh
 */
// Client implementations SHOULD support at least two texture coordinate sets, one vertex color, and one joints/weights set.
typedef struct gltfPrimitiveAttributes {
    int m_PositionIndex;
    int m_NormalIndex;
    int m_TangentIndex;

    int m_TexCoord_0_Index;
    int m_TexCoord_1_Index;

    int m_Color_0_Index;

    int m_Joints_0_Index;
    int m_Weights_0_Index;
} gltfPrimitiveAttributes;

// Indexes go to Accessors[Index]
typedef struct gltfPrimitive {
    gltfPrimitiveAttributes m_Attributes;

    int m_IndicesIndex;
    int m_MaterialIndex;
    int m_Mode;

} gltfPrimitive;

typedef struct gltfMesh {
    char m_Name[256];

    int m_NumPrimitives;
    gltfPrimitive* m_Primitives;

} gltfMesh;

/*
 *   Textures
 */
typedef struct gltfTexture {
    int m_SamplerIndex;

    // Image;
    int m_SourceIndex;
} gltfTexture;

typedef struct gltfImage {
    char m_Name[256];
    char m_MimeType[256];
    char m_URI[256];
};

typedef struct gltfSampler {
    int m_MagFilter;
    int m_MinFilter;
} gltfSampler;

/*
 *   Buffers
 */
typedef struct gltfBuffer {
    int m_ByteLength;
    char m_URI[256];
} gltfBuffer;

typedef struct gltfBufferView {
    int m_BufferIndex;
    int m_ByteOffset;
    int m_ByteLength;
    int m_ByteStride;
    int m_Target;
} gltfBufferView;

typedef struct gltfAccessor {
    int m_BufferViewIndex;
    int m_ByteOffset;
    int m_ComponentType;
    int m_Count;
    char m_Type[256];
} gltfAccessor;

/*
 *   Material
 */
typedef struct gltfBaseColorTexture {
    int m_Index;
    int m_TexCoord;
} gltfBaseColorTexture;

typedef struct gltfMetallicRoughnessTexture {
    int m_Index;
    int m_TexCoord;
} gltfMetallicRoughnessTexture;

typedef struct gltfMetallicRoughness {
    gltfBaseColorTexture* m_BaseColorTexture;
    glm::vec4 m_BaseColorFactor;
    gltfMetallicRoughnessTexture* m_MetallicRoughnessTexture;

    float m_MetallicFactor;
    float m_RoughnessFactor;
} gltfMetallicRoughness;

typedef struct gltfNormalTexture {
    float m_Scale;
    int m_Index;
    int m_TexCoord;
} gltfNormalTexture;

typedef struct gltfOcclusionTexture {
    float m_Strength;
    int m_Index;
    int m_TexCoord;
} gltfOcclusionTexture;

typedef struct gltfEmissiveTexture {
    int m_Index;
    int m_TexCoord;
} gltfEmissiveTexture;

typedef struct gltfMaterial {
    char m_Name[256];

    bool m_DoubleSided;

    gltfMetallicRoughness* m_MetalicRoughness;
    gltfNormalTexture* m_NormalTexture;
    gltfOcclusionTexture* m_OcclusionTexture;
    gltfEmissiveTexture* m_EmissiveTexture;

    glm::vec3 m_EmissiveFactor;
} gltfMaterial;

/*
 *   Animation
 */
typedef struct gltfAnimationTarget {
    int m_NodeIndex;
    //interpolation type
    char m_Path[256];
} gltfAnimationTarget;

typedef struct gltfChannel {
    int m_AnimationSamplerIndex;
    gltfAnimationTarget m_Target;
} gltfChannel;

typedef struct gltfAnimationSampler {
    //Accessor index of times for keyframes
    int m_Input;
    //Accessor index of transformations for keyframes
    int m_Output;
    char m_Interpolation[256];
};
typedef struct gltfAnimation {

    char m_Name[256];
    
    //Each channel targets a single type of transformation 
    int m_NumChannels;
    gltfChannel* m_Channels;

    int m_NumSamplers;
    gltfAnimationSampler* m_Samplers;

} gltfAnimation;

typedef struct gltfSkin {
    char m_Name[256];
    int m_InverseBindMatrices;

    int m_NumJoints;
    int* m_Joints;
} gltfSkin;





typedef struct gltfVertex {
    glm::vec3 m_Position;
    glm::vec3 m_Normal;
    glm::vec2 m_TexCoord_0;
} gltfVertex;

typedef struct TestMesh {
    gltfVertex* vertices;
    int numVertices;

    unsigned int* indices;
    int numIndices;
} TestMesh;

typedef struct gltfDraw {
    unsigned int VAO;
    int numIndices;
} gltfDraw;

















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
    int numChannels;
    Channel* channels;

    int numSamplers;
    Sampler* samplers;

} Animation;

typedef struct {
    glm::vec4 in_tangent;
    glm::vec4 keyframe;
    glm::vec4 out_tangent;
} CubicKeyFrame;

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
    int numJoints;
    glm::mat4* inverseBindMatrices;
    glm::mat4* finalBoneMatrices;
} Skin;


/*
typedef struct g_Mesh {
    unsigned int m_VAO;
    int m_NumIndices;
} g_Mesh;

typedef struct g_Model {
    gltfScene m_Scene;

    int m_NumMaterials;
    Material* m_Materials;

    int m_NumMeshes;
    g_Mesh* m_Meshes;
} g_Model;
*/
typedef struct {
    unsigned int VAO;
    int numIndices;

    unsigned int materialIndex;
} Primitive;

typedef struct {
    int numPrimitives;
    Primitive* primitives;
} Mesh;

typedef struct {
    int numRootNodes;
    int* rootNodeIndices;

    int numAnimations;
    Animation* animations;

    Skin skin;

    Node* nodes;

    int numMaterials;
    Material* materials;

    int numMeshes;
    Mesh* meshes;
} Model;



#endif // !1
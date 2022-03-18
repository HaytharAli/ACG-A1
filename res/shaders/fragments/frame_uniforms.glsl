// Stores uniforms that change every frame (ex: time, camera data)
layout (std140, binding = 0) uniform b_FrameLevelUniforms {
    // The camera's view matrix
    uniform mat4 u_View;
    // The camera's projection matrix
    uniform mat4 u_Projection;
    // The combined viewProject matrix
    uniform mat4 u_ViewProjection;
    // The position of the camera in world space
    uniform vec4  u_CamPos;
    // The time in seconds since the start of the application
    uniform float u_Time;    
    // The time in seconds since the last frame
    uniform float u_DeltaTime;
    // Lets us store up to 32 bool flags in one value
    uniform uint  u_Flags;
};

// Stores uniforms that change every object/instance
layout (std140, binding = 1) uniform b_InstanceLevelUniforms {
    // Complete MVP
    uniform mat4 u_ModelViewProjection;
    // Just the model transform, we'll do worldspace lighting
    uniform mat4 u_Model;
    // Normal Matrix for transforming normals
    uniform mat4 u_NormalMatrix;
};

#define FLAG_ENABLE_COLOR_CORRECTIONA (1 << 0)
#define FLAG_ENABLE_COLOR_CORRECTIONB (1 << 1)
#define FLAG_ENABLE_COLOR_CORRECTIONC (1 << 2)
#define FLAG_ENABLE_TOON_BULL (1 << 3)
#define FLAG_ENABLE_TOON_LIGHT (1 << 4)
#define FLAG_ENABLE_ASC (1 << 5)
#define FLAG_ENABLE_AS (1 << 6)
#define FLAG_ENABLE_S (1 << 7)
#define FLAG_ENABLE_A (1 << 8)
#define FLAG_ENABLE_D (1 << 9)

bool IsFlagSet(uint flag) {
    return (u_Flags & flag) != 0;
}
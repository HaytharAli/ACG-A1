#version 430

#include "../fragments/fs_common_inputs.glsl"

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

////////////////////////////////////////////////////////////////
/////////////// Instance Level Uniforms ////////////////////////
////////////////////////////////////////////////////////////////

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D Diffuse;
	float     Shininess;
	sampler1D toonTex;
};
// Create a uniform for the material
uniform Material u_Material;

////////////////////////////////////////////////////////////////
///////////// Application Level Uniforms ///////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/multiple_point_lights.glsl"

////////////////////////////////////////////////////////////////
/////////////// Frame Level Uniforms ///////////////////////////
////////////////////////////////////////////////////////////////

#include "../fragments/frame_uniforms.glsl"
#include "../fragments/color_correction.glsl"

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos.xyz, u_Material.Shininess);
	
	vec4 textureColor = texture(u_Material.Diffuse, inUVAlt);
	// Get the albedo from the diffuse / albedo map
	if(IsFlagSet(FLAG_ENABLE_ASC)){
		textureColor = texture(u_Material.Diffuse, inUV);
		lightAccumulation = inLight;
	}

	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;

	frag_color = vec4(ColorCorrect(mix(result, vec3(0.3647, 0.3412, 0.4), inFog)), textureColor.a);

	if(IsFlagSet(FLAG_ENABLE_D)){
		frag_color = textureColor;
	}
	if(IsFlagSet(FLAG_ENABLE_A)){
		frag_color = vec4((textureColor.rgb * vec3(0.1, 0.1, 0.1)), textureColor.a);
	}
}
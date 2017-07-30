shader "Standard" {
    properties {
        _ALBEDO_SOURCE("Albedo") : enum "Color;Texture" = "0" (shaderDefine)
        albedoColor("Albedo Color") : color3 = "1 1 1"
        albedoMap("Albedo Map") : object TextureAsset = "_whiteTexture"
        _WRAPPED_DIFFUSE("Wrapped Diffuse") : bool = "false" (shaderDefine)
        wrappedDiffuse("Wrapped") : float range 0 1.0 0.001 = "0.5"
        _METALLIC_SOURCE("Metallic") : enum "Scale;Texture(R)" = "0" (shaderDefine)
        metallicMap("Metallic Map") : object TextureAsset = "_whiteTexture"
        metallicScale("Metallic Scale") : float range 0 1.0 0.01 = "1.0"
        _ROUGHNESS_SOURCE("Roughness") : enum "Scale;From Metallic Map (G);Texture (R)" = "0" (shaderDefine)
        roughnessMap("Roughness Map") : object TextureAsset = "_whiteTexture"
        roughnessScale("Roughness Scale") : float range 0 1.0 0.01 = "1.0"
        _NORMAL_SOURCE("Normal") : enum "Vertex;Texture;Texture + Detail Texture" = "0" (shaderDefine)
        normalMap("Normal Map") : object TextureAsset = "_flatNormalTexture"
        detailNormalMap("Detail Normal Map") : object TextureAsset = "_flatNormalTexture"
        detailRepeat("Detail Repeat") : float = "8"
        _PARALLAX_SOURCE("Parallax") : enum "None;Texture" = "0" (shaderDefine)
        heightMap("Height Map") : object TextureAsset = "_whiteTexture"
        heightScale("Height Scale") : float range 0.01 1.0 0.001 = "0.008"
        _OCCLUSION_SOURCE("Occlusion") : enum "None;Texture" = "0" (shaderDefine)
        occlusionMap("Occlusion Map") : object TextureAsset = "_whiteTexture"
        occlusionScale("Occlusion Scale") : float range 0 1 0.001 = "1"
        _EMISSION_SOURCE("Emission") : enum "None;Color;Texture" = "0" (shaderDefine)
        emissionMap("Emission Map") : object TextureAsset = "_blackTexture"
        emissionColor("Emission Color") : color3 = "1 1 1"
        emissionScale("Emission Scale") : float range 0 10 0.001 = "1"
    }
    
    generatePerforatedVersion
    generateGpuSkinningVersion

    ambientLitVersion "StandardAmbientLit.shader"
    directLitVersion "StandardDirectLit.shader"
    ambientLitDirectLitVersion "StandardAmbientLitDirectLit.shader"
    
    glsl_vp {
        #define STANDARD_METALLIC_LIGHTING
        $include "ForwardCore.vp"
    }
    glsl_fp {
        #define STANDARD_METALLIC_LIGHTING
        $include "ForwardCore.fp"
    }
}

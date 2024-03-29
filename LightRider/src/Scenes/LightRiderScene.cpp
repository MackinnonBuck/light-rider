#include "Scenes/LightRiderScene.h"
#include "ComputeProgram.h"
#include "ProgramMetadata.h"

void LightRiderScene::loadAssets()
{
    AssetManager* pAssets = getAssetManager();

    // Bike assets.
    Program* pBikeShader = loadShaderProgramWithDynamicOutput("bikeShader", "bike_vertex.glsl", "bike_fragment.glsl");
    pBikeShader->addUniform("playerId");
    pBikeShader->addUniform("transitionAmount");

    Program* pTrailShader = loadShaderProgramWithDynamicOutput("trailShader", "trail_vertex.glsl", "trail_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
    );
    pTrailShader->addUniform("playerId");
    pTrailShader->addUniform("noiseSeed");
    pTrailShader->addUniform("currentTime");

    pAssets->loadTexture("bikeTexture", "light_cycle_texture.png", TextureType::IMAGE);
    pAssets->loadShape("bikeShape", "light_cycle.shape");

    Program* pChunkShader = loadShaderProgramWithDynamicOutput("chunkShader", "chunk_vertex.glsl", "chunk_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_0);
    pChunkShader->addUniform("playerId");

    pAssets->loadTexture("chunkTexture", "chunk_texture.png", TextureType::IMAGE);
    pAssets->loadShape("chunkShape", "chunk_particle.shape");

    // Ground assets.
    Program* pGroundShader = loadShaderProgramWithDynamicOutput("groundShader", "ground_vertex.glsl", "ground_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_0
      | ShaderUniform::TEXTURE_3);
    pGroundShader->addUniform("lightPV");
    pGroundShader->addUniform("campos");
    pGroundShader->bind();
    glUniform1i(pGroundShader->getUniform("texture3"), 3);
    pGroundShader->unbind();
    
    pAssets->loadTexture("groundTexture", "ground_texture.png", TextureType::IMAGE);

    Program* pRampShader = loadShaderProgramWithDynamicOutput("rampShader", "ramp_vertex.glsl", "ramp_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX);
    pRampShader->addUniform("time");

    // Container assets.
    loadShaderProgramWithDynamicOutput("containerShader", "container_vertex.glsl", "container_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_0);
      //| ShaderUniform::TEXTURE_2);
    pAssets->loadTexture("containerTexture", "container_color_texture.png", TextureType::IMAGE);
    pAssets->loadTexture("containerNormalTexture", "container_normal_texture.png", TextureType::NORMAL);
    pAssets->loadShape("containerShape", "container.shape");

    // Sky assets.
    pAssets->loadShaderProgram("skyShader", "sky_vertex.glsl", "sky_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_3
    );
    pAssets->loadTexture("skyTexture", "sky.jpg", TextureType::BACKGROUND);

    // Miscellaneous assets.
    Program* pShadowShader = pAssets->loadShaderProgram("shadowShader", "shadow_vertex.glsl", "shadow_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX);
    pShadowShader->addAttribute("vertexPosition");

    Program* pDeferredShader = pAssets->loadShaderProgram("deferredShader", "deferred_vertex.glsl", std::vector<std::string>{ "deferred_fragment.glsl", "materials.glsl" }, ShaderUniform::NONE);
    pDeferredShader->addUniform("gColor");
    pDeferredShader->addUniform("gPosition");
    pDeferredShader->addUniform("gNormal");
    pDeferredShader->addUniform("gMaterial");
    pDeferredShader->addUniform("shadowMap");
    pDeferredShader->addUniform("skyTexture");
    pDeferredShader->addUniform("campos");
    pDeferredShader->addUniform("lightPV");

    Program* pGiShader = pAssets->loadShaderProgram("giShader", "gi_vertex.glsl", "gi_fragment.glsl", ShaderUniform::NONE);
    pGiShader->addUniform("gPosition");
    pGiShader->addUniform("gNormal");
    pGiShader->addUniform("gMaterial");
    pGiShader->addUniform("noise");
    pGiShader->addUniform("voxelMap");
    pGiShader->addUniform("view");
    pGiShader->addUniform("focalLength");
    pGiShader->addUniform("voxelCenterPosition");

    Program* pBlendedDeferredShader = pAssets->loadShaderProgram("blendedDeferredShader", "blended_deferred_vertex.glsl", "blended_deferred_fragment.glsl", ShaderUniform::NONE);
    pBlendedDeferredShader->addUniform("gColor");
    pBlendedDeferredShader->addUniform("gPosition");
    pBlendedDeferredShader->addUniform("gNormal");
    pBlendedDeferredShader->addUniform("gMaterial");
    pBlendedDeferredShader->addUniform("ssaoTexture");
    pBlendedDeferredShader->addUniform("indirectTexture");
    pBlendedDeferredShader->addUniform("occlusionFactor");

    Program* pPostShader = pAssets->loadShaderProgram("postShader", "post_vertex.glsl", "post_fragment.glsl", ShaderUniform::NONE);
    pPostShader->addUniform("screenTexture");
    pPostShader->addUniform("exposure");

    Program* pFxaaShader = pAssets->loadShaderProgram("fxaaShader", "fxaa_vertex.glsl", "fxaa_fragment.glsl", ShaderUniform::NONE);
    pFxaaShader->addUniform("screenTexture");
    pFxaaShader->addUniform("screenWidth");
    pFxaaShader->addUniform("screenHeight");

    Program* pBlurShader = pAssets->loadShaderProgram("blurShader", "blur_vertex.glsl", "blur_fragment.glsl", ShaderUniform::NONE);
    pBlurShader->addUniform("screenTexture");
    pBlurShader->addUniform("horizontal");

    Program* pBloomShader = pAssets->loadShaderProgram("bloomShader", "bloom_vertex.glsl", "bloom_fragment.glsl", ShaderUniform::NONE);
    pBloomShader->addUniform("screenTexture");
    pBloomShader->addUniform("blurTexture");
    pBloomShader->addUniform("bloom");
    pBloomShader->addUniform("exposure");

    ComputeProgram* pLuminanceProgram = pAssets->loadComputeShaderProgram("luminanceCompute", "luminance_compute.glsl");
    pLuminanceProgram->addBuffer("result", sizeof(glm::vec4));
    pLuminanceProgram->addUniform("colorImage");

    ComputeProgram* pVoxelClearProgram = pAssets->loadComputeShaderProgram("voxelClear", "voxel_clear.glsl");
    pVoxelClearProgram->addUniform("voxelMapR");
    pVoxelClearProgram->addUniform("voxelMapG");
    pVoxelClearProgram->addUniform("voxelMapB");
    pVoxelClearProgram->addUniform("voxelMapA");

    ComputeProgram* pVoxelCombineProgram = pAssets->loadComputeShaderProgram("voxelCombine", "voxel_combine.glsl");
    pVoxelCombineProgram->addUniform("voxelMapR");
    pVoxelCombineProgram->addUniform("voxelMapG");
    pVoxelCombineProgram->addUniform("voxelMapB");
    pVoxelCombineProgram->addUniform("voxelMapA");

    ComputeProgram* pVoxelMipmapProgram = pAssets->loadComputeShaderProgram("voxelMipmap", "voxel_mipmap.glsl");
    pVoxelMipmapProgram->addUniform("inMip");
    pVoxelMipmapProgram->addUniform("outMip");

    pAssets->loadShape("sphereShape", "sphere.shape");
    pAssets->loadShape("planeShape", "plane.shape");
    pAssets->loadShape("rampShape", "ramp.shape");
    pAssets->loadShape("rampShapeCollision", "ramp_collision.shape");
}

Program* LightRiderScene::loadShaderProgramWithDynamicOutput(const std::string& id, const std::string& vertexShaderFileName,
    const std::string& fragmentShaderFileName, ShaderUniform defaultUniforms)
{
    Program* pProgram = getAssetManager()->loadShaderProgram(id, vertexShaderFileName, { "output.glsl", "materials.glsl", fragmentShaderFileName}, defaultUniforms);
    pProgram->addUniform("_outputMode");
    pProgram->addUniform("_voxelCenterPosition");
    pProgram->addUniform("_cameraPosition");
    pProgram->addUniform("_lightPV");
    pProgram->addUniform("_shadowMap");
    pProgram->addUniform("_skyTexture");
    pProgram->addUniform("_voxelMapR");
    pProgram->addUniform("_voxelMapG");
    pProgram->addUniform("_voxelMapB");
    pProgram->addUniform("_voxelMapA");
    pProgram->setUserPointer((void*)ProgramMetadata::USES_DYNAMIC_OUTPUT);

    pProgram->bind();
    glUniform1i(pProgram->getUniform("_shadowMap"), 4);
    glUniform1i(pProgram->getUniform("_skyTexture"), 5);
    pProgram->unbind();

    return pProgram;
}

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
    pGroundShader->addUniform("shadowMap");
    pGroundShader->addUniform("campos");
    //pGroundShader->addUniform("lightPosition");
    //pGroundShader->addUniform("lightDirection");
    pGroundShader->bind();
    glUniform1i(pGroundShader->getUniform("texture3"), 3);
    glUniform1i(pGroundShader->getUniform("shadowMap"), 4);
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

    Program* pDeferredShader = pAssets->loadShaderProgram("deferredShader", "deferred_vertex.glsl", "deferred_fragment.glsl", ShaderUniform::NONE);
    pDeferredShader->addUniform("gColor");
    pDeferredShader->addUniform("gPosition");
    pDeferredShader->addUniform("gNormal");
    pDeferredShader->addUniform("gMaterial");
    pDeferredShader->addUniform("shadowMap");
    pDeferredShader->addUniform("skyTexture");
    pDeferredShader->addUniform("campos");
    pDeferredShader->addUniform("lightPV");

    Program* pSsaoShader = pAssets->loadShaderProgram("ssaoShader", "ssao_vertex.glsl", "ssao_fragment.glsl", ShaderUniform::NONE);
    pSsaoShader->addUniform("gPosition");
    pSsaoShader->addUniform("gNormal");
    pSsaoShader->addUniform("gMaterial");
    pSsaoShader->addUniform("noise");
    pSsaoShader->addUniform("samples");
    pSsaoShader->addUniform("projection");
    pSsaoShader->addUniform("view");

    Program* pHbaoShader = pAssets->loadShaderProgram("hbaoShader", "hbao_vertex.glsl", "hbao_fragment.glsl", ShaderUniform::NONE);
    pHbaoShader->addUniform("gPosition");
    pHbaoShader->addUniform("gNormal");
    pHbaoShader->addUniform("gMaterial");
    pHbaoShader->addUniform("noise");
    pHbaoShader->addUniform("view");
    pHbaoShader->addUniform("focalLength");

    Program* pBlendedDeferredShader = pAssets->loadShaderProgram("blendedDeferredShader", "blended_deferred_vertex.glsl", "blended_deferred_fragment.glsl", ShaderUniform::NONE);
    pBlendedDeferredShader->addUniform("gColor");
    pBlendedDeferredShader->addUniform("gPosition");
    pBlendedDeferredShader->addUniform("gNormal");
    pBlendedDeferredShader->addUniform("gMaterial");
    pBlendedDeferredShader->addUniform("ssaoTexture");
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

    pAssets->loadShape("sphereShape", "sphere.shape");
    pAssets->loadShape("planeShape", "plane.shape");
    pAssets->loadShape("rampShape", "ramp.shape");
    pAssets->loadShape("rampShapeCollision", "ramp_collision.shape");
}

Program* LightRiderScene::loadShaderProgramWithDynamicOutput(const std::string& id, const std::string& vertexShaderFileName,
    const std::string& fragmentShaderFileName, ShaderUniform defaultUniforms)
{
    Program* pProgram = getAssetManager()->loadShaderProgram(id, vertexShaderFileName, { "output.glsl", fragmentShaderFileName }, defaultUniforms);
    pProgram->addUniform("_outputMode");
    pProgram->addUniform("_voxelMap");
    pProgram->setUserPointer((void*)ProgramMetadata::USES_DYNAMIC_OUTPUT);

    return pProgram;
}

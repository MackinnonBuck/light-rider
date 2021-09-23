#include "Scenes/LightRiderScene.h"

void LightRiderScene::loadAssets()
{
    AssetManager* pAssets = getAssetManager();

    // Bike assets.
    Program* pBikeShader = pAssets->loadShaderProgram("bikeShader", "bike_vertex.glsl", "bike_fragment.glsl");
    pBikeShader->addUniform("playerId");
    pBikeShader->addUniform("campos");

    Program* pTrailShader = pAssets->loadShaderProgram("trailShader", "trail_vertex.glsl", "trail_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
    );
    pTrailShader->addUniform("playerId");
    pTrailShader->addUniform("noiseSeed");
    pTrailShader->addUniform("currentTime");

    pAssets->loadTexture("bikeTexture", "light_cycle_texture.png", TextureType::IMAGE);
    pAssets->loadShape("bikeShape", "light_cycle.shape");

    // Ground assets.
    pAssets->loadShaderProgram("groundShader", "ground_vertex.glsl", "ground_fragment.glsl");
    pAssets->loadTexture("groundTexture", "ground_texture.png", TextureType::IMAGE);

    // Sky assets.
    pAssets->loadShaderProgram("skyShader", "sky_vertex.glsl", "sky_fragment.glsl",
        ShaderUniform::P_MATRIX
      | ShaderUniform::V_MATRIX
      | ShaderUniform::M_MATRIX
      | ShaderUniform::TEXTURE_3
    );
    pAssets->loadTexture("skyTexture", "sky.jpg", TextureType::BACKGROUND);

    // Miscellaneous assets.
    Program* pDeferredShader = pAssets->loadShaderProgram("deferredShader", "deferred_vertex.glsl", "deferred_fragment.glsl", ShaderUniform::NONE);
    pDeferredShader->addUniform("gColor");
    pDeferredShader->addUniform("gPosition");
    pDeferredShader->addUniform("gNormal");
    pDeferredShader->addUniform("gMaterial");
    pDeferredShader->addUniform("campos");

    Program* pPostShader = pAssets->loadShaderProgram("postShader", "post_vertex.glsl", "post_fragment.glsl", ShaderUniform::NONE);
    pPostShader->addUniform("screenTexture");

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

    pAssets->loadShape("sphereShape", "sphere.shape");
    pAssets->loadShape("planeShape", "plane.shape");
}

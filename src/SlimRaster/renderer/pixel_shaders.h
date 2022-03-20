#pragma once

#include "../math/vec3.h"
#include "../scene/texture.h"
#include "./common.h"


INLINE vec3 sampleNormal(Texture *texture, vec2 uv, vec2 dUV) {
    vec3 normal = sampleTexture(texture, uv, dUV).v3;
    f32 y = normal.z;
    normal.z = normal.y;
    normal.y = y;
    return normVec3(scaleAddVec3(normal, 2.0f, getVec3Of(-1.0f)));
}
INLINE quat getNormalRotation(vec3 normal, f32 magnitude) {
    quat q;
    normal = scaleVec3(normal, 0.25f);
    q.axis.x = normal.z;
    q.axis.y = 0;
    q.axis.z = normal.x;
    q.amount = normal.y * magnitude;
    return normQuat(q);
}

INLINE u8 getCheckerBoardPixelValueByUV(vec2 UV, f32 half_step_count) {
    f32 s = UV.u * half_step_count;
    f32 t = UV.v * half_step_count;
    s -= floorf(s);
    t -= floorf(t);
    return (s > 0.5f ? (u8)1 : (u8)0) ^ (t < 0.5f ? (u8)1 : (u8)0);
}

void shadePixelTextured(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = sampleTexture(scene->textures + shaded->material->texture_ids[0], inputs->UV, inputs->dUV).v3;
}

void shadePixelDepth(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(inputs->depth > 10 ? 1 : inputs->depth * 0.1f);
}

void shadePixelUV(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color.v2 = inputs->UV;
}

void shadePixelPosition(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleVec3(addVec3(shaded->position, getVec3Of(2.0f)), 0.5f);
}

void shadePixelNormal(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = scaleAddVec3(shaded->normal, 0.5f, getVec3Of(0.5f));
}

void shadePixelCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    outputs->color = getVec3Of(getCheckerBoardPixelValueByUV(inputs->UV, 4));
}

void shadePixelClassic(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    f32 NdotL, NdotRd, squared_distance;
    decodeMaterialSpec(shaded->material->flags, &shaded->has, &shaded->uses);

    shaded->diffuse = shaded->material->diffuse;

    if (shaded->material->texture_count) {
        Texture *texture = &scene->textures[shaded->material->texture_ids[0]];
        vec4 texture_sample = sampleTexture(texture, inputs->UV, inputs->dUV);
        shaded->diffuse = mulVec3(shaded->diffuse, texture_sample.v3);
        if (shaded->material->texture_count > 1) {
            texture = &scene->textures[shaded->material->texture_ids[1]];
            texture_sample.v3 = sampleNormal(texture, inputs->UV, inputs->dUV);
            quat normal_rotation = getNormalRotation(texture_sample.v3, shaded->material->normal_magnitude);
            shaded->normal = mulVec3Quat(shaded->normal, normal_rotation);
        }
    }

    outputs->color = scene->ambient_light.color;
    shaded->viewing_direction = normVec3(subVec3(shaded->position, shaded->viewing_origin));
    if (shaded->uses.phong) {
        NdotRd = DotVec3(shaded->normal, shaded->viewing_direction);
        shaded->reflected_direction = reflectWithDot(shaded->viewing_direction, shaded->normal, NdotRd);
    }

    Light *light = scene->lights;
    for (u32 i = 0; i < scene->settings.lights; i++, light++) {
        shaded->light_direction = subVec3(light->position_or_direction, shaded->position);
        squared_distance = squaredLengthVec3(shaded->light_direction);
        shaded->light_direction = scaleVec3(shaded->light_direction, 1.0f / sqrtf(squared_distance));
        NdotL = dotVec3(shaded->normal, shaded->light_direction);
        if (NdotL > 0)
            outputs->color = mulAddVec3(
                    shadePointOnSurface(shaded, NdotL),
                    scaleVec3(light->color, light->intensity / squared_distance),
                    outputs->color);
    }

    outputs->color.x = toneMappedBaked(outputs->color.x);
    outputs->color.y = toneMappedBaked(outputs->color.y);
    outputs->color.z = toneMappedBaked(outputs->color.z);
}

void shadePixelClassicCheckerboard(PixelShaderInputs *inputs, Scene *scene, Shaded *shaded, PixelShaderOutputs *outputs) {
    shadePixelClassic(inputs, scene, shaded, outputs);

    if (!getCheckerBoardPixelValueByUV(inputs->UV, 4))
        outputs->color = scaleVec3(outputs->color, 0.5f);
}
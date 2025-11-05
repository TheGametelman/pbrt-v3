//
// Created by mathi on 03.11.2025.
//

#pragma once

#include "pbrt.h"
#include "integrator.h"
#include "scene.h"

namespace pbrt
{
    class ConventionalIntegrator : public SamplerIntegrator
    {
    public:
        ConventionalIntegrator(int maxDepth, std::shared_ptr<const Camera> camera,
                               std::shared_ptr<Sampler> sampler,
                               const Bounds2i& pixelBounds) : SamplerIntegrator(camera, sampler, pixelBounds), maxDepth(maxDepth) {}

        Spectrum Li(const RayDifferential& ray, const Scene& scene, Sampler& sampler, MemoryArena& arena, int depth) const;

    private:
        const int maxDepth;
    };

    ConventionalIntegrator* CreateConventionalIntegrator(
                                const ParamSet& params, std::shared_ptr<Sampler> sampler,
                                std::shared_ptr<const Camera> camera);
}
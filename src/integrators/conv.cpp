//
// Created by mathi on 03.11.2025.
//

#include "integrators/conv.h"
#include "interaction.h"
#include "camera.h"
#include "film.h"
#include "paramset.h"
#include <core\sampling.h>

namespace pbrt
{
    Spectrum ConventionalIntegrator::Li(const RayDifferential& ray, const Scene& scene,
                                        Sampler& sampler, MemoryArena& arena, int depth) const
    {
        Spectrum L(0.);

        SurfaceInteraction isect;
        bool wasHit = scene.Intersect(ray, &isect);
        if (!wasHit) {
            return L;
        }

        Vector3f wo = isect.wo;

        for (const auto& light : scene.lights) {
            Point3f lightPos = light->LightToWorld(Point3f(0, 0, 0));
            Spectrum power = light->Power();
            float cos = AbsDot(Normalize(Vector3f(lightPos)-Vector3f(isect.p)),isect.n);
            Spectrum truePower = power * cos / (4 * Pi * DistanceSquared(lightPos, isect.p));

            Ray r = isect.SpawnRayTo(lightPos);
            bool isVisible = !scene.IntersectP(r);

            if (isVisible) {
                L += truePower;
            }
        }
        if (depth + 1 < maxDepth) {
            // Trace rays for specular reflection and refraction
            Ray rayRefl = isect.SpawnRay(-wo - Vector3f(2*Dot(isect.n, -wo) *isect.n));
            L += Li(rayRefl, scene, sampler, arena, depth + 1);
        }
        return L;
    }

    ConventionalIntegrator* CreateConventionalIntegrator(const ParamSet& params,
                                                         std::shared_ptr<Sampler> sampler,
                                                         std::shared_ptr<const Camera> camera)
    {
        int maxDepth = params.FindOneInt("maxdepth", 5);
        int np;
        const int* pb = params.FindInt("pixelbounds", &np);
        Bounds2i pixelBounds = camera->film->GetSampleBounds();
        if (pb) {
            if (np != 4)
                Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
                    np);
            else {
                pixelBounds = Intersect(pixelBounds,
                    Bounds2i{ {pb[0], pb[2]}, {pb[1], pb[3]} });
                if (pixelBounds.Area() == 0)
                    Error("Degenerate \"pixelbounds\" specified.");
            }
        }
        return new ConventionalIntegrator(maxDepth, camera, sampler, pixelBounds);
    }
}

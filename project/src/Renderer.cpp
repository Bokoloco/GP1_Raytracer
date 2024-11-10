//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

#include <execution>
#define PARALLEL_EXECUTION

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	/*auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();*/

	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	float aspectRatio{ float(m_Width) / m_Height };
	const float fovAngle{ camera.fovAngle * TO_RADIANS };
	const float fov{ tan(fovAngle/ 2) };

	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };

#if defined(PARALLEL_EXECUTION)
	//Parallel logic
	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountOfPixels);
	for (uint32_t index; index < amountOfPixels; ++index) pixelIndices.emplace_back(index);

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i) {
		RenderPixel(pScene, i, fov, aspectRatio, cameraToWorld, camera.origin);});
#else
	//Synchronous logic (no threading)
	for (uint32_t pixelIndex{}; pixelIndex < amountOfPixels; ++pixelIndex)
	{
		RenderPixel(pScene, pixelIndex, fov, aspectRatio, cameraToWorld, camera.origin);
	}

#endif


	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

/* Old rendering function
void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float aspectRatio{ float(m_Width) / m_Height };
	float FOV{ tan((camera.fovAngle * (PI / 180.f)) / 2) };
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Vector3 rayDirection{ ((2.f * ((float(px) + 0.5f) / m_Width)) - 1.f) * aspectRatio * FOV, (1.f - (2.f * ((float(py) + 0.5f) / m_Height))) * FOV, 1.f };

			rayDirection = cameraToWorld.TransformVector(rayDirection);

			Ray viewRay{ camera.origin, rayDirection.Normalized()};

			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };

			ColorRGB finalColor{};

			HitRecord closestHit{};

			/*Plane testsphere{ {0.f, -50.f, 0.f}, {0.f, 1.f, 0.f}, 0 };
			GeometryUtils::HitTest_Plane(testsphere, viewRay, closestHit);

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = {};

				for (int light{}; light < lights.size(); ++light)
				{

					Vector3 directionToLight{ LightUtils::GetDirectionToLight(lights[light], closestHit.origin)};
					float magnitude{ directionToLight.Normalize() };

					Ray lightRay{ closestHit.origin + (directionToLight * .01f), directionToLight };
					lightRay.max = magnitude;

					float dot{ Vector3::Dot(closestHit.normal, directionToLight) };

					if (dot <= 0.f) continue;

					if (pScene->DoesHit(lightRay) and m_ShadowsEnabled) continue;

					switch (m_CurrentLightingMode)
					{
					case dae::Renderer::LightingMode::ObservationArea:
						finalColor += dot * ColorRGB{1.f, 1.f, 1.f};
						break;
					case dae::Renderer::LightingMode::Radiance:
						finalColor += LightUtils::GetRadiance(lights[light], closestHit.origin);
						break;
					case dae::Renderer::LightingMode::BRDF:
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, -viewRay.direction);
						break;
					case dae::Renderer::LightingMode::Combined:
						finalColor += LightUtils::GetRadiance(lights[light], closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, -viewRay.direction) * dot;
						break;
					default:
						break;
					}
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}*/

void dae::Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto& materials{ pScene->GetMaterials() };
	auto& lights{ pScene->GetLights() };

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ px + .5f }, ry{ py + .5f };
	float cx{ (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov };
	float cy{ (1 - (2 * (ry / float(m_Height)))) * fov };

	Vector3 rayDirection{ cx, cy, 1.f };

	rayDirection = cameraToWorld.TransformVector(rayDirection);

	Ray viewRay{ cameraOrigin, rayDirection.Normalized() };

	ColorRGB finalColor{};
	HitRecord closestHit{};

	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		finalColor = {};

		for (int light{}; light < lights.size(); ++light)
		{

			Vector3 directionToLight{ LightUtils::GetDirectionToLight(lights[light], closestHit.origin) };
			float magnitude{ directionToLight.Normalize() };

			Ray lightRay{ closestHit.origin + (directionToLight * .01f), directionToLight };
			lightRay.max = magnitude;

			float dot{ Vector3::Dot(closestHit.normal, directionToLight) };

			if (dot <= 0.f) continue;

			if (pScene->DoesHit(lightRay) and m_ShadowsEnabled) continue;

			switch (m_CurrentLightingMode)
			{
			case dae::Renderer::LightingMode::ObservationArea:
				finalColor += dot * ColorRGB{ 1.f, 1.f, 1.f };
				break;
			case dae::Renderer::LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(lights[light], closestHit.origin);
				break;
			case dae::Renderer::LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, -viewRay.direction);
				break;
			case dae::Renderer::LightingMode::Combined:
				finalColor += LightUtils::GetRadiance(lights[light], closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, -viewRay.direction) * dot;
				break;
			default:
				break;
			}
		}
	}

	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	int lightingStateInt{ int(m_CurrentLightingMode) };
	m_CurrentLightingMode = LightingMode((lightingStateInt + 1) % 4);
}

void Renderer::CheckKeysInput()
{
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	if (pKeyboardState[SDL_SCANCODE_F2])
	{
		ToggleShadows();
	}
	if (pKeyboardState[SDL_SCANCODE_F3])
	{
		CycleLightingMode();
	}
}

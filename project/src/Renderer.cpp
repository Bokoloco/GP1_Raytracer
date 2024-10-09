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
			GeometryUtils::HitTest_Plane(testsphere, viewRay, closestHit);*/

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				/*const float scaled_t = (closestHit.t / 500.f);
				finalColor = { scaled_t, scaled_t, scaled_t };*/

				for (int light{}; light < lights.size(); ++light)
				{
					Vector3 lightDirection{ LightUtils::GetDirectionToLight(lights[light], closestHit.origin)};
					float magnitude{ lightDirection.Magnitude() };

					Ray lightRay{ closestHit.origin + (closestHit.normal / 100.f), lightDirection.Normalized() };
					lightRay.max = magnitude;

					if (pScene->DoesHit(lightRay))
					{
						finalColor *= 0.5f;
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
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

#include "Scene.h"
#include "Utils.h"
#include "Material.h"

#define SPHERE_SCENE

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		//todo W1
		HitRecord closestHitTemp{};
		closestHit.t = ray.max;

		for (int idx{}; idx < m_SphereGeometries.size(); idx++)
		{
			GeometryUtils::HitTest_Sphere(m_SphereGeometries.at(idx), ray, closestHitTemp);

			if (closestHitTemp.t <= closestHit.t and closestHitTemp.didHit)
			{
				closestHit = closestHitTemp;
			}
		}

		for (int idx{}; idx < m_PlaneGeometries.size(); idx++)
		{
			GeometryUtils::HitTest_Plane(m_PlaneGeometries.at(idx), ray, closestHitTemp);

			if (closestHitTemp.t <= closestHit.t and closestHitTemp.didHit)
			{
				closestHit = closestHitTemp;
			}
		}

		for (int idx{}; idx < m_TriangleMeshGeometries.size(); idx++)
		{
			GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries.at(idx), ray, closestHitTemp);

			if (closestHitTemp.t <= closestHit.t and closestHitTemp.didHit)
			{
				closestHit = closestHitTemp;
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (int idx{}; idx < m_SphereGeometries.size(); idx++)
		{
			if (GeometryUtils::HitTest_Sphere(m_SphereGeometries.at(idx), ray)) return true; 
		}

		for (int idx{}; idx < m_PlaneGeometries.size(); idx++)
		{
			if (GeometryUtils::HitTest_Plane(m_PlaneGeometries.at(idx), ray)) return true;
		}

		for (int idx{}; idx < m_TriangleMeshGeometries.size(); idx++)
		{
			if (GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries.at(idx), ray)) return true;
		}
		
		return false;
	}

	void Scene::ChangeScene()
	{

	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		/*AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);*/
		/*AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);*/
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}
#pragma endregion

#pragma region SCENE W3
void Scene_W3::Initialize()
{
	m_Camera.origin = { 0.f, 3.f, -9.f };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 1.0f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 1.0f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.0f));

	//Plane
	AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BACK
	AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
	AddPlane(Vector3{ 5.0f, 0.0f, 0.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //RIGHT
	AddPlane(Vector3{ -5.0f, 0.0f, 0.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

	/*const auto matLambertPhong1 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 3.0f));
	const auto matLambertPhong2 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 15.0f));
	const auto matLambertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, 0.5f, 0.5f, 50.0f));

	AddSphere(Vector3{ -1.75f, 1.0f, 0.0f }, 0.75f, matLambertPhong1);
	AddSphere(Vector3{ 0.0f, 1.0f, 0.0f }, 0.75f, matLambertPhong2);
	AddSphere(Vector3{ 1.75f, 1.0f, 0.0f }, 0.75f, matLambertPhong3);*/

	//Spheres
	AddSphere(Vector3{ -1.75f, 1.0f, 0.0f }, 0.75f, matCT_GrayRoughMetal);
	AddSphere(Vector3{ 0.0f, 1.0f, 0.0f }, 0.75f, matCT_GrayMediumMetal);
	AddSphere(Vector3{ 1.75f, 1.0f, 0.0f }, 0.75f, matCT_GraySmoothMetal);
	AddSphere(Vector3{ -1.75f, 3.0f, 0.0f }, 0.75f, matCT_GrayRoughPlastic);
	AddSphere(Vector3{ 0.0f, 3.0f, 0.0f }, 0.75f, matCT_GrayMediumPlastic);
	AddSphere(Vector3{ 1.75f, 3.0f, 0.0f }, 0.75f, matCT_GraySmoothPlastic);

	//Light
	AddPointLight({ 0.0f, 5.0f, 5.0f }, 50.f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //Backlight
	AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light left
	AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });

	//m_Camera.origin = { 0.f, 1.f, -5.f };
	//m_Camera.fovAngle = 45.f;

	//const auto matLambert_Red = AddMaterial(new Material_Lambert(colors::Red, 1.f));
	//const auto matLambert_Blue = AddMaterial(new Material_LambertPhong(colors::Blue, 1.f, 1.f, 30.f));
	//const auto matLambert_Yellow = AddMaterial(new Material_Lambert(colors::Yellow, 1.f));

	////Spheres
	//AddSphere({ -.75f, 1.f, 0.f }, 1.f, matLambert_Red);
	//AddSphere({ .75f, 1.f, 0.f }, 1.f, matLambert_Blue);

	////Plane
	//AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matLambert_Yellow);

	//AddPointLight({ 0.f, 5.f, 5.f }, 25.f, colors::White);
	//AddPointLight({ 0.f, 2.5f, -5.f }, 25.f, colors::White);
}
#pragma endregion

#pragma region SCENE W4
void Scene_W4::Initialize()
{
	m_Camera.origin = { 0.0f, 3.f, -9.f };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 1.0f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 1.0f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.0f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//Plane
	AddPlane(Vector3{ 0.0f, 0.0f, 10.0f }, Vector3{ 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BACK
	AddPlane(Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(Vector3{ 0.0f, 10.0f, 0.0f }, Vector3{ 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
	AddPlane(Vector3{ 5.0f, 0.0f, 0.0f }, Vector3{ -1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //RIGHT
	AddPlane(Vector3{ -5.0f, 0.0f, 0.0f }, Vector3{ 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

#if defined (SPHERE_SCENE)
	//Spheres
	AddSphere(Vector3{ -1.75f, 1.0f, 0.0f }, 0.75f, matCT_GrayRoughMetal);
	AddSphere(Vector3{ 0.0f, 1.0f, 0.0f }, 0.75f, matCT_GrayMediumMetal);
	AddSphere(Vector3{ 1.75f, 1.0f, 0.0f }, 0.75f, matCT_GraySmoothMetal);
	AddSphere(Vector3{ -1.75f, 3.0f, 0.0f }, 0.75f, matCT_GrayRoughPlastic);
	AddSphere(Vector3{ 0.0f, 3.0f, 0.0f }, 0.75f, matCT_GrayMediumPlastic);
	AddSphere(Vector3{ 1.75f, 3.0f, 0.0f }, 0.75f, matCT_GraySmoothPlastic);

	//Triangles
	const Triangle baseTriangle{ Vector3{-.75f, 1.5f, 0.f}, Vector3{.75f, 0.f, 0.f}, Vector3{-.75f, 0.f, 0.f} };

	m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	m_Meshes[0]->AppendTriangle(baseTriangle, true);
	m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
	m_Meshes[0]->UpdateTransforms();

	m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
	m_Meshes[1]->AppendTriangle(baseTriangle, true);
	m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
	m_Meshes[1]->UpdateTransforms();

	m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	m_Meshes[2]->AppendTriangle(baseTriangle, true);
	m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
	m_Meshes[2]->UpdateTransforms();
#else
	pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	Utils::ParseOBJ("Resources/lowpoly_bunny.obj", pMesh->positions, pMesh->normals, pMesh->indices);

	pMesh->Scale({ 2.f, 2.f, 2.f });

	pMesh->UpdateAABB();
	pMesh->UpdateTransforms();
#endif

	//Light
	AddPointLight({ 0.0f, 5.0f, 5.0f }, 50.f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //Backlight
	AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light left
	AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.f, ColorRGB{ 0.34f, 0.47f, 0.68f });

	//Triangle (temp)
	/*auto triangle = Triangle{ {-0.75f, 0.5f, 0.0f}, {-0.75f, 2.0f, 0.0f}, {0.75f, 0.5f, 0.0f} };
	triangle.cullMode = TriangleCullMode::BackFaceCulling;
	triangle.materialIndex = matLambert_White;

	m_Triangles.emplace_back(triangle);*/

	//const auto triangleMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	//triangleMesh->positions = { {-0.75f, -1.0f, 0.0f}, {-0.75f, 1.0f, 0.0f}, {0.75f, 1.0f, 1.0f}, {0.75f, -1.0f, 0.0f} };
	//triangleMesh->indices = {
	//	0, 1, 2, //triangle 1
	//	0, 2, 3 //triangle 2
	//};

	//triangleMesh->CalculateNormals();

	//triangleMesh->Translate({ 0.f, 1.5f, 0.f });
	////triangleMesh->RotateY(45);

	//triangleMesh->UpdateTransforms();

	//pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	//pMesh->indices = {
	//	0, 1, 2, //triangle 1
	//	0, 2, 3 //triangle 2
	//};
	//Utils::ParseOBJ("Resources/simple_cube.obj", pMesh->positions, pMesh->normals, pMesh->indices);

	//pMesh->Scale({ .7f, .7f, .7f });
	//pMesh->Translate({ 0.f, 1.f, 0.f });

	//pMesh->UpdateTransforms();
}
void Scene_W4::Update(Timer* pTimer)
{
	Scene::Update(pTimer);

#if defined(SPHERE_SCENE)
	const auto yawAngle{ (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2 };
	for (const auto m : m_Meshes)
	{
		m->UpdateAABB();
		m->RotateY(yawAngle);
		m->UpdateTransforms();
	}
#endif
}
#pragma endregion

#pragma region SCENE SPHERE
void Scene_SphereScene::Initialize()
{
	m_Camera.origin = { 0.0f, 3.f, -9.f };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//Plane
	AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
	AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
	AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
	AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

	//Spheres
	AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
	AddSphere(Vector3{ 0.0f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
	AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
	AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
	AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
	AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

	//Triangles
	const Triangle baseTriangle{ Vector3{-.75f, 1.5f, 0.f}, Vector3{.75f, 0.f, 0.f}, Vector3{-.75f, 0.f, 0.f} };

	m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	m_Meshes[0]->AppendTriangle(baseTriangle, true);
	m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
	m_Meshes[0]->UpdateTransforms();

	m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
	m_Meshes[1]->AppendTriangle(baseTriangle, true);
	m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
	m_Meshes[1]->UpdateTransforms();

	m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	m_Meshes[2]->AppendTriangle(baseTriangle, true);
	m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
	m_Meshes[2]->UpdateTransforms();

	//Light
	AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
	AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
	AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
}
void Scene_SphereScene::Update(Timer* pTimer)
{
	Scene::Update(pTimer);

	const auto yawAngle{ (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2 };
	for (const auto m : m_Meshes)
	{
		m->UpdateAABB();
		m->RotateY(yawAngle);
		m->UpdateTransforms();
	}
}
#pragma endregion

#pragma region SCENE BUNNY
void Scene_BunnyScene::Initialize()
{
	m_Camera.origin = { 0.0f, 3.f, -9.f };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//Plane
	AddPlane(Vector3{ 0.f, 0.f, 10.f }, Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
	AddPlane(Vector3{ 0.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(Vector3{ 0.f, 10.f, 0.f }, Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
	AddPlane(Vector3{ 5.f, 0.f, 0.f }, Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
	AddPlane(Vector3{ -5.f, 0.f, 0.f }, Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

	//Bunny
	pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	Utils::ParseOBJ("Resources/lowpoly_bunny.obj", pMesh->positions, pMesh->normals, pMesh->indices);

	pMesh->Scale({ 2.f, 2.f, 2.f });

	pMesh->UpdateAABB();
	pMesh->UpdateTransforms();

	//Light
	AddPointLight({ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f }); //Backlight
	AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, ColorRGB{ 1.f, .8f, .45f }); //Front Light left
	AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
}

void Scene_BunnyScene::Update(Timer* pTimer)
{
	Scene::Update(pTimer);
}
#pragma endregion
}

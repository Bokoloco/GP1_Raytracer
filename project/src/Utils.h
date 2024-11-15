#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			Vector3 raySphere{ ray.origin - sphere.origin };

			float B{ Vector3::Dot((2 * ray.direction), raySphere) };
			float C{ Vector3::Dot(raySphere, raySphere) - Square(sphere.radius) };
			float discriminant{ Square(B) - 4 * C};

			if (discriminant < 0)
				return false;
			
			float sqrtDiscriminant{ sqrt(discriminant) };

			float t{ (-B - sqrtDiscriminant) / 2 >= ray.min ? (-B - sqrtDiscriminant) / 2 : (-B + sqrtDiscriminant) / 2 };

			if (t < ray.min or t > ray.max )
				return false;

			if (!ignoreHitRecord)
			{
				hitRecord.t = t;
				hitRecord.didHit = true;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.origin = ray.origin + (t * ray.direction);
				Vector3 vectorHitToSphere{ hitRecord.origin - sphere.origin };
				hitRecord.normal = vectorHitToSphere.Normalized();
			}

			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			float t{ (Vector3::Dot((plane.origin - ray.origin), plane.normal)) / (Vector3::Dot(ray.direction, plane.normal))};

			if (t < ray.min or t >= ray.max)
				return false;

			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.t = t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.origin = ray.origin + (t * ray.direction);
				hitRecord.normal = plane.normal;
			}

			return true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion

#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float dotNormalDir{ Vector3::Dot(triangle.normal, ray.direction) };

			if (AreEqual(dotNormalDir, 0.f)) return false;
			
			if (!ignoreHitRecord)
			{
				if ((triangle.cullMode == TriangleCullMode::BackFaceCulling and dotNormalDir > 0)
					or (triangle.cullMode == TriangleCullMode::FrontFaceCulling and dotNormalDir < 0)) return false;
			}
			else
			{
				if ((triangle.cullMode == TriangleCullMode::BackFaceCulling and dotNormalDir <= 0)
					or (triangle.cullMode == TriangleCullMode::FrontFaceCulling and dotNormalDir >= 0)) return false;
			}

			Vector3 L{ triangle.v0 - ray.origin };
			float t{ Vector3::Dot(L, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal) };

			if (t < ray.min or t > ray.max) return false;

			Vector3 point{ ray.origin + (ray.direction * t) };

			/*Vector3 vectors[3]{ triangle.v0, triangle.v1, triangle.v2 };
			for (int vectorIdx{}; vectorIdx < 3; ++vectorIdx)
			{

				Vector3 e{ vectors[(vectorIdx + 1) % 3] - vectors[vectorIdx] };
				Vector3 p{ point - vectors[vectorIdx] };

				Vector3 cross{ Vector3::Cross(e, p) };

				if (Vector3::Dot(cross, triangle.normal) < 0) return false;
			}*/

			Vector3 e{ triangle.v1 - triangle.v0 };
			Vector3 p{ point - triangle.v0 };

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0) return false;

			e = triangle.v2 - triangle.v1;
			p = point - triangle.v1;

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0) return false;

			e = triangle.v0 - triangle.v2;
			p = point - triangle.v2;

			if (Vector3::Dot(Vector3::Cross(e, p), triangle.normal) < 0) return false;


			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				hitRecord.origin = ray.origin + (t * ray.direction);
				hitRecord.t = t;
			}

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}

			bool didHitMesh{};
			HitRecord closestHit{};
			for (int idx{}; idx < mesh.indices.size(); idx += 3)
			{
				Triangle triangle{ mesh.transformedPositions[mesh.indices[idx]], mesh.transformedPositions[mesh.indices[idx + 1]], mesh.transformedPositions[mesh.indices[idx + 2]], mesh.transformedNormals[idx / 3] };
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;

				if (HitTest_Triangle(triangle, ray, closestHit, ignoreHitRecord)) didHitMesh = true;
				if (!ignoreHitRecord)	if (closestHit.t < hitRecord.t and closestHit.didHit) hitRecord = closestHit;
			}

			return didHitMesh;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			if (light.type == LightType::Point)
				return { light.origin - origin };
			if (light.type == LightType::Directional)
				return light.direction;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			if (light.type == LightType::Point)
			{
				Vector3 lightDirection{ light.origin - target };
				float irradiance{ light.intensity / lightDirection.SqrMagnitude() };

				return { light.color * irradiance };
			}
			if (light.type == LightType::Directional)
			{
				return {light.color * light.intensity};
			}
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}
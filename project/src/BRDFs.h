#pragma once
#include "Maths.h"
#include <cmath>
#include <iostream>

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3
			ColorRGB rho{ kd * cd };
			return {rho / PI};
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3
			ColorRGB rho{ cd * kd };
			return {rho / PI};
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//todo: W3
			Vector3 reflect{ l - (2 * Vector3::Dot(l, n) * n)};
			float cos{ std::max(0.f,Vector3::Dot(reflect.Normalized(), v))};
			float phongSpecularReflection{ ks * std::powf(cos, exp) };
			return  phongSpecularReflection * ColorRGB{1.f, 1.f, 1.f};
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//todo: W3
			ColorRGB secondPartFunction{ (ColorRGB{1.f, 1.f, 1.f} - f0) * std::powf((1.f - Vector3::Dot(h, v)), 5) };
			return {f0 + secondPartFunction };
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			//todo: W3
			float a{ Square(roughness) };
			float denominator{ PI * Square(Square(Vector3::Dot(n, h)) * (Square(a) - 1.f) + 1.f) };
			return {Square(a) / denominator};
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			//todo: W3
			float a{ Square(roughness) };
			float k{ Square(a + 1.f) / 8.f };
			float dotNV{ Vector3::Dot(n, v) };
			float denominator{ (dotNV * (1.f - k)) + k };
			return {dotNV / denominator};
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			//todo: W3
			return {GeometryFunction_SchlickGGX(n, v, roughness) * GeometryFunction_SchlickGGX(n, l, roughness)};
		}

	}
}
#include "collision.h"
#include "cPhysicsComponents.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace std;
using namespace glm;
namespace collision {

	bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &c1, const cSphereCollider &c2) {
		const dvec3 p1 = c1.GetParent()->GetPosition();
		const dvec3 p2 = c2.GetParent()->GetPosition();
		const dvec3 d = p2 - p1;
		const double distance = glm::length(d);
		const double sumRadius = c1.radius + c2.radius;
		if (distance < sumRadius) {
			auto depth = sumRadius - distance;
			auto norm = -glm::normalize(d);
			auto pos = p1 - norm * (c1.radius - depth * 0.5f);
			civ.push_back({ &c1, &c2, pos, norm, depth });
			return true;
		}
		return false;
	}

	bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &s, const cPlaneCollider &p) {
		const dvec3 sp = s.GetParent()->GetPosition();
		const dvec3 pp = p.GetParent()->GetPosition();

		// Calculate a vector from a point on the plane to the center of the sphere
		const dvec3 vecTemp(sp - pp);

		// Calculate the distance: dot product of the new vector with the plane's normal
		double distance = dot(vecTemp, p.normal);

		if (distance <= s.radius) {
			civ.push_back({ &s, &p, sp - p.normal * distance, p.normal, s.radius - distance });
			return true;
		}

		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Copying the above for domino and plane
	bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cDominoCollider &d, const cPlaneCollider &p) {
		const dvec3 sp = d.GetParent()->GetPosition();
		const dvec3 pp = p.GetParent()->GetPosition();

		// Calculate a vector from a point on the plane to the center of the sphere
		const dvec3 vecTemp(sp - pp);

		// Calculate the distance: dot product of the new vector with the plane's normal
		double distance = dot(vecTemp, p.normal);

		if (distance <= (d.height * 0.5)) {
			civ.push_back({ &d, &p, sp - p.normal * distance, p.normal, (d.height * 0.5) - distance });
			//cout << "plane and domino collide" << endl;
			return true;
		}

		return false;
	}

	bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cSphereCollider &c1, const cBoxCollider &c2) {
		const dvec3 sp = c1.GetParent()->GetPosition();
		const dvec3 bp = c2.GetParent()->GetPosition();
		if (length(sp - bp) < c1.radius + c2.radius) {
			// TODO
			cout << "Sphere Box" << endl;
			// return true;
		}

		return false;
	}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cPlaneCollider &c1, const cPlaneCollider &c2) {
	// TODO
	cout << "PLANE PLANE" << endl;
	return false;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cPlaneCollider &p, const cBoxCollider &b) {
	const dvec3 pp = p.GetParent()->GetPosition();
	const dvec3 bp = b.GetParent()->GetPosition();

	// local coords on cube
	dvec3 points[8] = { dvec3(b.radius, b.radius, b.radius),   dvec3(-b.radius, b.radius, b.radius),
		dvec3(b.radius, -b.radius, b.radius),  dvec3(-b.radius, -b.radius, b.radius),
		dvec3(b.radius, b.radius, -b.radius),  dvec3(-b.radius, b.radius, -b.radius),
		dvec3(b.radius, -b.radius, -b.radius), dvec3(-b.radius, -b.radius, -b.radius) };

	// transfrom to global
	const mat4 m = glm::translate(bp) * mat4_cast(b.GetParent()->GetRotation());
	for (int i = 0; i < 8; i++) {
		points[i] = dvec3(m * dvec4(points[i], 1.0));
	}

	// For each point on the cube, which side of cube are they on?
	double distances[8];
	bool isCollided = false;
	for (int i = 0; i < 8; i++) {
		dvec3 planeNormal = p.normal;

		distances[i] = dot(pp, planeNormal) - dot(points[i], planeNormal);

		if (distances[i] > 0) {
			//	 cout << "CuboidPlane!\n";
			civ.push_back({ &p, &b, points[i] + planeNormal * distances[i], planeNormal, distances[i] });
			isCollided = true;
		}
	}
	return isCollided;
}

bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cBoxCollider &c1, const cBoxCollider &c2) {
	cout << "Box Box" << endl;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cDominoCollider &c1, const cDominoCollider &c2)
{

	//Trying to work out all the points of the domino (min/max x y and z)

	/*vec3 MinX1 = c1.GetParent->getPosition() - vec3(		(c1.width / 2),				0,					0				);
	vec3 MinY1 = c1.GetParent->getPosition() - vec3(			0,				(c1.height / 2),			0				);
	vec3 MinZ1 = c1.GetParent->getPosition() - vec3(			0,						0,				(c1.depth / 2)		);

	vec3 MaxX1 = c1.GetParent->getPosition() + vec3(		(c1.width / 2),				0,					0				);
	vec3 MaxY1 = c1.GetParent->getPosition() + vec3(			0,				(c1.height / 2),			0				);
	vec3 MaxZ1 = c1.GetParent->getPosition() + vec3(			0,						0,				(c1.depth / 2)		);



	vec3 MinX2 = c2.GetParent->getPosition() - vec3(		(c2.width / 2),				0,					0				);
	vec3 MinY2 = c2.GetParent->getPosition() - vec3(			0,				(c2.height / 2),			0				);
	vec3 MinZ2 = c2.GetParent->getPosition() - vec3(			0,						0,				(c2.depth / 2)		);

	vec3 MaxX2 = c2.GetParent->getPosition() + vec3(		(c2.width / 2),				0,					0				);
	vec3 MaxY2 = c2.GetParent->getPosition() + vec3(			0,				(c2.height / 2),			0				);
	vec3 MaxZ2 = c2.GetParent->getPosition() + vec3(			0,						0,				(c2.depth / 2)		);*/


	const dvec3 p1 = c1.GetParent()->GetPosition();
	const dvec3 p2 = c2.GetParent()->GetPosition();
	const dvec3 d = p2 - p1;

	const double distance = glm::length(d);
	const double sumZ = c1.depth + c2.depth;
	
	double MinX1 = c1.GetParent()->GetPosition().x - (c1.width / 2);
	double MinY1 = c1.GetParent()->GetPosition().y - (c1.height / 2);
	double MinZ1 = c1.GetParent()->GetPosition().z - (c1.depth / 2);

	double MaxX1 = c1.GetParent()->GetPosition().x + (c1.width / 2);
	double MaxY1 = c1.GetParent()->GetPosition().y + (c1.height / 2);
	double MaxZ1 = c1.GetParent()->GetPosition().z + (c1.depth / 2);



	double MinX2 = c2.GetParent()->GetPosition().x - (c2.width / 2);
	double MinY2 = c2.GetParent()->GetPosition().y - (c2.height / 2);
	double MinZ2 = c2.GetParent()->GetPosition().z - (c2.depth / 2);

	double MaxX2 = c2.GetParent()->GetPosition().x + (c2.width / 2);
	double MaxY2 = c2.GetParent()->GetPosition().y + (c2.height / 2);
	double MaxZ2 = c2.GetParent()->GetPosition().z + (c2.depth / 2);

	double MinMaxes[] = { MinX1, MinY1, MinZ1, MaxX1, MaxY1, MaxZ1, MinX2, MinY2, MinZ2, MaxX2, MaxY2, MaxZ2 };


	if (MaxX1 < MinX2 || MinX1 > MaxX2)
	{
		//cout << "Not colliding on X" << endl;
		return false;
	}

	if (MaxY1 < MinY2 || MinY1 > MaxY2)
	{
		//cout << "Not colliding on Y" << endl;
		return false;
	}

	if (MaxZ1 < MinZ2 || MinZ1 > MaxZ2)
	{
		//cout << "Not colliding on Z" << endl;
		return false;
	}
	cout << "Domino collision occurs" << endl;

	
	auto depth = sumZ - distance;
	auto norm = -glm::normalize(d);
	//auto pos = p1 - norm * ((c1.depth/2) - depth * 0.5f);
	auto pos = p1;
	civ.push_back({ &c1, &c2, pos, norm, depth });

	return true;


		//Code from Bounding Volumes Slides
		/*if (a.max[x] < b.min[x] || a.min > b.max[x])
		{
			return false;
		}
		if (a.max[y] < b.min[y] || a.min[y] > b.max[y])
		{
			return false;
		}
		if (a.max[z] < b.min[z] || a.min[z] > b.max[z])
		{
			return false;
		}
		cout << "Domino collides with Domino" << endl;
		return true; */







		//Code from Bounding Volumes Slides (AABB - AABB)
		/*if (c1.width < c2.width || c1.width > c2.width)
		{
			c1.GetParent()->GetPosition().x;

			cout << "Not colliding on width" << endl;
			return false;
		}
		if (c1.height < c2.height || c1.height > c2.height)
		{ 
			cout << "Not colliding on height" << endl;
			return false;
		}
		if (c1.depth < c2.depth || c1.depth > c2.depth)
		{
			cout << "Not colliding on depth" << endl;
			return false;
		}
		cout << "Domino collides with Domino" << endl;
		return true;*/
	}

	bool IsColliding(std::vector<collisionInfo> &civ, const cCollider &c1, const cCollider &c2) {
		enum shape { UNKOWN = 0, PLANE, SPHERE, BOX, DOMINO }; ///////////////////////////////////////////////////////////////////////////////////////added domino here, more work needed
		shape s1 = UNKOWN;
		shape s2 = UNKOWN;
		if (dynamic_cast<const cSphereCollider *>(&c1)) {
			s1 = SPHERE;
		}
		else if (dynamic_cast<const cPlaneCollider *>(&c1)) {
			s1 = PLANE;
		}
		else if (dynamic_cast<const cBoxCollider *>(&c1)) {
			s1 = BOX;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////attempting said work^
		else if (dynamic_cast<const cDominoCollider *>(&c1))
		{
			s1 = DOMINO;
		}

		if (dynamic_cast<const cSphereCollider *>(&c2)) {
			s2 = SPHERE;
		}
		else if (dynamic_cast<const cPlaneCollider *>(&c2)) {
			s2 = PLANE;
		}
		else if (dynamic_cast<const cBoxCollider *>(&c2)) {
			s2 = BOX;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////attempting said work^
		else if (dynamic_cast<const cDominoCollider *>(&c2))
		{
			s2 = DOMINO;
		}

		if (!s1 || !s2) {
			cout << "Routing Error" << endl;
			return false;
		}
		if (s1 == PLANE) {
			if (s2 == PLANE) {
				return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
			}
			else if (s2 == SPHERE) {
				return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
			}
			else if (s2 == BOX) {
				return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c1), dynamic_cast<const cBoxCollider &>(c2));
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////doesn't work cause I haven't set up plane to domino collisions yet
			else if (s2 == DOMINO)
			{
				cout << "s1 == " << s1 << " & s2 == " << s2 << endl;

				return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c2), dynamic_cast<const cPlaneCollider &>(c1));
				//return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
				//This is causing a break, dunno why
			}
			else {
				cout << "Routing Error" << endl;
				return false;
			}
		}
		else if (s1 == SPHERE) {
			if (s2 == PLANE) {
				return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cPlaneCollider &>(c2));
			}
			else if (s2 == SPHERE) {
				return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1),
					dynamic_cast<const cSphereCollider &>(c2));
			}
			else if (s2 == BOX) {
				return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cBoxCollider &>(c2));
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////doesn't work cause I haven't set up sphere to domino collisions yet
			/*else if (s2 == DOMINO)
			{
			return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c1), dynamic_cast<const cDominoCollider &>(c2));
			}*/
			else {
				cout << "Routing Error" << endl;
				return false;
			}
		}
		else if (s1 == BOX) {
			if (s2 == PLANE) {
				return IsCollidingCheck(civ, dynamic_cast<const cPlaneCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
			}
			else if (s2 == SPHERE) {
				return IsCollidingCheck(civ, dynamic_cast<const cSphereCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
			}
			else if (s2 == BOX) {
				return IsCollidingCheck(civ, dynamic_cast<const cBoxCollider &>(c2), dynamic_cast<const cBoxCollider &>(c1));
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////doesn't work cause I haven't set up box to domino collisions yet
			/*else if (s2 == DOMINO)
			{
			return IsCollidingCheck(civ, dynamic_cast<const cBoxCollider &>(c1), dynamic_cast<const cDominoCollider &>(c2));
			}*/
			else {
				cout << "Routing Error" << endl;
				return false;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Domino to domino collisions
		else if (s1 == DOMINO)
		{
			if (s2 == PLANE)
			{
				cout << "s1 == " << s1 << " & s2 == " << s2 << endl;

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////doesn't work cause I haven't set up plane to domino collisions yet
				return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c2), dynamic_cast<const cPlaneCollider &>(c1));
				///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////don't think it matters what way round you put the colliders?
			}
			//Chuck more shite in here for extra collisions but only arsed about domino to domino and domino to plane atm
			if (s2 == DOMINO)
			{
				cout << "s1 == " << s1 << " & s2 == " << s2 << endl;

				return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c2), dynamic_cast<const cDominoCollider &>(c1));
			}
			else
			{
				cout << "Routing Error" << endl;
				return false;
			}
		}
		return false;
	}
}
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////domino and plane collisions
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Domino to domino collisions
bool IsCollidingCheck(std::vector<collisionInfo> &civ, const cDominoCollider &c1, const cDominoCollider &c2)
{
	//Get the positions of each collider and find the distance between them
	const dvec3 p1 = c1.GetParent()->GetPosition();
	const dvec3 p2 = c2.GetParent()->GetPosition();
	const dvec3 d = p2 - p1;

	const double distance = glm::length(d);
	const double sumZ = c1.depth + c2.depth;
	
	//Get the minimum and maximum X, Y and Z values of each collider
	double MinX1 = c1.GetParent()->GetPosition().x - (c1.width);
	double MinY1 = c1.GetParent()->GetPosition().y - (c1.height);
	double MinZ1 = c1.GetParent()->GetPosition().z - (c1.depth);

	double MaxX1 = c1.GetParent()->GetPosition().x + (c1.width);
	double MaxY1 = c1.GetParent()->GetPosition().y + (c1.height);
	double MaxZ1 = c1.GetParent()->GetPosition().z + (c1.depth);


	double MinX2 = c2.GetParent()->GetPosition().x - (c2.width);
	double MinY2 = c2.GetParent()->GetPosition().y - (c2.height);
	double MinZ2 = c2.GetParent()->GetPosition().z - (c2.depth);

	double MaxX2 = c2.GetParent()->GetPosition().x + (c2.width);
	double MaxY2 = c2.GetParent()->GetPosition().y + (c2.height);
	double MaxZ2 = c2.GetParent()->GetPosition().z + (c2.depth);


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

	
	//Move the colliders back to where they should be after the collision
	auto depth = sumZ - distance;
	auto norm = -glm::normalize(d);
	auto pos = p1 - norm * ((c1.depth/2) - (depth * 0.5f));
	civ.push_back({ &c1, &c2, pos, norm, depth });

	return true;
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
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////plane to domino collisions
			else if (s2 == DOMINO)
			{
				//cout << "s1 == " << s1 << " & s2 == " << s2 << endl;
				return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c2), dynamic_cast<const cPlaneCollider &>(c1));
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
			/////////////////////////////////////////////////////////////////////////////////////doesn't work cause I haven't set up sphere to domino collisions yet
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
			else {
				cout << "Routing Error" << endl;
				return false;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////Domino to domino/plane collisions
		else if (s1 == DOMINO)
		{
			if (s2 == PLANE)
			{
				//cout << "s1 == " << s1 << " & s2 == " << s2 << endl;
				return IsCollidingCheck(civ, dynamic_cast<const cDominoCollider &>(c2), dynamic_cast<const cPlaneCollider &>(c1));
			}
			if (s2 == DOMINO)
			{
				//cout << "s1 == " << s1 << " & s2 == " << s2 << endl;
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
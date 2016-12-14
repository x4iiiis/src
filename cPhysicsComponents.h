#pragma once
#include "game.h"

class cParticle : public Component {
public:
  //
  glm::dvec3 position;
  glm::dvec3 prev_position;
  glm::dvec3 forces;
  double mass;
  double inversemass;
  double linearDamping;
  //
  cParticle(const std::string &tag = "Particle");
  ~cParticle();
  void Update(double delta);
  virtual void SetParent(Entity *p);
  virtual void AddLinearForce(const glm::dvec3 &i);
  virtual void AddLinearImpulse(const glm::dvec3 &i);
  virtual void Integrate(const double dt);

private:
};

class cRigidBody : public cParticle {
public:
  cRigidBody();
  ~cRigidBody();

  // ***** ANGULAR******
  glm::dquat orientation;
  glm::dmat3 localInvInertia;
  glm::dmat3 worldInvInertia;
  glm::dvec3 angVelocity;
  glm::dvec3 torques;
  double angularDamping;
  void Update(double delta);
  void AddForceAt(const glm::dvec3 &force, const glm::dvec3 &point);
  void AddAngularForce(const glm::dvec3 &i);
  virtual void ComputeLocalInvInertiaTensor();
  void Integrate(const double dt);
  void SetParent(Entity *p);

private:
};

class cRigidSphere : public cRigidBody {
public:
  double radius;
  void ComputeLocalInvInertiaTensor();
};

class cRigidCube : public cRigidBody {
public:
  double radius;
  void ComputeLocalInvInertiaTensor();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class cRigidDomino : public cRigidBody
{
public:
	double width = 1;
	double height = 2;
	double depth = 0.5;
	//double CenterOfMass;
	void ComputeLocalInvInertiaTensor();
};

class cRigidPlane : public cRigidBody {
public:
  void ComputeLocalInvInertiaTensor();
};

class cCollider : public Component {
public:
  cCollider(const std::string &tag);
  ~cCollider();
  void Update(double delta);

private:
};

class cSphereCollider : public cCollider {

public:
  double radius;
  cSphereCollider();

private:
};

class cBoxCollider : public cCollider {
public:
  double radius;
  //double xradius;
  //double yradius;
  //double zradius;
  cBoxCollider();

private:
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class cDominoCollider : public cCollider {
public:
	//double radius;
	double width;
	double height;
	double depth;

	/*vec3 MinX;
	vec3 MinY;
	vec3 MinZ;

	vec3 MaxX;
	vec3 MaxY;
	vec3 MaxZ;*/
	//double radius[3] = { xradius, yradius, zradius };
	//Point c; //center point of bounding box
	cDominoCollider();

private:
};


class cPlaneCollider : public cCollider {
public:
  glm::dvec3 normal;
  cPlaneCollider();

private:
};

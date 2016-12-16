#include "game.h"
#include <algorithm>
#include <glm/gtx/transform.hpp>
using namespace glm;
using namespace std;
Component::Component(const string &token) {
	token_ = token;
	Ent_ = nullptr;
	active_ = false;
}

Component::~Component() {}

bool Component::IsActive() { return active_; }

void Component::SetActive(bool b) { active_ = b; }

void Component::SetParent(Entity *p) { Ent_ = p; }

Entity *Component::GetParent() const { return Ent_; }

//############## Entity ###################

Entity::Entity() {
	visible_ = true;
	changed_ = true;
	//scale_ = vec3(1.0f, 1.0f, 1.0f);      Changed this to the below to make the objects the right size/shape
	scale_ = vec3(1.0, 2.0, 0.5);
	position_ = vec3(0.0f, 0.0f, 0.0f);
	rotation_ = quat();
	colour = RED;
}

Entity::~Entity() {}

const dvec3 Entity::GetScale() const { return scale_; }

const dvec3 Entity::GetPosition() const { return position_; }

const dquat Entity::GetRotation() const { return rotation_; }
const dvec3 Entity::GetRotationV3() const { return glm::eulerAngles(GetRotation()); }

const dmat4 Entity::GetTranform() {
	if (changed_) {
		mat4 scl = scale(scale_);
		mat4 rot = mat4_cast(rotation_);
		mat4 trn = translate(position_);
		transform_ = (trn * rot * scl);
		changed_ = false;
	}
	return transform_;
}

const bool Entity::IsVisible() const { return false; }

const string Entity::GetName() const { return name_; }

void Entity::SetTransform(const dmat4 m4) { assert(false); }

void Entity::SetScale(const dvec3 &v3) {
	scale_ = v3;
	changed_ = true;
}

void Entity::SetPosition(const dvec3 &v3) {
	position_ = v3;
	changed_ = true;
}

void Entity::SetRotation(const dvec3 &v3) {
	rotation_ = glm::quat(v3);
	changed_ = true;
}

void Entity::SetRotation(const dquat &q) {
	rotation_ = q;
	changed_ = true;
}

void Entity::SetVisibility(const bool b) {}

void Entity::SetName(string const &name) { name_ = name; }

void Entity::Update(const double delta) {
	for (auto &c : components_) {
		c->Update(delta);
	}
}

void Entity::Render() {
	for (auto &c : components_) {
		c->Render();
	}
}

void Entity::AddComponent(unique_ptr<Component> &c) {
	c->SetParent(this);
	components_.push_back(move(c));
}

void Entity::RemoveComponent(Component &c) {
	// Todo: Test This
	auto position =
		find_if(components_.begin(), components_.end(), [c](unique_ptr<Component> &p) { return p.get() == &c; });
	if (position != components_.end()) {
		components_.erase(position);
	}
}

// be careful what you do with this function...
vector<Component *> Entity::GetComponents(string const &name) const {
	vector<Component *> list;
	if (components_.size() < 1) {
		return list;
	}
	for (auto &c : components_) {
		if (c->token_ == name) {
			list.push_back(c.get()); // It's not like we want to make safe programs anyway...
		}
	}
	return list;
}

const vector<unique_ptr<Component>> *Entity::GetComponents() const { return &components_; }

//############## Shape Renderer ###################

void cShapeRenderer::SetColour(const phys::RGBAInt32 c) { col_ = c; }

cShapeRenderer::cShapeRenderer(SHAPES s) : shape(s), col_(RED), Component("ShapeRenderer") {}

cShapeRenderer::~cShapeRenderer() {}

void cShapeRenderer::Update(double delta) {}

void cShapeRenderer::Render() {
	const mat4 m = glm::scale(Ent_->GetScale()) *  glm::translate(Ent_->GetPosition()) * mat4_cast(Ent_->GetRotation());

	switch (shape) {
	case SPHERE:
		phys::DrawSphere(m, col_);
		break;
	case BOX:
		phys::DrawCube(m, col_);
		break;
	default:
		phys::DrawSphere(Ent_->GetPosition(), 1.0f, col_);
		break;
	}
}

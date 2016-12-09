#include "game.h"
#include "physics.h"
#include "cPhysicsComponents.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include <phys_utils.h>


using namespace std;
using namespace graphics_framework;
using namespace glm;
#define physics_tick 1.0 / 60.0




free_camera freecam;
target_camera targetcam;

//Use targetcam to begin with (change to 0, using freecam for speed of testing)
int Cam = 1;

//Used for rotating freecam based on cursor position
double xpos = 0.0f;
double ypos = 0.0f;



//Entity shite
static vector<unique_ptr<Entity>> SceneList;

static unique_ptr<Entity> floorEnt;

unique_ptr<Entity> CreateParticle (float xPos, float yPos, float zPos, phys::RGBAInt32 c)
{
	unique_ptr<Entity> ent(new Entity());
	ent->SetPosition(vec3(xPos, yPos, zPos));
	unique_ptr<Component> physComponent(new cRigidCube());
	unique_ptr<cShapeRenderer> renderComponent(new cShapeRenderer(cShapeRenderer::BOX));
	renderComponent->SetColour(c);
	ent->AddComponent(physComponent);
	ent->AddComponent(unique_ptr<Component>(new cBoxCollider()));
	ent->AddComponent(unique_ptr<Component>(move(renderComponent)));
	return ent;
}




bool load_content() {
	phys::Init();

	floorEnt = unique_ptr<Entity>(new Entity());
	floorEnt->AddComponent(unique_ptr<Component>(new cPlaneCollider()));

	for (float z = 23; z > 1; z = z - 2)
	{
		unique_ptr<Entity> particle = CreateParticle(2.0, 1.0f, z, WHITE);
		SceneList.push_back(move(particle));
	}	


	////NAPIER TRIANGLE
	////Row 1
	/*SceneList.push_back(move(CreateParticle(2, 0.5, 0.5, RED)));
	////Extender
	SceneList.push_back(move(CreateParticle(2.5, 0.5, -0.75, RED)));
	////Row 2
	SceneList.push_back(move(CreateParticle(2, 0.5, -2, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -2, RED)));
	////Extender 2
	SceneList.push_back(move(CreateParticle(3.5, 0.5, -3.25, RED)));
	////Row 3
	SceneList.push_back(move(CreateParticle(2, 0.5, -4.5, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -4.5, RED)));
	SceneList.push_back(move(CreateParticle(4.2, 0.5, -4.5, RED)));
	////Extender 3
	SceneList.push_back(move(CreateParticle(4.6, 0.5, -5.75, RED)));
	////Row 4
	SceneList.push_back(move(CreateParticle(2, 0.5, -7, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -7, RED)));
	SceneList.push_back(move(CreateParticle(4.2, 0.5, -7, RED)));
	SceneList.push_back(move(CreateParticle(5.4, 0.5, -7, RED)));
	////Extender 4
	SceneList.push_back(move(CreateParticle(6, 0.5, -8.25, RED)));
	////Row 5
	SceneList.push_back(move(CreateParticle(2, 0.5, -9.5, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -9.5, RED)));
	SceneList.push_back(move(CreateParticle(4.2, 0.5, -9.5, RED)));
	SceneList.push_back(move(CreateParticle(5.4, 0.5, -9.5, RED)));
	SceneList.push_back(move(CreateParticle(6.6, 0.5, -9.5, RED)));
	////Extender 5
	SceneList.push_back(move(CreateParticle(7, 0.5, -10.75, RED)));
	////Row 6
	SceneList.push_back(move(CreateParticle(2, 0.5, -12, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -12, RED)));
	SceneList.push_back(move(CreateParticle(4.2, 0.5, -12, RED)));
	SceneList.push_back(move(CreateParticle(5.4, 0.5, -12, RED)));
	SceneList.push_back(move(CreateParticle(6.6, 0.5, -12, RED)));
	SceneList.push_back(move(CreateParticle(7.8, 0.5, -12, RED)));
	////Extender 6
	SceneList.push_back(move(CreateParticle(8.2, 0.5, -13.25, RED)));
	////Row 7
	SceneList.push_back(move(CreateParticle(2, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(3.1, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(4.2, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(5.4, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(6.6, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(7.8, 0.5, -14.5, RED)));
	SceneList.push_back(move(CreateParticle(9, 0.5, -14.5, RED)));*/



	//Initial Target Camera position
	targetcam.set_target(vec3(15, 0, -25));
	targetcam.set_position(vec3(-20, 10, 10));
	//Initial Free Camera position
	freecam.set_target(vec3(15, 0, 25));
	freecam.set_position(vec3(2, 3, 30));

	return true;
}

bool update(float delta_time) {
	static float rot = 0.0f;
	rot += 0.2f * delta_time;

	//Code for spinning camera that was here by default
	//phys::SetCameraPos(rotate(vec3(15.0f, 12.0f, 15.0f), rot, vec3(0, 1.0f, 0)));
	//phys::SetCameraTarget(targetcam.get_target());
	//phys::SetCameraPos(targetcam.get_position());




	//ahghajkghajghjhagjahklhajglajgklaghlkaghajghjahgkhakghlakhghalkghklahlkghakhglkahlakhglkahgklhalghkhalag
	static double t = 0.0;
	static double accumulator = 0.0;
	accumulator += delta_time;

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) 
	{
		for (auto &e : SceneList) 
		{
			auto b = e->getComponent<cRigidCube>();

			if (b != NULL) 
			{
				b->AddAngularForce({ 0, 0, 5.0 });
			}
		}
	}

	while (accumulator > physics_tick)
	{
		UpdatePhysics(t, physics_tick);
		accumulator -= physics_tick;
		t += physics_tick;
	}
	
	//Entity shite



	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		Cam = 0;
		targetcam.set_target(vec3(15, 0, -25));
		targetcam.set_position(vec3(-20, 10, 10));
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		Cam = 0;
		targetcam.set_target(vec3(15, 0, -25));
		targetcam.set_position(vec3(-80, 10, 10));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3))
	{
		Cam = 0;
		targetcam.set_target(vec3(15, 0, -25));
		targetcam.set_position(vec3(-80, 70, 10));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4))
	{
		Cam = 0;
		targetcam.set_target(vec3(15, 0, -25));
		targetcam.set_position(vec3(-45, 30, 10));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_5))
	{
		Cam = 0;
		targetcam.set_target(vec3(15, 5, -25));
		targetcam.set_position(vec3(-80, 10, 90));
	}

	//if 0 is pressed initialise freecam **not initialised yet
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_0))
	{
		Cam = 1;
	}

	//while freecam is in use
	if (Cam == 1)
	{
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Mouse cursor is disabled and used to rotate freecam

																					 //Get mouse position for freecam
		double deltax, deltay;
		glfwGetCursorPos(renderer::get_window(), &deltax, &deltay);
		double tempx = deltax;
		double tempy = deltay;
		deltax -= xpos;
		deltay -= ypos;
		double ratio_width = (double)renderer::get_screen_width() / (double)renderer::get_screen_height();
		double ratio_height = 1.0 / ratio_width;
		deltax *= ratio_width * delta_time / 10;
		deltay *= -ratio_height * delta_time / 10;

		//Rotate freecam based on the mouse coordinates
		freecam.rotate(deltax, deltay);

		// set last cursor pos
		xpos = tempx;
		ypos = tempy;

		//Movement (freecam)
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
		{
			freecam.move(vec3(0.0f, 0.0f, 10.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
		{
			freecam.move(vec3(0.0f, 0.0f, -10.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
		{
			freecam.move(vec3(-10.0f, 0.0f, 0.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
		{
			freecam.move(vec3(10.0f, 0.0f, 0.0f) * delta_time);
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
		{
			freecam.move(vec3(0.0f, 10.0f, 0.0f) * delta_time);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN))
		{
			freecam.move(vec3(0.0f, -10.0f, 0.0f) * delta_time);
		}
		//Update freecam
		freecam.update(delta_time);

		phys::SetCameraTarget(freecam.get_target());
		phys::SetCameraPos(freecam.get_position());
	}

	if (Cam == 0)
	{
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL); //Mouse cursor is disabled and used to rotate freecam
		phys::SetCameraTarget(targetcam.get_target());
		phys::SetCameraPos(targetcam.get_position());
		targetcam.update(delta_time);
	}

	for (auto &e : SceneList)
	{
		e->Update(delta_time);
	}

	phys::Update(delta_time);
	return true;
}

bool render()
{
	for (auto &e : SceneList) 
	{
		e->Render();
	}

	phys::DrawScene();
	return true;
}

void main()
{
	// Create application
	app application;
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}
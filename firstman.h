#pragma once
#include <glew.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
using namespace glm;

class FirstMan {
public:
	FirstMan(vec3 position, vec3 forwardVector, vec3 speed, vec3 upVector) :
		position(position), forwardVector(forwardVector), speed(speed), upVector(upVector) {};
	void setPosition(vec3 position)	{	this->position = position;	}
	vec3 getPosition() { return position; }
	vec3* getPtrPos() { return &position; }
	void setForwardVector(vec3 forward) 
	{ 
		this->forwardVector = forward; 
		rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));	
	}
	void setForwardVectorRecur(mat4 &frans)
	{
		this->forwardVector = (frans*vec4(forwardVector, 0.0f)).xyz;
		rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
	}
	vec3 getForwardVector() { return forwardVector; }
	vec3* getPtrFor() { return &forwardVector; }
	vec3 getRightVector() { return rightVector; }
	vec3* getPtrRig() { return &rightVector; }
	void setSpeed(vec3 speed) { this->speed = speed; }
	vec3 getSpeed() { return speed; }
	vec3* getPtrSpeed() { return &speed; }
	vec3 getUpvector() { return upVector; }
	vec3* getPtrUp() { return &upVector; }
	mat4 getViewMatrix(){return lookAt(position, position + forwardVector, upVector);}
	void setMove(vec3 forward) { this->position += forward; }
private:
	vec3 forwardVector;
	vec3 position;
	vec3 speed;
	vec3 upVector;
	vec3 rightVector;
};

class Camera {
public:
	Camera(FirstMan* firstMan) :firstMan(firstMan),manView(1) 
	{
		this->upVector = vec3(0, 1, 0);
	};
	mat4* getViewPtr() { return &view; }
	mat4 getView() { return view; }
	vec3* getPosPtr() { return &position; }
	vec3 getPosition() { return position; }
	vec3* getForPtr() { return &forward; }
	vec3 getForward() { return forward; }
	vec3* getUpPtr() { return &upVector; }
	vec3 getUpVector() { return upVector; }
	void setManView()
	{
		int i = manView;
		switch (i)
		{
		case 1:
			position = firstMan->getPosition() - vec3(5)*firstMan->getForwardVector();
			forward = firstMan->getForwardVector();
			view = lookAt(firstMan->getPosition() - vec3(5)*firstMan->getForwardVector(), firstMan->getPosition(), firstMan->getUpvector());
			break;
		default:
			position = firstMan->getPosition() + vec3(5)*firstMan->getForwardVector();
			forward = -firstMan->getForwardVector();
			view = lookAt(firstMan->getPosition() + vec3(5)*firstMan->getForwardVector(), firstMan->getPosition(), firstMan->getUpvector()); 
			break;
		}
	}
	void setMan()
	{
		manView = (manView + 1) % 2;
	}
private:
	FirstMan* firstMan;
	int manView;
	mat4 view;
	vec3 position;
	vec3 forward;
	vec3 upVector;
};


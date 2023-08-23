#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

public: // make private when finished
	glm::vec3 mWorldUp;
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mVelocity;
	float mMoveSpeed;
	float mLookSpeed;
	float mPitch;
	float mYaw;
	float mPlayerHeight;
	void updateVectors();

public:
	Camera();
	void Move(float dx, float dy, float dt);
	void UpDown(int direction, float dt);
	void Rotate(float dx, float dy, float dt);
	glm::vec3 GetPosition();
	glm::vec3 GetTarget();
	glm::vec3 GetUp();
};
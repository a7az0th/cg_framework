#pragma once

#include "vector.h"
#include "defs.h"

// A class that represents a simple rectangular pinhole camera.
class Camera {
public:
	void init(int w = 640, int h = 480);
	Camera();
	Ray getCameraRay(int x,int y);
	float getRoll();
	float getPitch();
	float getYaw();
	Vector getFront();
	Vector getRight();
	Vector getUp();
	Vector getPos();
	Vector getDir();
	int getWidth();
	int getHeight();
	void zoomIn();
	void zoomOut();
	void rotateCamera(float, float, float);
	void moveCameraAbsolute(const Vector &);
	void moveCameraRelative(const Vector &);
	void moveCameraGameLike(const Vector &);
	void resetDefaults();
	void lockCamera();
	void unlockCamera();
	bool isCameraLocked();
	void switchCameraLock();

private:
	int width;
	int height;
	float fov;
	float aspect;

	float roll;
	float pitch;
	float yaw;
	float sensorWidth;
	float sensorHeight;
	Vector pos;
	Vector cameraRight;
	Vector cameraUp;
	Vector cameraFront;

	Vector sensorTopLeft;
	Vector sensorTopRight;
	Vector sensorBotLeft;
};

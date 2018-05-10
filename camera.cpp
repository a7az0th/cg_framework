#include "camera.h"
#include "matrix.h"

// The default constructor for the class
// It initializes all camera parameters with default values and precomputes
// the location and size of the virtual image sensor.
// The virtual image sensor is a rectangle that is perpendicular to the direction of the camera
// And is situated in front of the camera at distance 1.
// The aspect ratio of the camera is determined by the predefined image width and height
// The size of the virtual image sensor depends on both the aspect ratio and the FOV angle.
Camera::Camera() {
	width  = 640;
	height = 480;
	fov    = 90;
	yaw    = 0.0f;
	pitch  = 0.0f;
	roll   = 0.0f;
	pos    = Vector(0.0f, -3.0f, 0.0f);
}

void Camera::init(int w, int h) {
	width = w;
	height = h;
	aspect = (float)width / height;
	float diag = tanf(toRadians(fov / 2));

	Matrix rotate =  rotateAroundX(pitch)
				   * rotateAroundY(roll)
				   * rotateAroundZ(yaw);

	sensorHeight = sqrtf((diag * diag) / (aspect * aspect + 1.0f));
	sensorWidth = sensorHeight * aspect;

	cameraRight = Vector(1, 0, 0) * rotate;
	cameraFront = Vector(0, 1, 0) * rotate;
	cameraUp    = Vector(0, 0, 1) * rotate;

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Given the indices of a pixel in the final image, the function generates and
/// returns a ray that starts at the location of the camera and goes through the
/// top-left corner of the corresponding pixel in the virtual image sensor.
Ray Camera::getCameraRay(int x, int y)
{
	Vector dir;

	float w = float(x) / width;
	float h = float(y) / height;

	Vector hor_offset;
	Vector ver_offset;

	hor_offset = w * (sensorTopRight - sensorTopLeft);
	ver_offset = h * (sensorBotLeft  - sensorTopLeft);

	dir = (sensorTopLeft + hor_offset + ver_offset) - pos;
	dir.normalize();

	Ray ray;
	ray.origin = pos;
	ray.dir = dir;
	ray.depth = 0;

	return ray;
}

/// Function is responsible for handling camera rotation
/// Function takes as input 3 numbers that represent rotation in all 3 axis
/// The roll, pitch and yaw angles of the camera are then increased by the ammounts given.
/// NOTE: We do not allow the camera to flip so up and down movements
/// (represented with the pitch angle) are limited to [-90:90]
void Camera::rotateCamera(float r, float p, float y) {
	roll  += r;
	pitch += p;
	yaw   += y;

	//Do not allow camera to flip
	if (pitch > 90)  pitch = 90;
	if (pitch < -90) pitch = -90;

	const Matrix rotate =  
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	cameraRight = Vector(1, 0, 0) * rotate;
	cameraFront = Vector(0, 1, 0) * rotate;
	cameraUp    = Vector(0, 0, 1) * rotate;

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Function is responsible for moving the camera to a specified location given with Vector v
/// Function takes as input one vector - v and translates the camera to that position.
void Camera::moveCameraAbsolute(const Vector & v)
{
	const Vector translation = pos - v;
	const Matrix rotate = 
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	cameraRight = Vector(1, 0, 0) * rotate;
	cameraFront = Vector(0, 1, 0) * rotate;
	cameraUp    = Vector(0, 0, 1) * rotate;

	pos += translation;

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;

}

/// Function is responsible for moving the camera in units relative to its internal Front, Up and Right direction
/// Function takes as input one vector - v and uses its components to describe how much the camera should move
/// in each direction. The final camera movement is calculated by adding up the movements in each separate axis
/// Final Camera position = Original camera position + v.x * ForwardDirection + v.y * RightDirection + v.z * UpDirection;
void Camera::moveCameraRelative(const Vector & v) {
	const Matrix rotate =  
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	cameraRight = Vector(1, 0, 0) * rotate;
	cameraFront = Vector(0, 1, 0) * rotate;
	cameraUp    = Vector(0, 0, 1) * rotate;

	const Vector translate(v.x * cameraRight + v.y * cameraFront + v.z * cameraUp);

	pos += translate;

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Function is responsible for moving the camera in units relative to the global Front, Up and Right direction
/// Function takes as input one vector - v and uses its components to describe how much the camera should move
/// in each direction. The final camera movement is calculated by adding up the movements in each separate axis
/// Final Camera position = Original camera position + v.x * ForwardDirection + v.y * RightDirection + v.z * UpDirection;
void Camera::moveCameraGameLike(const Vector & v)
{
	const Matrix rotate = 
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	cameraRight = Vector(1, 0, 0) * rotate;
	cameraFront = Vector(0, 1, 0) * rotate;
	cameraUp    = Vector(0, 0, 1) * rotate;

	Vector translate(v.x * cameraRight + v.y * cameraFront + v.z * cameraUp);

	/// Do not allow movement on the Z axis.
	translate.z = 0.0f;

	pos += translate;

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Function resets all camera parameters to their default values.
void Camera::resetDefaults()
{
	//width = 640;
	//height = 480;
	//fov = 90;
	pos = Vector(0.0f, 0.0f, 0.0f);
	yaw = pitch = roll = 0.0f;

	aspect = (float)width / height;
	const float diag = tanf(toRadians(fov / 2));

	sensorHeight = sqrtf(sqr(diag) / (sqr(aspect) + 1.0f));
	sensorWidth = sensorHeight * aspect;

	cameraRight = Vector(1, 0, 0);
	cameraFront = Vector(0, 1, 0);
	cameraUp    = Vector(0, 0, 1);

	sensorTopLeft  = pos + Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = pos + Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = pos + Vector(-sensorWidth, 1.0f, -sensorHeight);
}

/// Function implements the effect of zooming in on the objects in the direction the camera is facing
/// Function decreases camera FOV by 1% of the current value
/// and then recalculates all the camera parameters that would be affected by this change.
void Camera::zoomIn() {
	fov -= fov * 0.01f;
	const float diag = tanf(toRadians(fov / 2));

	sensorHeight = sqrtf((diag * diag) / (aspect * aspect + 1.0f));
	sensorWidth = sensorHeight * aspect;

	const Matrix rotate = 
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Function implements the effect of zooming out on the objects in the direction the camera is facing
/// Function increases camera FOV by 1% of the current value
/// and then recalculates all the camera parameters that would be affected by this change.
/// NOTE: FOV cannot be increased to a value greater than 180 degrees.
void Camera::zoomOut()
{
	fov += fov * 0.01f;
	if (fov > 180) fov = 180;
	const float diag = tanf(toRadians(fov / 2));

	sensorHeight = sqrtf((diag * diag) / (aspect * aspect + 1.0f));
	sensorWidth = sensorHeight * aspect;

	const Matrix rotate = 
	     rotateAroundX(pitch)
	   * rotateAroundY(roll)
	   * rotateAroundZ(yaw);

	sensorTopLeft  = Vector(-sensorWidth, 1.0f,  sensorHeight);
	sensorTopRight = Vector( sensorWidth, 1.0f,  sensorHeight);
	sensorBotLeft  = Vector(-sensorWidth, 1.0f, -sensorHeight);

	sensorTopLeft  = sensorTopLeft  * rotate;
	sensorTopRight = sensorTopRight * rotate;
	sensorBotLeft  = sensorBotLeft  * rotate;

	sensorTopLeft  += pos;
	sensorTopRight += pos;
	sensorBotLeft  += pos;
}

/// Function returns the value of camera Roll angle
/// The Roll angle describes camera rotation around the Z axis
float Camera::getRoll() {
	return roll;
}

/// Function returns the value of camera Pitch angle
/// The Pitch angle describes camera rotation around the X axis
float Camera::getPitch() {
	return pitch;
}

/// Function returns the value of camera Yaw angle
/// The Yaw angle describes camera rotation around the Y axis
float Camera::getYaw() {
	return yaw;
}

Vector Camera::getFront() {
	return cameraFront;
}

Vector Camera::getRight() {
	return cameraRight;
}

Vector Camera::getUp() {
	return cameraUp;
}

Vector Camera::getPos() {
	return pos;
}

Vector Camera::getDir() {
	return cameraFront;
}

int Camera::getWidth() {
	return width;
}

int Camera::getHeight() {
	return height;
}

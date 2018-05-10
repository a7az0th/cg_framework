#include <stdio.h>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <GLUT/glut.h>
#else
	#ifdef WIN32
		#include <GL/glut.h>
	#else
		#ifdef __linux__
			#include <GL/gl.h>
			#include <GL/glut.h>
		#else
			#error "Unknown platform!"
	#endif //UNIX
	#endif //WIN32
#endif //APPLE

#include "color.h"
#include "camera.h"
#include "sphere.h"

struct Canvas {
	Canvas(int width, int height): width(width), height(height) {
		buffer = new Color[width*height];
	}
	~Canvas() {
		delete [] buffer;
		buffer = nullptr;
		width = height = 0;
	}

	int width;
	int height;
	Color *buffer;
};


Camera cam;
Sphere sphere;
Canvas* gCanvas = nullptr;

struct Light {
	Light() {
		pos = Vector(0, 0, -10);
		col = Color(1, 1, 1);
		intensity = 50;
	}

	float intensity;
	Color col;
	Vector pos;
};

Light light;

inline Vector getReflectionDir(const Vector &viewDir, const Vector& normal) {
	const float cos = -dot(viewDir, normal);
	return viewDir + normal*2*cos;
}

Color lambert(const Color &c, IntersectionInfo& info) {

	const int numLights = 1;
	const int numSamples = 1;
	const Color lightColor = light.col * light.intensity;
	const Vector pointOnLight = light.pos;

	const Vector lightVec = (pointOnLight - info.intersectionPoint).normalize();

	const Vector from = info.intersectionPoint + info.normal * 1e-6f;
	const Vector to   = pointOnLight;

	Color lambertComponent  = Color(0.0f, 0.0f, 0.0f);
	const float cosTheta  = dot(lightVec, info.normal);
	const Color lightContribution = c * lightColor * max(0, cosTheta) / (from - to).lengthSqr();
	lambertComponent += lightContribution / numSamples;
	lambertComponent = lambertComponent / numLights;

	Color specularComponent(0,0,0);
	const bool phong = 1;
	if (phong) {
		const Vector reflect = getReflectionDir(lightVec, info.normal);
		const Vector viewDir = (info.intersectionPoint - cam.getPos()).normalize();
		const float factor = max(0.f, dot(reflect, viewDir));

		specularComponent = lightColor * pow(factor, 30);
	}

	const float& AMBIENT_LIGHT = 0.1f;
	const Color ambientComponent  = c * AMBIENT_LIGHT;

	return ambientComponent + (lambertComponent+specularComponent) * (1.f - AMBIENT_LIGHT);
}



void raytrace(Canvas& c) {
	for (int y = 0; y < c.height; y++) {
		for (int x = 0; x < c.width; x++) {
			Color& col = c.buffer[y*c.width + x];
			Ray r = cam.getCameraRay(x, y);
			IntersectionInfo info;
			sphere.intersect(r, info);

			if (info.isValid()) {
				col = lambert(RED, info);
			} else {
				col = WHITE*0.3f;
			}
		}
	}
};

float angle = 0.f;

void display() {
	Canvas &c = *gCanvas;

	raytrace(c);
	glDrawPixels(c.width, c.height, GL_RGB, GL_FLOAT ,(float*)c.buffer);
	glutSwapBuffers();


	const float radius = 5;
	const float x = cosf(angle)*radius;
	const float y = sinf(angle)*radius;

	angle += pi() / 80;
	if (angle > pi()*2) {
		angle = 0;
	}

	Vector v(x, y, -5);
	light.pos.x = x;
	light.pos.y = y;
	light.pos.z = -5;

	glutPostRedisplay();
	//glFlush();
}


int main(int argc, char ** argv) {

	sphere.setRadius(1.f);

	Canvas c(640, 480);
	cam.init(c.width, c.height);
	gCanvas = &c;


	glutInit(&argc, argv);                 // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(c.width, c.height);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutMainLoop();

	return 0;
}

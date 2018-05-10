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

#include "threadman.h"
#include "timer.h"

#include <vector>

// Simple structure used to represent a rectangular section of an image. A 'bucket'
struct Rect {

	int x0, y0, x1, y1; //< The 2 diagonal points of the rectangle

	Rect() {}
	Rect(int x0, int y0, int x1, int y1) : x0(x0), x1(x1), y0(y0), y1(y1){}
	// Clips the rectangle against image size
	void clip(int maxX, int maxY) {
		x1 = min(x1, maxX);
		y1 = min(y1, maxY);
	}
};

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

struct Scene {
	Scene() {
		numThreads = a7az0th::getProcessorCount();
	}

	a7az0th::ThreadManager threadman;
	int numThreads;

	Camera cam;
	Sphere sphere;
	Canvas *c;
	std::vector<Rect> buckets;
};

Scene scene;



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

	Color lambertComponent = Color(0.0f, 0.0f, 0.0f);
	const float cosTheta  = dot(lightVec, info.normal);
	const Color lightContribution = c * lightColor * max(0, cosTheta) / (from - to).lengthSqr();
	lambertComponent += lightContribution / numSamples;
	lambertComponent = lambertComponent / numLights;

	Color specularComponent(0,0,0);
	const bool phong = 1;
	if (phong) {
		const Vector reflect = getReflectionDir(lightVec, info.normal);
		const Vector viewDir = (info.intersectionPoint - scene.cam.getPos()).normalize();
		const float factor = max(0.f, dot(reflect, viewDir));

		specularComponent = lightColor * pow(factor, 30);
	}

	const float& AMBIENT_LIGHT = 0.1f;
	const Color ambientComponent  = c * AMBIENT_LIGHT;

	return ambientComponent + (lambertComponent+specularComponent) * (1.f - AMBIENT_LIGHT);
}


void initBuckets(Canvas& c, std::vector<Rect>& buckets, const int BUCKET_SIZE = 32) {

	buckets.clear();

	const int W = c.width;
	const int H = c.height;
	const int BW = (W + BUCKET_SIZE-1) / BUCKET_SIZE;
	const int BH = (H + BUCKET_SIZE-1) / BUCKET_SIZE;
	for (int y = 0; y < BH; y++) {
		if (y % 2 == 0)
			for (int x = 0; x < BW; x++)
				buckets.push_back(Rect(x * BUCKET_SIZE, y * BUCKET_SIZE, (x + 1) * BUCKET_SIZE, (y + 1) * BUCKET_SIZE));
		else
			for (int x = BW - 1; x >= 0; x--)
				buckets.push_back(Rect(x * BUCKET_SIZE, y * BUCKET_SIZE, (x + 1) * BUCKET_SIZE, (y + 1) * BUCKET_SIZE));
	}
	for (int i = 0; i < (int) buckets.size(); i++) {
		buckets[i].clip(W, H);
	}
}

struct MultiThreadedRender : a7az0th::MultiThreadedFor {
	MultiThreadedRender(std::vector<Rect>& buckets, Canvas& c): buckets(buckets), c(c) {}
	virtual void body(int index, int threadIdx, int numThreads) override {
		const Rect& r = buckets[index];
		for (int y=r.y0; y < r.y1; y++) {
			for (int x = r.x0; x < r.x1; x++) {
				Color& col = c.buffer[y*c.width + x];
				const Ray& r = scene.cam.getCameraRay(x, y);
				IntersectionInfo info;
				scene.sphere.intersect(r, info);

				if (info.isValid()) {
					col = lambert(RED, info);
				} else {
					col = WHITE*0.3f;
				}
			}
		}
	}
private:
	std::vector<Rect>& buckets;
	Canvas& c;
};


void raytrace(Scene& scene) {
	MultiThreadedRender renderer(scene.buckets, *scene.c);
	renderer.run(scene.threadman, scene.buckets.size(), scene.numThreads);
};

void display() {

	a7az0th::Timer t;
	raytrace(scene);
	t.stop();
	printf("Frame rendered in %f seconds\r", t.elapsedSeconds());
	glDrawPixels(scene.c->width, scene.c->height, GL_RGB, GL_FLOAT ,(float*)scene.c->buffer);
	glutSwapBuffers();

	static float angle = 0.f;
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

	Canvas c(640, 480);
	scene.cam.init(c.width, c.height);
	scene.c = &c;
	initBuckets(c, scene.buckets);


	glutInit(&argc, argv);                 // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(c.width, c.height);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutMainLoop();

	return 0;
}

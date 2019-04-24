#pragma once
#include "SPHParticle.h"
#include "Grid.h"
#include "Plane.h"
using namespace std;

class Water
{
public:

	Grid* grid;
	float support = 2.0f;
	float smooth = 1.0f;
	float space = 0.5f;
	float density;
	float pressure;
	float k;
	float restDensity;
	//viscosity
	float v;
	void initialize();
	void update(float dt);
	void setup();
	void render(mat4 view, mat4 proj);
	void findNeightbors(SPHParticle* p);
	float W(float q);
	float f(float q);
	float fd(float q);
	float Wd(float q);
	void computeDensityAndPressure();
	void computeForces();
	void collisionDetection();
	void reset();
	vec3 Normalize(vec3 in);
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<GLuint> indices;
	vector<SPHParticle*> particles;
	vector<Plane* > walls;
	int numParticles;
	Shader shader = Shader("shader.vs.glsl", "shader.frag.glsl");
	Water();
	Water(int n);
	void updateGrid();
	~Water();
};


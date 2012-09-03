#ifndef __FLUIDS_H__
#define __FLUIDS_H__

struct glsl_program;
struct mouse_data;

#define DATA_PATH "data/"

#include "AntTweakBar.h"

class Fluids {

	friend void TW_CALL DensStepCallback(void *clientData);
	friend void TW_CALL UpdateVelStepCallback(void *clientData);
public:
	enum eDrawMode { DM_VELOCITY0, DM_VELOCITY, DM_DENSITY };
private:

	// buffers
	GLuint frambuffer_;
	GLuint velocity_;
	GLuint velocity_accum_u_;
	GLuint velocity_accum_v_;
	GLuint velocity_accum_ut_;
	GLuint velocity_accum_vt_;
	GLuint velocity_accum_u0_;
	GLuint velocity_accum_v0_;
	GLuint velocity_accum_u0t_;
	GLuint velocity_accum_v0t_;
	
	GLuint density_;
	GLuint density_accum0_;
	GLuint density_accum0t_;
	GLuint density_accum_;
	GLuint density_accum_t_;

	GLuint p_;
	GLuint p_prev_;
	
	GLuint depth_;

	// shaders
	glsl_program* calc_density_;
	glsl_program* mark_velocity_;
	glsl_program* mark_density_;
	glsl_program* do_diffuse_method1_;
	glsl_program* do_diffuse_simple_;
	glsl_program* do_advect_;
	glsl_program* do_updvel_;
	glsl_program* do_project_a_;
	glsl_program* do_project_b_;
	glsl_program* do_project_c_;
	glsl_program* do_set_bnd_;
	glsl_program* clear_rts4_;
	glsl_program* do_copy_;
	

	void diffuse(float dt, int b, GLuint& temp_x, GLuint& x, GLuint& x0, const bool method1 = false);
	void advect(float dt, int b, GLuint& d, GLuint d0, GLuint u, GLuint v);
	void project(float dt, GLuint& u, GLuint& v, GLuint& p, GLuint& div);
	void set_boundaries(int b, GLuint& tmp, GLuint& buf);

	const static int NUM_OP = 50;

	eDrawMode draw_mode_; 
	eDrawMode view_mode_; 

	bool dens_step_;
	bool vel_step_;

	bool update_;

public:
	Fluids():draw_mode_(DM_VELOCITY0), view_mode_(DM_VELOCITY0), dens_step_(false), update_(false), vel_step_(false) {}

	void createShaders();
	void createBuffers();
	void createUI();
	void drawBuffers(mouse_data* pmouse, float dt);

	//GLuint getVelocityTexture() { return velocity_; }
	void getVelocityAccumTexture0(GLuint* u, GLuint* v) { *u = velocity_accum_u0_; *v = velocity_accum_v0_; }
	void getVelocityAccumTexture(GLuint* u, GLuint* v) { *u = velocity_accum_u_; *v = velocity_accum_v_; }

	GLuint getDensityTexture() { return density_; }
	GLuint getDensityAccumTexture() { return density_accum0_; }
	GLuint getPressureAccumTexture() { return velocity_accum_u0_; }

	
	eDrawMode getDrawMode() { return draw_mode_; }
	eDrawMode getViewMode() { return view_mode_; }
};

#endif // __FLUIDS_H__
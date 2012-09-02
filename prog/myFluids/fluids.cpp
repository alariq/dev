#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <assert.h>

#include "fluids.h"
#include "renderer.h"
#include "shader_builder.h"
#include "input.h"

#include "utils/ui.h"

template <typename T>
void swap(T& a, T& b)
{
	T t = a;
	a = b;
	b = t;
}

static GLenum drawbuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

void TW_CALL DensStepCallback(void *clientData)
{
	Fluids* pfluids = static_cast<Fluids*>(clientData);
	pfluids->dens_step_ = true;
}

void TW_CALL UpdateVelStepCallback(void *clientData)
{
	Fluids* pfluids = static_cast<Fluids*>(clientData);
	pfluids->vel_step_ = true;
}

void Fluids::createShaders()
{
	mark_velocity_ = glsl_program::makeProgram("mark_velocity", DATA_PATH"quad.vs", DATA_PATH"mark_velocity.fs");
	mark_density_ = glsl_program::makeProgram("mark_density", DATA_PATH"quad.vs", DATA_PATH"mark_density.fs");
	calc_density_ = glsl_program::makeProgram("calc_density", DATA_PATH"quad.vs", DATA_PATH"calc_density.fs");

	do_diffuse_method1_ = glsl_program::makeProgram("do_diffuse_method1", DATA_PATH"quad.vs", DATA_PATH"do_diffuse_method1.fs");
	do_diffuse_simple_ = glsl_program::makeProgram("do_diffuse_simple", DATA_PATH"quad.vs", DATA_PATH"do_diffuse_simple.fs");
	do_advect_ = glsl_program::makeProgram("do_advect", DATA_PATH"quad.vs", DATA_PATH"do_advect.fs");
	do_updvel_ = glsl_program::makeProgram("do_updvel", DATA_PATH"quad.vs", DATA_PATH"do_updvel.fs");

	do_project_a_ = glsl_program::makeProgram("do_project_a", DATA_PATH"quad.vs", DATA_PATH"do_project_a.fs");
	do_project_b_ = glsl_program::makeProgram("do_project_b", DATA_PATH"quad.vs", DATA_PATH"do_project_b.fs");
	do_project_c_ = glsl_program::makeProgram("do_project_c", DATA_PATH"quad.vs", DATA_PATH"do_project_c.fs");

	do_set_bnd_ = glsl_program::makeProgram("do_set_bnd", DATA_PATH"quad.vs", DATA_PATH"do_set_boundary.fs");

	clear_rts4_ = glsl_program::makeProgram("clear_rts4", DATA_PATH"quad.vs", DATA_PATH"clear_rts4.fs");
	do_copy_ = glsl_program::makeProgram("do_copy", DATA_PATH"quad.vs", DATA_PATH"do_copy.fs");
}

void Fluids::createBuffers()
{
	int tex_fmt = GL_R32F;
	int tex_int_fmt = GL_RED;
	frambuffer_ = createFrameBuffer();

	//velocity_ = createRenderTexture(128, 128, GL_RGBA8, GL_RGBA);
	density_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	CHECK_GL_ERROR();
	velocity_accum_u_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	CHECK_GL_ERROR();
	velocity_accum_v_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	velocity_accum_ut_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	velocity_accum_vt_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);

	velocity_accum_u0_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);	// 7
	velocity_accum_v0_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT); // 8
	velocity_accum_u0t_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	velocity_accum_v0t_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);

	//velocity_accum_ = createRenderTexture(128, 128, GL_RGBA8, GL_RGBA );
	//velocity_accum2_ = createRenderTexture(128, 128, GL_RGBA8, GL_RGBA );

	density_accum0_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	density_accum0t_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	density_accum_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	density_accum_t_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);

	//tex_fmt = GL_RGBA32F;
	//tex_int_fmt = GL_RGBA;
	p_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);
	p_prev_ = createRenderTexture(128, 128, tex_fmt, tex_int_fmt, GL_FLOAT);


	printf("density_accum: 1: %d 2: %d\n", density_accum0_, density_accum_);

	depth_ = createRenderBuffer(128, 128);
}

void Fluids::createUI()
{
	UIBar* pbar = UI::CreateBar("fluids");

	TwEnumVal dm[] = { {DM_DENSITY, "Density"}, {DM_VELOCITY0, "Velocity0"}, {DM_VELOCITY, "Velocity"} };
	TwType dmType = TwDefineEnum("DrawMode", dm, 3);

	UI::CreateVar(pbar, "DrawMode", dmType, &draw_mode_, "keyincr=] keydecr=[");
	UI::CreateVar(pbar, "ViewMode", dmType, &view_mode_, "keyincr=l keydecr=;");
	UI::AddButton(pbar, "density step", DensStepCallback, this, "key=a");
	UI::AddButton(pbar, "updvel step", UpdateVelStepCallback, this, "key=v");

	UI::CreateVar(pbar, "update", TW_TYPE_BOOLCPP, &update_, "key=SPACE");
}

void Fluids::drawBuffers(mouse_data* pmouse, float dt)
{
	setFrameBuffer(frambuffer_	);
	
	//glClearColor(0, 1, 0, 1);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, 128, 128);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	bool bDraw  = pmouse->buttons[0];
	// draw velocity
	if(DM_VELOCITY0 == draw_mode_ && bDraw)
	{
		float dx = (float)(pmouse->rel_x)/8.0f;
		float dy = (float)(pmouse->rel_y)/6.0f;
		float xy[4] = { 128.0f*((float)pmouse->x/799.0f), 128.0f*(1 - (float)pmouse->y/599.0f), dx, dy };

		printf("delta mouse: %f %f\n", xy[2], xy[3]);


		setRenderTexture(velocity_accum_u0t_, 0);
		setRenderTexture(velocity_accum_v0t_, 1);
		// Enable both attachments as draw buffers
        
        glDrawBuffers(2, drawbuffers);
		checkFramebufferStatus();

		mark_velocity_->setFloat4("mouse", xy);
		//mark_velocity_->setFloat("speed_k", 1);
		mark_velocity_->apply();

		glEnable(GL_TEXTURE_2D);

		set_texture_for_sampler(mark_velocity_, "u0", 0, velocity_accum_u0_);
		set_texture_for_sampler(mark_velocity_, "v0", 1, velocity_accum_v0_);
		draw_quad(1);
		swap(velocity_accum_u0_, velocity_accum_u0t_);
		swap(velocity_accum_v0_, velocity_accum_v0t_);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glDisable(GL_TEXTURE_2D);
		glUseProgram(0);
		setRenderTexture(0, 0);
		setRenderTexture(0, 1);

	}
	else if(DM_DENSITY == draw_mode_ && bDraw)
	{
		float dx = (float)(pmouse->rel_x)/8.0f;
		float dy = (float)(pmouse->rel_y)/6.0f;
		float xy[4] = { 128.0f*((float)pmouse->x/799.0f), 128.0f*(1 - (float)pmouse->y/599.0f), dx, dy };

		setRenderTexture(density_accum0t_, 0);        
        glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();

		mark_density_->setFloat2("mouse", xy);
		mark_density_->apply();
		glEnable(GL_TEXTURE_2D);
		set_texture_for_sampler(mark_density_, "x0", 0, density_accum0_);
		draw_quad(1);
		swap(density_accum0_, density_accum0t_);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glUseProgram(0);
		setRenderTexture(0, 0);
	}

	static bool init_accum = true;
	if(init_accum)
	{
		float xy[] = {0};
		float color[4] = {0,1,0,1};

		clear_rts4_->setFloat4("color", color);
		clear_rts4_->apply();

		setRenderTexture(velocity_accum_u0_, 0);
		setRenderTexture(velocity_accum_v0_, 1);
		setRenderTexture(velocity_accum_u0t_, 2);
		setRenderTexture(velocity_accum_v0t_, 3);
		// Enable both attachments as draw buffers
        glDrawBuffers(4, drawbuffers);
		checkFramebufferStatus();
		
		glClearColor(1,1,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		draw_quad(0, 1);
		//glUseProgram(0);

		setRenderTexture(velocity_accum_u_, 0);
		setRenderTexture(velocity_accum_v_, 1);
		setRenderTexture(velocity_accum_ut_, 2);
		setRenderTexture(velocity_accum_vt_, 3);
		checkFramebufferStatus();
		glClearColor(0,0, 0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		draw_quad(0, 1);
		setRenderTexture(0,0);
		setRenderTexture(0,1);
		setRenderTexture(0,2);
		setRenderTexture(0,3);
		
		glUseProgram(0);

		setRenderTexture(density_accum0_, 0);
		glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();
		glClearColor(0,0, 0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		mark_density_->setFloat2("mouse", xy);
		mark_density_->apply();
		draw_quad(0, 1);
		setRenderTexture(0,0);

		setRenderTexture(density_, 0);
		glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();
		glClearColor(0,0, 0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		mark_density_->setFloat2("mouse", xy);
		mark_density_->apply();
		draw_quad(0, 1);
		setRenderTexture(0,0);


		setRenderTexture(density_accum_,0);
		setRenderTexture(density_accum_t_,1);
		GLenum drawbuffers2[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, drawbuffers2);
		checkFramebufferStatus();
		glClearColor(0,0,0,0);
		draw_quad(0, 1);
		glUseProgram(0);

		setRenderTexture(0,0);
		setRenderTexture(0,1);
		setRenderTexture(0,2);
		setRenderTexture(0,3);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		init_accum = false;
	}
	else if(DM_DENSITY == draw_mode_ && bDraw) // add only of we actually were drawing :-)
	{
		/*setRenderTexture(density_accum0_,0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		checkFramebufferStatus();
		glUseProgram(0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		draw_quad(density_, 1);
		glDisable(GL_BLEND);
		setRenderTexture(0,0);*/
	}


	if(/*pmouse->buttons[2]*/ dens_step_ || update_)
	{
		// result in density_accum_
		diffuse(dt, 0, density_accum_t_, density_accum_, density_accum0_);
		swap(density_accum_, density_accum0_);
		advect(dt, 0, density_accum_, density_accum0_, velocity_accum_u0_, velocity_accum_v0_);
		swap(density_accum_, density_accum0_);
		dens_step_ = false;
	}

	if(vel_step_ || update_)
	{
		// updates u v components of velocity
		//swap(velocity_accum_u_, velocity_accum_u0_);
		diffuse(dt, 1, velocity_accum_u0t_, velocity_accum_u_, velocity_accum_u0_, true);
		//swap(velocity_accum_v_, velocity_accum_v0_);
		diffuse(dt, 2, velocity_accum_v0t_, velocity_accum_v_, velocity_accum_v0_, true);
		
		project (dt, velocity_accum_u_, velocity_accum_v_, velocity_accum_u0_, velocity_accum_v0_);
		swap(velocity_accum_u_, velocity_accum_u0_);
		swap(velocity_accum_v_, velocity_accum_v0_);
		////			uv				uv0
		advect(dt, 1, velocity_accum_u_, velocity_accum_u0_, velocity_accum_u0_, velocity_accum_v0_);
		advect(dt, 2, velocity_accum_v_, velocity_accum_v0_, velocity_accum_u0_, velocity_accum_v0_);
		project (dt, velocity_accum_u_, velocity_accum_v_, velocity_accum_u0_, velocity_accum_v0_);

		swap(velocity_accum_u_, velocity_accum_u0_);
		swap(velocity_accum_v_, velocity_accum_v0_);

		vel_step_ = false;
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	

	setFrameBuffer(0);
}

// final result in x
void Fluids::diffuse(float dt, int b, GLuint& temp_x, GLuint& x, GLuint& x0, const bool method1/* = false*/)
{
	setFrameBuffer(frambuffer_);


	glsl_program* prog = method1 ? do_diffuse_method1_ : do_diffuse_simple_;

	if(method1)
	{
		setRenderTexture(x);
		glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();
		do_copy_->apply();
		set_texture_for_sampler(do_copy_, "tex", 0, x0);
		draw_quad(1);
	}
	
	prog->setFloat("dt", dt);
	prog->apply();
	glEnable(GL_TEXTURE_2D);

	

	for(int i=0; i< NUM_OP; ++i)
	{
		// 2 different ways: both are a bit incorrect though :-)
		if(method1)
			setRenderTexture(temp_x);
		else
			setRenderTexture(x);
		
		glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();
		if(method1)
		{
			// use temp texture for processing
			//set_texture_for_sampler(prog, "x", 0, x);
			//set_texture_for_sampler(prog, "x0", 1, x0);
			set_texture_for_sampler(prog, "x0", 1, x);
			draw_quad(1);
			swap(temp_x, x);
		}
		else
		{
			// change do_diffuse shader, so that only reads from x0 are done even at places where we read from x
			set_texture_for_sampler(prog, "x0", 0, x0);
			draw_quad(1);
			//glBindTexture(GL_TEXTURE_2D, 0);

			set_boundaries(b, x0, x);
			prog->setFloat("dt", dt);
			prog->apply();

			swap(x0, x);
		}
	}

	//if(method1 )
	//	swap(x0, x);

	setRenderTexture(0,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);

#undef METHOD1
}

void Fluids::advect(float dt, int b, GLuint& d, GLuint d0, GLuint u, GLuint v)
{
	setFrameBuffer(frambuffer_);

	do_advect_->setFloat("dt", dt);
	do_advect_->apply();
	
	setRenderTexture(d);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	checkFramebufferStatus();

	//assert(do_advect_->samplers_.count("density") && do_advect_->samplers_.count("velocity"));

	glEnable(GL_TEXTURE_2D);

	set_texture_for_sampler(do_advect_, "d0", 0, d0);
	set_texture_for_sampler(do_advect_, "u", 1, u);
	set_texture_for_sampler(do_advect_, "v", 2, v);
	
	draw_quad(1);

	set_boundaries(b, p_prev_, d);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_TEXTURE_2D);

	glUseProgram(0);
	setRenderTexture(0,0);
}

void Fluids::project(float dt, GLuint& u, GLuint& v, GLuint& p, GLuint& div)
{
	setFrameBuffer(frambuffer_);

	// stage 1
	do_project_a_->apply();
	
	setRenderTexture(div,0);
	setRenderTexture(p,1);
	setRenderTexture(velocity_accum_u0t_,2);
	setRenderTexture(velocity_accum_v0t_,3);
	glDrawBuffers(4, drawbuffers);
	checkFramebufferStatus();

	glEnable(GL_TEXTURE_2D);
	set_texture_for_sampler(do_project_a_, "u", 0, u);
	set_texture_for_sampler(do_project_a_, "v", 1, v);
	draw_quad(1);

	setRenderTexture(0,0);
	setRenderTexture(0,1);
	setRenderTexture(0,2);
	setRenderTexture(0,3);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	set_boundaries(0, p_prev_, div);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	setRenderTexture(0,0);


	// stage 2
	do_project_b_->apply();
	for(int i=0; i< NUM_OP; ++i)
	{
		setRenderTexture(p_prev_,0);
		glDrawBuffers(1, drawbuffers);
		checkFramebufferStatus();
		
		set_texture_for_sampler(do_project_b_, "div", 0, div);
		set_texture_for_sampler(do_project_b_, "p", 1, p);
		draw_quad(1);
		
		swap(p_prev_, p); // p has result

		set_boundaries(0, p_prev_, p);
		do_project_b_->apply();
		
	}

	setRenderTexture(0,0);

	// stage 3
	do_project_c_->apply();
	setRenderTexture(u,0);
	setRenderTexture(v,1);
	glDrawBuffers(2, drawbuffers);
	checkFramebufferStatus();

	set_texture_for_sampler(do_project_c_, "p", 0, p);
	set_texture_for_sampler(do_project_c_, "u", 1, velocity_accum_u0t_); // same as u
	set_texture_for_sampler(do_project_c_, "v", 2, velocity_accum_v0t_); // same as v
	// we need those to make A = A + B assignment
	draw_quad(1);

	setRenderTexture(0,0);
	setRenderTexture(0,1);

	set_boundaries(1, p_prev_, u);
	set_boundaries(2, p_prev_, v);

	setRenderTexture(0,0);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_TEXTURE_2D);

	glUseProgram(0);
	setRenderTexture(0,0);
	setRenderTexture(1,0);
	setRenderTexture(2,0);
	setRenderTexture(3,0);
}

void Fluids::set_boundaries(int b, GLuint& tmp, GLuint& buf)
{
	do_set_bnd_->setInt("b", b);
	do_set_bnd_->apply();

	setRenderTexture(tmp,0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	set_texture_for_sampler(do_set_bnd_, "x", 0, buf);
	draw_quad(1);
	//glUseProgram(0);
	swap(tmp, buf);
	
}

	

#include "shader_builder.h"
#include "utils/stream.h"

#include <assert.h>

std::map<std::string, glsl_shader*> glsl_shader::s_vertex_shaders;
std::map<std::string, glsl_shader*> glsl_shader::s_fragment_shaders;

std::map<std::string, glsl_program*> glsl_program::s_programs;
UNIFORM_FUNC glsl_program::uniformFuncs[15] = {0};



const int constantSizes[] = {
    sizeof(float),
    sizeof(int),
    2*sizeof(float),
    3*sizeof(float),
    4*sizeof(float),
    sizeof(int) * 2,
    sizeof(int) * 3,
    sizeof(int) * 4,
    sizeof(int),
    sizeof(int) * 2,
    sizeof(int) * 3,
    sizeof(int) * 4,
    4*sizeof(float),
    9*sizeof(float),
    16*sizeof(float),
};

void init_func_ptrs(UNIFORM_FUNC (&uniformFuncs)[15])
{
    uniformFuncs[CONSTANT_FLOAT] = (UNIFORM_FUNC) glUniform1fvARB;
    uniformFuncs[CONSTANT_VEC2]  = (UNIFORM_FUNC) glUniform2fvARB;
    uniformFuncs[CONSTANT_VEC3]  = (UNIFORM_FUNC) glUniform3fvARB;
    uniformFuncs[CONSTANT_VEC4]  = (UNIFORM_FUNC) glUniform4fvARB;
    uniformFuncs[CONSTANT_INT]   = (UNIFORM_FUNC) glUniform1ivARB;
    uniformFuncs[CONSTANT_IVEC2] = (UNIFORM_FUNC) glUniform2ivARB;
    uniformFuncs[CONSTANT_IVEC3] = (UNIFORM_FUNC) glUniform3ivARB;
    uniformFuncs[CONSTANT_IVEC4] = (UNIFORM_FUNC) glUniform4ivARB;
    uniformFuncs[CONSTANT_BOOL]  = (UNIFORM_FUNC) glUniform1ivARB;
    uniformFuncs[CONSTANT_BVEC2] = (UNIFORM_FUNC) glUniform2ivARB;
    uniformFuncs[CONSTANT_BVEC3] = (UNIFORM_FUNC) glUniform3ivARB;
    uniformFuncs[CONSTANT_BVEC4] = (UNIFORM_FUNC) glUniform4ivARB;
    uniformFuncs[CONSTANT_MAT2]  = (UNIFORM_FUNC) glUniformMatrix2fvARB;
    uniformFuncs[CONSTANT_MAT3]  = (UNIFORM_FUNC) glUniformMatrix3fvARB;
    uniformFuncs[CONSTANT_MAT4]  = (UNIFORM_FUNC) glUniformMatrix4fvARB;
}

// true - error, false - no error
bool get_shader_error_status(GLuint shader, GLenum status_type)
{
    int status;
    glGetShaderiv(shader, status_type, &status);

    if(!status)
    {
        char* buf = 0;
        GLsizei len = 0, len2= 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        buf = new char[len];

        glGetShaderInfoLog(shader, len, &len2, buf);
        if(len2!=0)
            printf("CompileShader: %s\n", buf);

        return true;
    }

    return false;
}

bool get_program_error_status(GLuint program, GLenum status_type)
{
    int status;
    glGetProgramiv(program, status_type, &status);

    if(!status)
    {
        char* buf = 0;
        GLsizei len = 0, len2= 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        buf = new char[len];

        glGetProgramInfoLog(program, len, &len2, buf);
        if(len2!=0)
            printf("CompileShader: %s\n", buf);

        return true;
    }

    return false;
}

const char* glsl_load(const char* fname)
{
    assert(fname);
    stream* pstream = stream::makeFileStream();
    if(0 != pstream->open(fname,"rb"))
    {
        printf("Can't open %s \n", fname);
        return 0;
    }

    pstream->seek(0, stream::S_END);
    size_t size = pstream->tell();
    pstream->seek(0, stream::S_SET);

    char* pdata = new char[size + 1];
    size_t rv = pstream->read(pdata, 1, size);
    assert(rv==size);
    pdata[size] = '\0';
    return pdata;
}


glsl_shader* glsl_shader::makeShader(Shader_t stype, const char* fname)
{ 
    const char* psource = glsl_load(fname);
    if(!psource)
        return 0;

    GLenum type = stype == glsl_shader::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    GLuint shader = glCreateShader(type);
    if(0 == shader)
        return 0;

    const char* strings[] = { psource };

    glShaderSource(shader, 1, strings, 0);
    delete strings[0];

    glCompileShader(shader);

    if(get_shader_error_status(shader, GL_COMPILE_STATUS))
    {
        glDeleteShader(shader);
        return 0;
    }

    glsl_shader* pshader = new glsl_shader();
    pshader->fname_ = fname;
    pshader->shader_ = shader;
    pshader->type_ = type;

    if(stype == glsl_shader::VERTEX)
    {
		// this is some crazy code, we should not delete objects which are used in shaders previously loaded
		// though we'll have memeory leaks here, because new pshader will be inserted in place of old (if it has same neme)
		// but at least we will not delete old one. Have to rethink what is going on here
		/*
        if(s_vertex_shaders.count(fname))
		{
			glsl_shader* pshader = s_vertex_shaders[fname];
			s_vertex_shaders.erase(fname);
            delete pshader;
		}
		*/
        s_vertex_shaders.insert( std::make_pair(pshader->fname_, pshader) );
    }
    else
    {
		// this is some crazy code, we should not delete objects which are used in shaders previously loaded
		// though we'll have memeory leaks here, because new pshader will be inserted in place of old (if it has same neme)
		// but at least we will not delete old one. Have to rethink what is going on here
        /*if(s_fragment_shaders.count(fname))
		{
			glsl_shader* pshader = s_fragment_shaders[fname];
			s_fragment_shaders.erase(fname);
            delete pshader;
		}*/
        s_fragment_shaders.insert( std::make_pair(pshader->fname_, pshader) );
    }
  
    return pshader;
}

void glsl_shader::deleteShader(glsl_shader* psh)
{
    if(psh->type_ == GL_VERTEX_SHADER)
    {
        if(s_vertex_shaders.count(psh->fname_))
        {   
            delete s_vertex_shaders[psh->fname_];
            s_vertex_shaders.erase(psh->fname_);
        }
    }
    else
    {
        if(s_fragment_shaders.count(psh->fname_))
        {
            delete s_fragment_shaders[psh->fname_];
            s_fragment_shaders.erase(psh->fname_);
        }
    }
}

glsl_shader::~glsl_shader()
{
    glDeleteShader(shader_);
}

bool glsl_shader::reload()
{
    const char* psource = glsl_load(fname_.c_str());
    if(!psource)
        return false;

    const char* strings[] = { psource };

    glShaderSource(shader_, 1, strings, 0);
    delete strings[0];

    glCompileShader(shader_);

    if(get_shader_error_status(shader_, GL_COMPILE_STATUS))
    {
        //glDeleteShader(shader);
        return false;
    }

    return true;
}

void parse_uniforms(GLuint pprogram, glsl_program::UniArr_t* puniforms, glsl_program::SamplerArr_t* psamplers)
{
    GLint num_uni, max_name_len;
    glGetProgramiv(pprogram, GL_ACTIVE_UNIFORMS, &num_uni);
    glGetProgramiv(pprogram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
    char* buf = new char[max_name_len+1];
    GLsizei len;
    GLint size;
    GLenum type;
    for(GLint i=0;i<num_uni;++i)
    {
        glGetActiveUniform(pprogram, i, max_name_len+1, &len, &size, &type, buf);
        if(-1 == i) continue; // gl_ variable or does not correspond to an active uniform variable name in program

		if(type >=GL_SAMPLER_1D && type<= GL_SAMPLER_2D_SHADOW)
		{
			glsl_sampler* psampler = new glsl_sampler;
			psampler->index_ = i;
			psampler->name_ = buf;
			psampler->type_ = (SamplerType)(type - GL_SAMPLER_1D);

			assert(psampler->type_ <= SAMPLER_2D_SHADOW);

			static const char *typeNames[] = {
				"sampler_1d", "sampler_2d", "sampler_3d", "sampler_cube", "sampler_1d_shadow", "sampler_2d_shadow"
			};

			printf("name: %s type: %s\n", buf, typeNames[psampler->type_]);
	
			psamplers->insert(std::make_pair(psampler->name_, psampler));

			continue;
		}

        glsl_uniform* puni = new glsl_uniform(); 
        puni->name_ = buf;
        puni->is_dirty_ = true;
        puni->index_ = glGetUniformLocation(pprogram, buf);

        switch(type)
        {
            case GL_FLOAT:
                puni->type_ = CONSTANT_FLOAT;
                puni->num_el_ = 1;
                break;
            case GL_INT:
                puni->type_ = CONSTANT_INT;
                puni->num_el_ = 1;
                break;
            default:
                puni->type_ = (ConstantType)(CONSTANT_VEC2 + (type - GL_FLOAT_VEC2));
                puni->num_el_ = size;
                break;
        }


        size_t datasize = constantSizes[ puni->type_ ] * puni->num_el_;
        puni->data_ = new unsigned char[ datasize ];
        memset(puni->data_, 0, datasize);

        static const char *typeNames[] = {
            "float", "int  ", "vec2 ", "vec3 ", "vec4 ", "ivec2", "ivec3", "ivec4",
            "bool ", "bvec2", "bvec3", "bvec4", "mat2 ", "mat3 ", "mat4 "
        };
        printf("name: %s type: %s  num_el: %d\n", buf, typeNames[puni->type_], puni->num_el_);

        puniforms->insert( std::make_pair(puni->name_, puni) );
    }

    delete[] buf;
}

glsl_program* glsl_program::makeProgram(const char* name, const char* vp, const char* fp)
{
    if(!uniformFuncs[0])
        init_func_ptrs(uniformFuncs);

    assert(name);

    if(s_programs.count(name))
    {
        printf("Program with this name (%s) already exists\n", name);
        return 0;
    }

    glsl_shader* vsh = glsl_shader::makeShader(glsl_shader::VERTEX, vp);
    glsl_shader* fsh = glsl_shader::makeShader(glsl_shader::FRAGMENT, fp);
    if(!vsh || !fsh)
        return 0;

    GLuint shp = glCreateProgram();

    glAttachShader(shp, vsh->shader_);
    if( GL_INVALID_OPERATION == glGetError())
    {
        glDeleteProgram(shp);
        printf("glAttachShader: shader %s was already attached\n", vsh->fname_.c_str());
        return 0;
    }

    glAttachShader(shp, fsh->shader_);
    if( GL_INVALID_OPERATION == glGetError())
    {
        glDeleteProgram(shp);
        printf("glAttachShader: shader %s was already attached\n", vsh->fname_.c_str());
        return 0;
    }

    glLinkProgram(shp);

    if(get_program_error_status(shp, GL_LINK_STATUS))
    {
        glDeleteProgram(shp);
        return 0;
    }

    glsl_program* pprogram = new glsl_program();
    pprogram->shp_ = shp;
    pprogram->vsh_ = vsh;
    pprogram->fsh_ = fsh;
    pprogram->is_valid_ = true;

	glDetachShader(shp, vsh->shader_);
	glDetachShader(shp, fsh->shader_);

    parse_uniforms(shp, &pprogram->uniforms_, &pprogram->samplers_);

    s_programs.insert(std::make_pair(name, pprogram) );
    return pprogram;
}


void glsl_program::deleteProgram(const char* name)
{
    if(s_programs.count(name))
    {
        glsl_program* pprogram = s_programs[name];
        s_programs.erase(name);
        delete pprogram;
    }
}

glsl_program::~glsl_program()
{
    if(shp_)
    {
        glDetachShader(shp_, vsh_->shader_);
        glDetachShader(shp_, fsh_->shader_);
        glDeleteProgram(shp_);
    }
}

void glsl_program::apply()
{
    glUseProgram(shp_);

    UniArr_t::iterator it = uniforms_.begin(); 
    UniArr_t::iterator end = uniforms_.end(); 
    for(;it!=end;++it)
    {
        glsl_uniform* puni = it->second;
        if(puni->is_dirty_)
        {
            if (puni->type_ >= CONSTANT_MAT2){
                ((UNIFORM_MAT_FUNC) uniformFuncs[puni->type_])(puni->index_, puni->num_el_, GL_TRUE, (float *) puni->data_);
            } else {
                uniformFuncs[puni->type_](puni->index_, puni->num_el_, (float *) puni->data_);
                if(GL_INVALID_OPERATION == glGetError())
                    printf("Error setting variable\n");
            }
            puni->is_dirty_ = false;
        }
    }
}

bool glsl_program::reload()
{
    is_valid_ = false;

    bool rv = vsh_->reload();
    rv &= fsh_->reload();

    if(!rv) return false;

    glLinkProgram(shp_);
    if(get_program_error_status(shp_, GL_LINK_STATUS))
    {
        return false;
    }

    std::map< std::string, glsl_uniform*>::iterator it = uniforms_.begin(); 
    std::map< std::string, glsl_uniform*>::iterator end = uniforms_.end(); 
    for(;it!=end;++it)
        delete it->second;
    uniforms_.clear();

    parse_uniforms(shp_, &uniforms_, &samplers_);
    
    is_valid_ = true;
    return true;

}

bool glsl_program::is_valid()
{
    return is_valid_;
}

//=====================================================================================================================================
bool glsl_program::setFloat(const char* name, float v)
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_FLOAT)
    {
        memcpy(it->second->data_, &v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setFloat2(const char* name, float v[2])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_VEC2)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setFloat3(const char* name, float v[3])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_VEC3)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setFloat4(const char* name, float v[4])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_VEC4)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}

bool glsl_program::setInt(const char* name, int v)
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_INT)
    {
        memcpy(it->second->data_, &v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setInt2(const char* name, int v[2])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_IVEC2)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setInt3(const char* name, int v[3])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_IVEC3)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setInt4(const char* name, int v[4])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_IVEC4)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}

bool glsl_program::setMat2(const char* name, float v[4])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_MAT2)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setMat3(const char* name, float v[9])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_MAT3)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
bool glsl_program::setMat4(const char* name, float v[16])
{
    UniArr_t::iterator it = uniforms_.find(name);
    if(it!=uniforms_.end() && it->second->type_ == CONSTANT_MAT4)
    {
        memcpy(it->second->data_, v, constantSizes[it->second->type_] * it->second->num_el_);
        it->second->is_dirty_ = true;
        return true;
    }
	printf("Type mismatch: %s\n", name);
    return false;
}
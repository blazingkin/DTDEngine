#include "SystemApplyPostProcessing.h"

GLuint fbo;
GLuint fbotex;
std::shared_ptr<Program> ppprog;
GLuint squarevao;
GLuint squarepos;
GLuint squareele;

void initializePostprocessing() {
    // Initialize all of the framebuffers and associated textures

    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &fbo);

    //generate the texture
    glGenTextures(1, &fbotex);
    glBindTexture(GL_TEXTURE_2D, fbotex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 
        0,  GL_RGB, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbotex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const std::vector<std::string> depthShaders({"pass_vert.glsl", "horizontal_shift_frag.glsl"});
    ppprog = std::make_shared<Program>();
    ppprog->setVerbose(true);
    ppprog->init(depthShaders);
    ppprog->addAttribute("vertPos");
    ppprog->addUniform("Texture0");
    ppprog->addUniform("P");

    // Create the square on which to render
    static const GLfloat g_quad_vertex_buffer_data[] =
		{
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,
		};
    static std::vector<float> squareEle = {0, 1, 2,
                                    0, 2, 3};
    CHECKED_GL_CALL(glGenVertexArrays(1, &squarevao));
    CHECKED_GL_CALL(glBindVertexArray(squarevao));

    CHECKED_GL_CALL(glGenBuffers(1, &squarepos));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, squarepos));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), &g_quad_vertex_buffer_data[0], GL_STATIC_DRAW));

    CHECKED_GL_CALL(glGenBuffers(1, &squareele));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareele));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, squareEle.size()*sizeof(unsigned int), &squareEle[0], GL_STATIC_DRAW));

	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void postprocessingOnResize(int width, int height) {
    glBindTexture(GL_TEXTURE_2D, fbotex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 
        0,  GL_RGB, GL_FLOAT, NULL);
}


void applyPostprocessing(BScene *scene, GLuint tex) {
    ppprog->bind();
    // Bind the output to Texture0
    CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
    CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, tex));
    CHECKED_GL_CALL(glUniform1i(ppprog->getUniform("Texture0"), 0));
    CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    CHECKED_GL_CALL(glBindVertexArray(squarevao));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, squarepos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    ppprog->unbind();
}


void cleanupPostprocessing() {

}
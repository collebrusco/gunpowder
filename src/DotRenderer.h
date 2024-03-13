#ifndef DOT_RENDERER_H
#define DOT_RENDERER_H

#include <flgl.h>
#include <flgl/glm.h>
#include <flgl/tools.h>
#include "../lib/sw/Stopwatch.h"
#include "Dotfield.h"

struct DotRenderer {
	Mesh<Vt_classic> quad;
	Shader df_shad;
	Texture df_tex;
	Stopwatch timer;

	DotRenderer() : timer(MILLISECONDS) {}
	void init() {
		quad = DefaultMeshes::tile<Vt_classic>();
		df_shad = Shader::from_source("passthrough_vert", "tex");
		df_tex.create();
		df_tex.bind();
		df_tex.pixelate();
    	df_tex.unbind();
	}
	void buffer_texture(Dotfield const& field) {
		timer.reset_start();
		df_tex.bind();
    	df_tex.alloc(GL_RGB32F, field.x(), field.y(), GL_RGB, GL_UNSIGNED_BYTE, field.pixels());
		double stop = timer.stop_reset();
		df_tex.unbind();
		// LOG_DBG("updating took %3f ms", stop);
	}
	void render() {
		gl.clear();
		df_tex.bind();

		df_shad.bind();
		timer.reset_start();
		gl.draw_mesh(quad);
		double stop = timer.stop_reset();
		df_tex.unbind();
		df_shad.unbind();
		// LOG_DBG("drawing took %3f ms", stop);
	}
};

#endif

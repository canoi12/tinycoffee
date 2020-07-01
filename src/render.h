#ifndef TICO_RENDER_H
#define TICO_RENDER_H

#include "tico.h"

typedef struct tc_Render {
	tc_Batch batch;
	struct {
		tc_Shader defaultShader;
    tc_Shader currentShader;

    int defaultVertexShader;
    int defaultFragmentShader;

    unsigned int defaultTextureId;
    unsigned int currentTextureId;
    tc_Texture shapeTexture;

    unsigned int drawCount;
    tc_Canvas currentCanvas;

    tc_Matrix modelview;
    tc_Canvas defaultCanvas;

    tc_Matrix *camera;

    tc_Matrix matrixStack[MATRIX_STACK_SIZE];
    int currentMatrix;

    void (*backToDefaultCanvas)(void);
	} state;
} tc_Render;

TIC_API tc_bool tic_render_init(tc_Render *render);
TIC_API void tic_render_destroy(tc_Render *render);

#endif
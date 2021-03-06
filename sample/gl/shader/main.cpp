﻿#define SP_USE_GLEW 1

#include "simplesp.h"
#include "spex/spgl.h"

using namespace sp;

class RenderGUI : public BaseWindow {

    // camera
    CamParam m_cam;

    // image
    Mem2<Col3> m_img;

    // object mesh model
    Mem1<Mesh3> m_model;

    // object to cam pose
    Pose m_pose;


private:

    void help() {
        printf("'ESC' key : exit\n");
        printf("\n");
    }

    virtual void init() {

        help();

        m_cam = getCamParam(640, 480);

        m_img.resize(m_cam.dsize);
        m_img.zero();

        if (loadBunny(SP_DATA_DIR "/stanford/bun_zipper.ply", m_model) == false) {

            // if could not find stanford bunny, load dummy model
            loadGeodesicDorm(m_model, 100.0, 1);
        }

        m_pose = getPose(getVec(0.0, 0.0, getModelDistance(m_model, m_cam)));

    }
    
    virtual void keyFun(int key, int scancode, int action, int mods) {

    }

    void edge() {
        static Shader shader;
        static FrameBufferObject fbo;

        if (shader.valid() == false) {

            const char* vert =
                #include "spex/spshader/edge.vert"
                ;

            const char* frag =
                #include "spex/spshader/edge.frag"
                ;

            shader.load(vert, frag);
        }

        {
            fbo.resize(m_wcam.dsize);
            fbo.bind();
            glLoadView3D(m_wcam, m_viewPos, m_viewScale);

            glLoadMatrix(m_pose);
            glRenderSurface(m_model);

            fbo.unbind();
        }

        {
            shader.enable();
            shader.setUniform("nearPlane", 1.0);
            shader.setUniform("farPlane", 10000.0);
            shader.setUniform("dx", 1.0 / m_wcam.dsize[0]);
            shader.setUniform("dy", 1.0 / m_wcam.dsize[1]);

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBindTexture(GL_TEXTURE_2D, fbo.m_tex[1]);

            glBegin(GL_QUADS);
            glVertex2d(-1.0, -1.0);
            glVertex2d(+1.0, -1.0);
            glVertex2d(+1.0, +1.0);
            glVertex2d(-1.0, +1.0);
            glEnd();
            
            glBindTexture(GL_TEXTURE_2D, 0);

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            shader.disable();
        }
    }

    void simple() {
        const char *vert =
            "#version 400 core\n"
            "layout(location = 0) in vec3 vtx;"
            "out vec3 fcol;"
            "uniform mat4 mat;"

            "void main(){"
            "gl_Position = mat * vec4(vtx, 1.0);"
            "fcol = vec3(0.5, 1.0, 1.0);"
            "}"
            ;

        const char *flag =
            "#version 400 core\n"
            "in vec3 fcol;"
            "out vec3 color;"
            "void main()"
            "{"
            "color = fcol;"
            "}"
            ;

        static Shader shader;
        if (shader.valid() == false) {
            shader.load(vert, flag);
        }

        glLoadView3D(m_cam, m_viewPos, m_viewScale);
        glLoadMatrix(m_pose);

        shader.enable();
        shader.setUniform("mat", glGetMat(GL_PROJECTION_MATRIX) * glGetMat(GL_MODELVIEW_MATRIX));
        shader.setVertex(0, (Vec3*)m_model.ptr, m_model.size() * 3);

        glDrawArrays(GL_TRIANGLES, 0, m_model.size() * 3);

        shader.disable();
    }

    virtual void display() {

        edge();

    }

    virtual void mousePos(double x, double y) {
        controlPose(m_pose, m_mouse, m_wcam, m_viewScale);
    }

    virtual void mouseScroll(double x, double y) {
        controlPose(m_pose, m_mouse, m_wcam, m_viewScale);
    }

};


int main() {

    RenderGUI win;
    win.execute("glew", 800, 600);

    return 0;
}
﻿#include "simplesp.h"
#include "spex/spgl.h"

using namespace sp;

class SfMGUI : public BaseWindow {

    SfM m_sfm;

    // cam pose (object to cam pose)
    Pose m_pose;

    // axis pose (cam to axis pose)
    Pose m_axis;

private:

    void help() {
        printf("'a' key : update\n");
        printf("'s' key : reset\n");
        printf("'ESC' key : exit\n");
        printf("\n");
    }

    virtual void init() {
        help();

        const double distance = 5.0;
        m_pose = getPose(getVec(0.0, 0.0, +distance));
        m_axis = getPose(getVec(0.0, 0.0, -distance));

        reset();
    }

    void reset(){
        m_sfm.clear();

        CamParam cam;
        loadText(SP_DATA_DIR "/image/shiba.txt", cam);

        for (int i = 0; i < 7; i++) {
            char path[512];
            sprintf(path, SP_DATA_DIR "/image/shiba%02d.bmp", i);

            Mem2<Col3> img;
            loadBMP(path, img);

            m_sfm.addView(img, cam);
        }
        m_sfm.update();
    }

    virtual void keyFun(int key, int scancode, int action, int mods) {

        if (m_keyAction[GLFW_KEY_A] >= 1) {
            m_sfm.update();
        }

        if (m_keyAction[GLFW_KEY_S] == 1) {
            reset();
        }

    }

    virtual void display() {

        // view 3D
        glLoadView3D(m_wcam, m_viewPos, m_viewScale);

        // render points
        if (m_sfm.getPnts() != NULL) {
            const Mem1<SfM::PointData> &pnts = *m_sfm.getPnts();

            glPointSize(4.f);

            glLoadMatrix(m_pose);

            glBegin(GL_POINTS);
            for (int i = 0; i < pnts.size(); i++) {
                glColor(pnts[i].col);
                glVertex(pnts[i].pos);
            }
            glEnd();
        }

        // render cam
        if (m_sfm.getViews() != NULL) {
            const Mem1<SfM::ViewData> &views = *m_sfm.getViews();

            glLineWidth(2.f);

            glColor3d(0.5, 0.5, 0.8);

            for (int i = 0; i < views.size(); i++) {
                if (views[i].valid == false) continue;
                glLoadMatrix(m_pose * invPose(views[i].pose));

                glBegin(GL_LINES);
                glCam(views[i].cam, 0.2);
                glEnd();
            }
        }

        // render axis
        {
            glLoadMatrix(invPose(m_axis) * m_pose.rot);

            glLineWidth(2.f);
            glBegin(GL_LINES);
            glAxis(1.0);
            glEnd();
        }
    }

    virtual void mousePos(double x, double y) {
        Pose delta = zeroPose();
        controlPose(delta, m_mouse, m_wcam, m_viewScale, m_axis);
        m_pose = delta * m_pose;
    }

    virtual void mouseScroll(double x, double y) {
        Pose delta = zeroPose();
        controlPose(delta, m_mouse, m_wcam, m_viewScale, m_axis);
        m_pose.trn.z += delta.trn.z;
        m_axis.trn.z -= delta.trn.z;
    }

};


int main(){

    SfMGUI win;
    win.execute("sfm", 800, 600);

    return 0;
}
/*
 * MyDrawArea.cxx
 * 
 * Copyright 2017 Carlos Eduardo Sequeiros Borja <casebor@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <map>

#include "MolecularModel.h"
#include "functions.h"
#include "MyDrawArea.h"

#define PI 3.14159265358979323846
// Light parameters
GLfloat MyDrawArea::self_light_amb[] = {0.7,0.7,0.7,1.0};
GLfloat MyDrawArea::self_light_dif[] = {0.8,0.8,0.8,1.0};
GLfloat MyDrawArea::self_light_spe[] = {1.0,1.0,1.0,1.0};
GLfloat MyDrawArea::self_light_shi = 100.0;
// Fog parameters
GLfloat MyDrawArea::self_fog_start = 8.5;
GLfloat MyDrawArea::self_fog_end = 10.0;
GLfloat MyDrawArea::self_fog_color[] = {0,0,0};
GLfloat MyDrawArea::self_fog_density = 1.0;
// Lights positions
GLfloat MyDrawArea::self_light_0_position[] = { 1.0,  1.0, 1.0, 0.0};
GLfloat MyDrawArea::self_light_1_position[] = { 1.0, -1.0, 1.0, 0.0};
GLfloat MyDrawArea::self_light_2_position[] = {-1.0, -1.0, 1.0, 0.0};
// Background color
GLfloat MyDrawArea::self_bckgrnd_color[] = {0.0, 0.0, 0.0, 0.0};
//Camera settings
GLdouble MyDrawArea::self_fovy = 30.0;
GLdouble MyDrawArea::self_z_near = 1.0;
GLdouble MyDrawArea::self_z_far = 10.0;
GLdouble MyDrawArea::self_pitch = 0;
GLdouble MyDrawArea::self_yaw = -90;
GLdouble MyDrawArea::self_roll = 0;
double MyDrawArea::self_dist_cam_zpr = 10.0;
double MyDrawArea::self_cam_pos[3] = {0, 0, 10};
double MyDrawArea::self_pick_rad[2] = {10.0, 10.0};
// Mouse settings
bool MyDrawArea::self_mouse_rotate = false;
bool MyDrawArea::self_mouse_zoom = false;
bool MyDrawArea::self_mouse_pan = false;
bool MyDrawArea::self_dragging = false;
double MyDrawArea::self_scroll = 1.0;
double MyDrawArea::self_mouse_x = 0.0;
double MyDrawArea::self_mouse_y = 0.0;
double MyDrawArea::self_drag_pos_x = 0.0;
double MyDrawArea::self_drag_pos_y = 0.0;
double MyDrawArea::self_drag_pos_z = 0.0;
double MyDrawArea::self_pos_mouse[2] = {0.0, 0.0};
// Viewport settings
double MyDrawArea::self_left = -10.0;
double MyDrawArea::self_right = 10.0;
double MyDrawArea::self_top = 1.0;
double MyDrawArea::self_bottom = -1.0;
double MyDrawArea::self_zrp[3] = {0.0, 0.0, 0.0};

Frame MyDrawArea::self_data_frames;
// Draw lists
std::vector<GLuint> MyDrawArea::self_gl_point_list;
// Draw flags
bool MyDrawArea::self_dots = false;

MyDrawArea::MyDrawArea(){
    
    // Creates the class atributes for OpenGL
    self_drawing_area = gtk_drawing_area_new ();
    self_glconfig = gdk_gl_config_new_by_mode (static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE));
    
    // Add OpenGL capability to the drawing area
    gtk_widget_set_gl_capability (self_drawing_area, self_glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
    
    // Add the signals that will allow the draw area configure itself, 
    // catch events and so on
    g_signal_connect_after(G_OBJECT(self_drawing_area), "realize", G_CALLBACK(this->realize), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "configure_event", G_CALLBACK(this->reshape_wind), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "expose_event", G_CALLBACK(this->my_draw), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "button_press_event", G_CALLBACK(this->mouse_pressed), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "button_release_event", G_CALLBACK(this->mouse_released), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "motion_notify_event", G_CALLBACK(this->mouse_motion), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "scroll_event", G_CALLBACK(this->mouse_scroll), NULL);
    g_signal_connect(G_OBJECT(self_drawing_area), "key_press_event", G_CALLBACK(this->key_press), NULL);
    
    gtk_widget_set_events(self_drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
            GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
    gtk_widget_set_can_focus(self_drawing_area, TRUE);
    gtk_widget_grab_focus(self_drawing_area);
}

void MyDrawArea::realize(GtkWidget *widget, gpointer data){
    /*
     * Inside the realize function, you should put all you OpenGL
     * initialization code, e.g. set the projection matrix,
     * the modelview matrix, position of the camera.
     */
    GdkGLContext *gl_context = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable (widget);
    
    // All OpenGL initialization code goes here
    glMaterialfv(GL_FRONT,GL_AMBIENT, self_light_amb);
    glMaterialfv(GL_FRONT,GL_DIFFUSE, self_light_dif);
    glMaterialfv(GL_FRONT,GL_SPECULAR, self_light_spe);
    glMaterialf(GL_FRONT,GL_SHININESS, self_light_shi);
    glDepthFunc(GL_LESS);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    // FOG
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, self_fog_start);
    glFogf(GL_FOG_END, self_fog_end);
    glFogfv(GL_FOG_COLOR, self_fog_color);
    glFogf(GL_FOG_DENSITY, self_fog_density);
    glEnable(GL_POINT_SMOOTH);
    glClearColor(self_bckgrnd_color[0], self_bckgrnd_color[1], self_bckgrnd_color[2], self_bckgrnd_color[3]);
    glClearColor(0,0,0,0);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, self_light_0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, self_light_1_position);
    glLightfv(GL_LIGHT2, GL_POSITION, self_light_2_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    // Antialiased lines
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    // Initialize view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(self_fovy, (float)widget->allocation.width/widget->allocation.height, self_z_near, self_z_far);
    //self_proj_mat = setFrustum(self_fovy, widget->allocation.width/widget->allocation.height, self_z_near, self_z_far);
    //glLoadMatrixf(self_proj_mat.get());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(self_cam_pos[0], self_cam_pos[1], self_cam_pos[2], self_zrp[0], self_zrp[1], self_zrp[2], 0, 1, 0);
    //self_model_view_mat.translate(-self_cam_pos);
    //self_model_view_mat.lookAt(self_zrp);
    //glLoadMatrixf(self_model_view_mat.get());
    gdk_gl_drawable_gl_end (gl_drawable);
}


gboolean MyDrawArea::reshape_wind(GtkWidget *widget, GdkEventConfigure *event, gpointer data){
    /*
     * All modifications to the ratio of visualization have to be done in this
     * function. 
     */
    GdkGLContext *gl_context = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable (widget);
    
    if (!gdk_gl_drawable_gl_begin (gl_drawable, gl_context))
        return false;
    glViewport (0, 0, widget->allocation.width, widget->allocation.height);
    // ¡¡¡¡¡¡VERY IMPORTANT!!!!!!
    // YOU MUST SET THESE PARAMETERS (right and left) OTHERWISE THE PAN
    // FUNCTION OF THE MOUSE WILL SUFFER SEVERE BUGS :'(
    self_right = float(widget->allocation.width)/float(widget->allocation.height);
    self_left = -self_right;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(self_fovy, (float)widget->allocation.width/widget->allocation.height, self_z_near, self_z_far);
    glMatrixMode(GL_MODELVIEW);
    
    gdk_gl_drawable_gl_end (gl_drawable);
    gtk_widget_queue_draw(widget);
    return true;
}

gboolean MyDrawArea::my_draw(GtkWidget *widget, GdkEventExpose *event, gpointer data){
    /*
     * Function Description
     */
    GdkGLContext *gl_context = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable (widget);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(self_bckgrnd_color[0], self_bckgrnd_color[1], self_bckgrnd_color[2], self_bckgrnd_color[3]);
    glMatrixMode(GL_MODELVIEW);
    
    draw();
    
    if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context))
        return false;
    
    if (gdk_gl_drawable_is_double_buffered (gl_drawable))
        gdk_gl_drawable_swap_buffers(gl_drawable);
    else
        glFlush ();
    
    gdk_gl_drawable_gl_end (gl_drawable);
    return true;
}

void MyDrawArea::draw(){
    /*
     * The actual drawing goes here.
     */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(self_bckgrnd_color[0],self_bckgrnd_color[1],self_bckgrnd_color[2],self_bckgrnd_color[3]);
    glMatrixMode(GL_MODELVIEW);
    
    if (self_dots)
        glCallList(self_gl_point_list[0]);
    
    glFlush();
    //float cube0[] = { 1, 0, 1};
    //float cube1[] = {-1, 0, 1};
    //float cube2[] = {-1, 1, 1};
    //float cube3[] = { 1, 1, 1};
    //glBegin(GL_TRIANGLES);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube1[0], cube1[1], cube1[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube2[0], cube2[1], cube2[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube3[0], cube3[1], cube3[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    //glEnd();
    
}

void MyDrawArea::draw_to_pick(){
    /*
     * The actual drawing goes here.
     */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(self_bckgrnd_color[0],self_bckgrnd_color[1],self_bckgrnd_color[2],self_bckgrnd_color[3]);
    glMatrixMode(GL_MODELVIEW);
    
    if (self_dots)
        glCallList(self_gl_point_list[0]);
    
    glFlush();
    //float cube0[] = { 1, 0, 1};
    //float cube1[] = {-1, 0, 1};
    //float cube2[] = {-1, 1, 1};
    //float cube3[] = { 1, 1, 1};
    //glBegin(GL_TRIANGLES);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube1[0], cube1[1], cube1[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube2[0], cube2[1], cube2[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube3[0], cube3[1], cube3[2]);
    //glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    //glEnd();
    
}

gboolean MyDrawArea::mouse_pressed(GtkWidget *widget, GdkEventButton *event, gpointer data){
    /*
     * The actual drawing goes here.
     */
    bool m_left = false, m_middle = false, m_right = false;
    double m_x, m_y;
    if (event->button == 1)
        m_left = true;
    if (event->button == 2)
        m_middle = true;
    if (event->button == 3)
        m_right = true;
    self_mouse_rotate = m_left && !(m_middle || m_right);
    self_mouse_zoom = m_right && !(m_middle || m_left);
    self_mouse_pan = m_middle && !(m_left || m_right);
    m_x = (double)event->x;
    m_y = (double)event->y;
    self_mouse_x = event->x;
    self_mouse_y = event->y;
    //pos(m_x, m_y, self_drag_pos_x, self_drag_pos_y, self_drag_pos_z);
    if ((event->button == 1) && (event->type == GDK_2BUTTON_PRESS)){
        //std::vector<GLint> nearest
        pick(m_x, widget->allocation.height -1- m_y, self_pick_rad[0], self_pick_rad[1]);
        
        //nearest, hits = self.pick(x, self.get_allocation().height-1-y, self.pick_radius[0], self.pick_radius[1], event)
        //selected = self.select(event, nearest, hits)
    }
    if ((event->button == 2) && (event->type == GDK_BUTTON_PRESS)){
        pos(m_x, m_y, self_drag_pos_x, self_drag_pos_y, self_drag_pos_z);
        //self.dist_cam_zpr = op.get_euclidean(self.zero_reference_point, self.get_cam_pos());
    }
    if ((event->button == 3) && (event->type == GDK_BUTTON_PRESS)){
        
    }
    if ((event->button == 1) && (event->type == GDK_BUTTON_PRESS)){
        self_pos_mouse[0] = m_x;
        self_pos_mouse[1] = m_y;
    }
    return true;
}

gboolean MyDrawArea::mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data){
    /*
     * 
     */
    gint x, y;
    if (event->is_hint)
        gdk_window_get_pointer(event->window, &x, &y, NULL);
    else{
        x = event->x;
        y = event->y;
    }
    double dx = x - self_mouse_x;
    double dy = y - self_mouse_y;
    if (dx==0 && dy==0)
        return true;
    self_mouse_x = double(x);
    self_mouse_y = double(y);
    bool changed = false;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (self_mouse_zoom){
        glMatrixMode(GL_MODELVIEW);
        GLdouble modelview[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        // Delta is a modifier for the zoom effect, otherwise the zoom movement
        // would be too abrupt.
        double delta = ((self_z_far-self_z_near)/2.0 + self_z_near)/200.0;
        glLoadIdentity();
        // We only need to move along the Z axis, that is why only use the 
        // glTranslatef function with a bz value
        double bz = dy*delta;
        glTranslatef(-self_zrp[0], -self_zrp[1], -self_zrp[2]);
        glTranslatef(0, 0, bz);
        glTranslatef(self_zrp[0], self_zrp[1], self_zrp[2]);
        glMultMatrixd(modelview);
        self_dist_cam_zpr += bz;
        // Now we make the new projection view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        self_z_near -= bz;
        self_z_far -= bz;
        self_fog_start -= bz;
        self_fog_end -= bz;
        // Depending how near we are from the z_near clipping plane, we need
        // to put some boundaries to avoid unexpected behaviors. Same with
        // the z_far clipping plane
        if (self_z_near > 0.1)
            gluPerspective(self_fovy, float(widget->allocation.width)/float(widget->allocation.height), self_z_near, self_z_far);
        else
            if (self_z_far <= 0.15)
                gluPerspective(self_fovy, float(widget->allocation.width)/float(widget->allocation.height), 0.1, 0.15);
            else
                gluPerspective(self_fovy, float(widget->allocation.width)/float(widget->allocation.height), 0.1, self_z_far);
        glFogf(GL_FOG_START, self_fog_start);
        glFogf(GL_FOG_END, self_fog_end);
        glMatrixMode(GL_MODELVIEW);
        changed = true;
    }
    else{
        if (self_mouse_rotate){
            // Functionality copied from the python version. I need to know how
            // this function works, right now is lika a black box
            glMatrixMode(GL_MODELVIEW);
            double angle = std::sqrt(dx*dx + dy*dy)/double(widget->allocation.width + 1)*180.0;
            GLdouble modelview[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            double bx, by, bz;
            bx = modelview[0]*dy + modelview[1]*dx;
            by = modelview[4]*dy + modelview[5]*dx;
            bz = modelview[8]*dy + modelview[9]*dx;
            glTranslatef(self_zrp[0], self_zrp[1], self_zrp[2]);
            glRotatef(angle,bx,by,bz);
            glTranslatef(-self_zrp[0], -self_zrp[1], -self_zrp[2]);
            changed = true;
        }
        else{
            if (self_mouse_pan){
                // The mouse pan function needs to be corrected to have
                // better behavior when the screen is far and near
                glMatrixMode(GL_MODELVIEW);
                double px, py, pz;
                pos(self_mouse_x, self_mouse_y, px, py, pz);
                GLdouble modelview[16];
                glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
                glLoadIdentity();
                glTranslatef((px - self_drag_pos_x)*self_z_far/10,
                             (py - self_drag_pos_y)*self_z_far/10,
                             (pz - self_drag_pos_z)*self_z_far/10);
                glMultMatrixd(modelview);
                self_drag_pos_x = px;
                self_drag_pos_y = py;
                self_drag_pos_z = pz;
                changed = true;
            }
        }
    }
    if (changed){
        gtk_widget_queue_draw(widget);
    }
    if (event->state && GDK_BUTTON2_MASK){
        self_dragging = true;
    }
    
    return true;
}

gboolean MyDrawArea::mouse_released(GtkWidget *widget, GdkEventButton *event, gpointer data){
    /*
     * 
     */
    self_mouse_pan = false;
    self_mouse_rotate = false;
    self_mouse_zoom = false;
    double m_x, m_y;
    m_x = event->x;
    m_y = event->y;
    self_mouse_x = event->x;
    self_mouse_y = event->y;
    //pos(m_x, m_y, self_drag_pos_x, self_drag_pos_y, self_drag_pos_z);
    if ((event->button == 1) && (event->type == GDK_BUTTON_RELEASE)){
        if ((std::abs(self_pos_mouse[0]-m_x) >= self_pick_rad[0]) && 
            (std::abs(self_pos_mouse[1]-m_y) >= self_pick_rad[1])){
            pick(m_x, widget->allocation.height -1- m_y, self_pick_rad[0], self_pick_rad[1]);
            
        }
        else{
            self_pos_mouse[0] = 0;
            self_pos_mouse[1] = 0;
        }
    }
    if ((event->button == 2) && (event->type == GDK_BUTTON_RELEASE)){
        if (self_dragging){
            glMatrixMode(GL_MODELVIEW);
            GLdouble modelview[16];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            double dir_vec[3] = {modelview[2], modelview[6], modelview[10]};
            double cam_pos[3];
            get_cam_pos(cam_pos);
            for (int i = 0; i < 3; i++)
                dir_vec[i] *= -self_dist_cam_zpr;
            double new_zrp[3] = {cam_pos[0]+dir_vec[0], cam_pos[1]+dir_vec[1], cam_pos[2]+dir_vec[2]};
            for (int i = 0; i < 3; i++)
                self_zrp[i] = new_zrp[i];
            //glTranslatef(self_zrp[0], self_zrp[1], self_zrp[2]);
            
            //dir_vec[0] = -self_dist_cam_zpr * modelview[0];
            //dir_vec[1] = -self_dist_cam_zpr * modelview[1];
            //dir_vec[2] = -self_dist_cam_zpr * modelview[2];
            //get_cam_pos(cam_pos);
            //dir_vec[0] = cam_pos[0] + dir_vec[0];
            //dir_vec[1] = cam_pos[1] + dir_vec[1];
            //dir_vec[2] = cam_pos[2] + dir_vec[2];
            //self_zrp[0] = dir_vec[0];
            //self_zrp[1] = dir_vec[1];
            //self_zrp[2] = dir_vec[2];
            self_dragging = false;
            std::cout << "llega aca???" << std::endl;
        }
        else{
            //nearest, hits = self.pick(x, self.get_allocation().height-1-y, self.pick_radius[0], self.pick_radius[1], event)
            //selected = self.select(event,nearest,hits)
            //if selected is not None:
                //self.center_on_atom(selected.pos)
                //self.zero_reference_point = selected.pos
                //self.target_point = selected.pos
        }
    }
    
    
    //std::cout << "mouse released" << std::endl;
    return true;
}

void MyDrawArea::get_cam_pos(double *cam_pos){
    /*
     * 
     */
    glm::vec3 trans;
    GLdouble modv[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modv);
    glm::mat3 inv = glm::inverse(glm::mat3(modv[0], modv[1], modv[2], modv[4], modv[5], modv[6], modv[8], modv[9], modv[10]));
    trans.x = modv[12]; trans.y = modv[13]; trans.z = modv[14];
    trans = inv * trans;
    cam_pos[0] = -trans.x; cam_pos[1] = -trans.y; cam_pos[2] = -trans.z;
    //return cam_pos;
    //GLdouble modelview[16], temp_mat[3];
    //glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    //temp_mat[0] = modelview[12];
    //temp_mat[1] = modelview[13];
    //temp_mat[2] = modelview[14];
    //cam_pos[0] = -1.0 * (temp_mat[0]*modelview[0] + temp_mat[1]*modelview[1] + temp_mat[2]*modelview[2]);
    //cam_pos[1] = -1.0 * (temp_mat[0]*modelview[4] + temp_mat[1]*modelview[5] + temp_mat[2]*modelview[6]);
    //cam_pos[2] = -1.0 * (temp_mat[0]*modelview[8] + temp_mat[1]*modelview[9] + temp_mat[2]*modelview[10]);
}

void MyDrawArea::pick(double x, double y, double dx, double dy){
    /*
     * 
     */
    GLuint buff[256];
    glSelectBuffer(256, buff);
    glRenderMode(GL_SELECT);
    glInitNames();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glLoadIdentity();
    gluPickMatrix(x, y, dx, dy, viewport);
    glMultMatrixd(projection);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //<----------->draw_to_pick();
    draw();
    glPopMatrix();
    GLint hits = glRenderMode(GL_RENDER);
    
    if (hits != 0){
        std::cout << "something was picked" << std::endl;
        std::cout << hits << std::endl;
        //if (hit.names.length > 0) && ((min_z is None) or (hit.near < min_z)):
            //min_z = hit.near
            //nearest = hit.names
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

gboolean MyDrawArea::mouse_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data){
    /*
     * 
     */
    if (event->direction == GDK_SCROLL_DOWN){
        self_z_near += self_scroll;
        self_z_far -= self_scroll;
        self_fog_start -= self_scroll;
        self_fog_end -= self_scroll;
    }
    if (event->direction == GDK_SCROLL_UP){
        if (self_z_near >= self_z_far)
            return true;
        else{
            self_z_near -= self_scroll;
            self_z_far += self_scroll;
            self_fog_start += self_scroll;
            self_fog_end += self_scroll;
        }
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (self_z_near >= 0.1)
        gluPerspective(self_fovy, double(widget->allocation.width)/double(widget->allocation.height), self_z_near, self_z_far);
    else
        gluPerspective(self_fovy, double(widget->allocation.width)/double(widget->allocation.height), 0.1, self_z_far);
    glFogf(GL_FOG_START, self_fog_start);
    glFogf(GL_FOG_END, self_fog_end);
    glMatrixMode(GL_MODELVIEW);
    gtk_widget_queue_draw(widget);
    return true;
}

gboolean MyDrawArea::key_press(GtkWidget *widget, GdkEventKey *event, gpointer data){
    /*
     * 
     */
    //std::cout << "key pressed: " << event->keyval << std::endl;
    if (event->keyval == 65307)
        gtk_main_quit();
    if (event->keyval == 108)
        load_mol();
    
    return true;
}

void MyDrawArea::pos(double wind_x, double wind_y, double &px, double &py, double &pz){
    /*
     * 
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    px = double(wind_x-viewport[0]) / double(viewport[2]);
    py = double(wind_y-viewport[1]) / double(viewport[3]);
    px = self_left + px * (self_right - self_left);
    py = self_top + py * (self_bottom - self_top);
    pz = self_z_near;
}

bool MyDrawArea::draw_dots(){
    /*
     * 
     */
    
    //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //glClearColor(self_bckgrnd_color[0],self_bckgrnd_color[1],self_bckgrnd_color[2],self_bckgrnd_color[3]);
    //if (self_dots)
        //glCallList(self_gl_point_list[0], GL_COMPILE);
    
    //GLunit gl_dot_li = glGenLists(1);
    //glNewList(gl_dot_li, GL_COMPILE);
    //if (self_frame != NULL){
    //for (int i = 0; i < self_frame.atoms.size(); i++){
        //glPushMatri();
        //glPushName(self_frame.atoms[i].index);
        //glColor3f(0.0, 1.0, 0.0);
        //glPointSize(5);
        //glBegin(GL_POINTS);
        //glVertex3f(self_frame.atoms[i].pos[0], self_frame.atoms[i].pos[1], self_frame.atoms[i].pos[2]);
        //glEnd();
        //glPopName();
        //plPopMatrix();
    //}
    //}
    //glEndList;
    return true;
}

void MyDrawArea::load_mol(){
    /*
     * 
     */
    GLuint gl_pt_li = glGenLists(1);
    glNewList(gl_pt_li, GL_COMPILE);
    for (int i = 1; i <= self_data_frames.atoms.size(); i++){
    //for (atom : self_data_frames.atoms){
        glPushMatrix();
        glPushName(self_data_frames.atoms[i].index);
        glColor3f(self_data_frames.atoms[i].color[0], self_data_frames.atoms[i].color[1], self_data_frames.atoms[i].color[2]);
        glPointSize(10);
        glBegin(GL_POINTS);
        glVertex3f(self_data_frames.atoms[i].pos[0], self_data_frames.atoms[i].pos[1], self_data_frames.atoms[i].pos[2]);
        glEnd();
        glPopName();
        glPopMatrix();
    }
    glEndList();
    self_gl_point_list.push_back(gl_pt_li);
    self_dots = true;
}

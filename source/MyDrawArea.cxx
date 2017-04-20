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
#include <map>
#include "MolecularModel.h"
#include "MyDrawArea.h"


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
// Mouse settings
bool MyDrawArea::self_mouse_rotate = false;
bool MyDrawArea::self_mouse_zoom = false;
bool MyDrawArea::self_mouse_pan = false;
bool MyDrawArea::self_dragging = false;
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


Frame MyDrawArea::self_data_frames;

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
    gluPerspective(self_fovy, 640.0/420.0, self_z_near, self_z_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
    
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
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(self_fovy, widget->allocation.width/widget->allocation.height, self_z_near, self_z_far);
    glMatrixMode(GL_MODELVIEW);
    
    gdk_gl_drawable_gl_end (gl_drawable);
    return true;
}

gboolean MyDrawArea::my_draw(GtkWidget *widget, GdkEventExpose *event, gpointer data){
    /*
     * Function Description
     */
    GdkGLContext *gl_context = gtk_widget_get_gl_context (widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable (widget);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);
    glMatrixMode(GL_MODELVIEW);
    
    draw(widget);
    
    if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context))
	return false;
    
    if (gdk_gl_drawable_is_double_buffered (gl_drawable))
	gdk_gl_drawable_swap_buffers(gl_drawable);
    else
	glFlush ();
    
    gdk_gl_drawable_gl_end (gl_drawable);
    return true;
}

void MyDrawArea::draw(GtkWidget *widget){
    /*
     * The actual drawing goes here.
     */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float cube0[] = { 1, 0, 1};
    float cube1[] = {-1, 0, 1};
    float cube2[] = {-1, 1, 1};
    float cube3[] = { 1, 1, 1};
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(cube1[0], cube1[1], cube1[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(cube2[0], cube2[1], cube2[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(cube3[0], cube3[1], cube3[2]);
    glColor3f(0.0, 1.0, 1.0); glVertex3f(cube0[0], cube0[1], cube0[2]);
    
    glEnd();
}

gboolean MyDrawArea::mouse_pressed(GtkWidget *widget, GdkEventButton *event, gpointer data){
    /*
     * The actual drawing goes here.
     */
    bool m_left = false, m_middle = false, m_right = false;
    double m_x, m_y;
    if (event->button == 1){
	std::cout << "left click pressed" << std::endl;
	m_left = true;
    }
    if (event->button == 2){
	std::cout << "middle click pressed" << std::endl;
	m_middle = true;
    }
    if (event->button == 3){
	std::cout << "right click pressed" << std::endl;
	m_right = true;
    }
    self_mouse_rotate = m_left && !(m_middle || m_right);
    self_mouse_zoom = m_right && !(m_middle || m_left);
    self_mouse_pan = m_middle && !(m_left || m_right);
    m_x = event->x;
    m_y = event->y;
    self_mouse_x = event->x;
    self_mouse_y = event->y;
    pos(m_x, m_y, self_drag_pos_x, self_drag_pos_y, self_drag_pos_z);
    if ((event->button == 1) && (event->type == GDK_2BUTTON_PRESS)){
	//nearest, hits = self.pick(x, self.get_allocation().height-1-y, self.pick_radius[0], self.pick_radius[1], event)
	//selected = self.select(event, nearest, hits)
    }
    //if (selected != NULL){
	//self.center_on_atom(selected.pos)
	//self.zero_reference_point = selected.pos
	//self.target_point = selected.pos
    //}
    if ((event->button == 2) && (event->type == GDK_BUTTON_PRESS)){
	//self.dist_cam_zpr = op.get_euclidean(self.zero_reference_point, self.get_cam_pos());
    }
    if ((event->button == 1) && (event->type == GDK_BUTTON_PRESS)){
	self_pos_mouse[0] = m_x;
	self_pos_mouse[1] = m_y;
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
    pos(m_x, m_y, self_drag_pos_x, self_drag_pos_y, self_drag_pos_z);
    if ((event->button == 1) && (event->type == GDK_BUTTON_RELEASE)){
	if (self_dragging){
	    self_dragging = false;
	}
	else{
	    if (){
		
	    }
	    
	}
	
	
    }
    
    std::cout << "mouse released" << std::endl;
    return true;
}

gboolean MyDrawArea::mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data){
    /*
     * 
     */
    //std::cout << "mouse moved" << std::endl;
    return true;
}

gboolean MyDrawArea::mouse_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data){
    /*
     * 
     */
    std::cout << "mouse scrolled" << std::endl;
    return true;
}

gboolean MyDrawArea::key_press(GtkWidget *widget, GdkEventKey *event, gpointer data){
    /*
     * 
     */
    std::cout << "key pressed" << std::endl;
    return true;
}

void MyDrawArea::pos(double wind_x, double wind_y, double &px, double &py, double &pz){
    /*
     * 
     */
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    px = (wind_x - viewport[0])/(viewport[2]);
    py = (wind_y - viewport[1])/(viewport[3]);
    px = self_left + px * (self_right - self_left);
    py = self_top + py * (self_bottom - self_top);
    pz = self_z_near;
}

bool MyDrawArea::draw_dots(){
    /*
     * 
     */
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

//
// Created by alwint on 9/2/24.
//

#ifndef TEST_BUTTONS_H
#define TEST_BUTTONS_H

#include<gtk/gtk.h>
#include<iostream>

void clicked_button(GtkApplication *app, gpointer user_data);

void pos_targets(GtkTextBuffer *buffer, std::string x, std::string y);

GtkWidget *writ_button();

void create_grid(GtkDrawingArea *widget, gpointer user_data);

//button UIs export
GtkWidget *grid_creator();

#endif //TEST_BUTTONS_H

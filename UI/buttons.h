//
// Created by alwint on 9/2/24.
//

#ifndef TEST_BUTTONS_H
#define TEST_BUTTONS_H

#include<gtk/gtk.h>
#include<iostream>

void clicked_button(GtkApplication *app,gpointer user_data);
void pos_targets(GtkWidget *button, std::string x, std::string y);

#endif //TEST_BUTTONS_H

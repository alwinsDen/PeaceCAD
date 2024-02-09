//
// Created by alwint on 9/2/24.
//

#include "buttons.h"
#include <gtk/gtk.h>
#include <iostream>

//this is the connection function from button
void clicked_button(GtkApplication *app,gpointer user_data){
    std::cout<<"Here is button click "<<"\n";
}

//get the mouse position on live
void pos_targets(GtkWidget *button, std::string x, std::string y){
    //combine the string
    std::string comb_pos = "X: "+x+"    Y "+y;
    const char *fmt_text = comb_pos.c_str();
    gtk_button_set_label(GTK_BUTTON(button),fmt_text);
}
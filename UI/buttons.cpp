//
// Created by alwint on 9/2/24.
//

#include "buttons.h"
#include <gtk/gtk.h>
#include <iostream>
#include <vector>
#include "../globals.h"

//A function to draw the grids
void create_grid(GtkWidget *widget, cairo_t *cr,gpointer user_data) {
//    set_grid_state(!get_grid_state());
    set_grid_state(true);
    gtk_widget_queue_draw(GTK_WIDGET(user_data));
};


//this is the connection function from button
void clicked_button(GtkApplication *app, gpointer user_data) {
    std::cout << "Here is button click " << "\n";
}

//get the mouse position on live
void pos_targets(GtkTextBuffer *buffer, std::string x, std::string y) {
    //combine the string
    std::string comb_pos = "X: " + x + "    Y: " + y;
    const char *fmt_text = comb_pos.c_str();
    gtk_text_buffer_set_text(buffer, fmt_text, -1);
}

//grid drawer buttons
GtkWidget *grid_creator() {
    GtkWidget *grid_tool = gtk_button_new_with_label("➖");
    gtk_widget_set_margin_start(GTK_WIDGET(grid_tool), 5);
    gtk_widget_set_margin_top(GTK_WIDGET(grid_tool), 5);
    return grid_tool;
}

GtkWidget *writ_button() {
    //creating a vertical box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *pencil_tool = gtk_button_new_with_label("✒\uFE0F");

    //apply styles
    std::vector<GtkWidget *> l_wdgs = {pencil_tool};
    for (auto vls: l_wdgs) {
        gtk_widget_set_margin_start(GTK_WIDGET(vls), 5);
        gtk_widget_set_margin_top(GTK_WIDGET(vls), 5);
    }

    gtk_box_append(GTK_BOX(box), pencil_tool);

    return box;
}
//
// Created by alwint on 8/2/24.
//
#include <iostream>
#include "drawing.h"
#include <gtk/gtk.h>
#include "UI/buttons.h"
#include "globals.h"

static cairo_surface_t *surface = NULL;

//struct definitions
struct CustomPosData {
    std::string xyz;
    GtkTextBuffer *widget{};
};

//don't use the static keyword here.
//clear the whiteboard
void clear_surface() {
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

//resize CB function
void resize_cb(
        GtkWidget *widget,
        int width,
        int height,
        gpointer data
) {
    if (surface) {
        cairo_surface_destroy(surface);
        surface = NULL;
    }
    if (gtk_native_get_surface(gtk_widget_get_native(widget))) {
        surface = gdk_surface_create_similar_surface(
                gtk_native_get_surface(gtk_widget_get_native(widget)),
                CAIRO_CONTENT_COLOR,
                gtk_widget_get_width(widget),
                gtk_widget_get_height(widget)
        );
        //reset the whiteboard
        clear_surface();
    }
}

void draw_cb(
        GtkDrawingArea *drawing_area,
        cairo_t *cr,
        int height,
        int width,
        gpointer data
) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    //conditional check for the grid state
    if (get_grid_state()){
        //if the drawing area is considered - get dimensions.
        GtkAllocation allocation;
        gtk_widget_get_allocation(reinterpret_cast<GtkWidget *>(drawing_area), &allocation);
        int ww = allocation.width;
        int hh = allocation.height;
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        cairo_set_line_width(cr, 1);
        int grid_spacing = 40;
        for (int i = 0; i < ww; i += grid_spacing) {
            cairo_move_to(cr, i, 0);
            cairo_line_to(cr, i, hh);
            cairo_stroke(cr);
        }
        for (int i = 0; i < hh; i += grid_spacing) {
            cairo_move_to(cr, 0, i);
            cairo_line_to(cr, ww, i);
            cairo_stroke(cr);
        }
    }
}

//drag functions and the draw brush
void draw_brush(GtkWidget *widget, double x, double y) {
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_rectangle(cr, x - 3, y - 3, 4, 4);
    cairo_fill(cr);
    cairo_destroy(cr);
    // invalidating drawing area.
    // this functions redraws the current painting when draw_brush is repeatedly
    //called
    gtk_widget_queue_draw(widget);
}

//here we create the drag functions
static double start_x;
static double start_y;

void drag_begin(
        GtkGestureDrag *gesture,
        double x,
        double y,
        GtkWidget *area
) {
    start_x = x;
    start_y = y;
    draw_brush(area, x, y);
}

void drag_update(
        GtkGestureDrag *gesture,
        double x,
        double y,
        GtkWidget *area
) {
    draw_brush(area, start_x + x, start_y + y);
}

void drag_end(
        GtkGestureDrag *gesture,
        double x,
        double y,
        GtkWidget *area
) {
    draw_brush(area, start_x + x, start_y + y);
}

//Get realtime postion of cursor
void get_cursor_position(
        GtkEventControllerMotion *controller,
        double x,
        double y,
        gpointer user_data
) {
    //deserializing a struct
    CustomPosData *data = static_cast<CustomPosData * >(user_data);
    pos_targets(data->widget, std::to_string(x), std::to_string(y));
}

void pressed(GtkGestureClick *gesture,
             int n_press,
             double x,
             double y,
             GtkWidget *area) {
    clear_surface();
    gtk_widget_queue_draw(area);
}

void close_window() {
    if (surface) {
        cairo_surface_destroy(surface);
    }
}

void activate_drawing(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;
    GtkWidget *box;
    GtkGesture *drag;
    GtkGesture *press;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "PeaceCAD v0.0.1");

    //adding box the frame
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    //the signal for closing function
    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    frame = gtk_frame_new(NULL);
    gtk_widget_set_margin_top(GTK_WIDGET(frame), 5);
    gtk_widget_set_margin_end(GTK_WIDGET(frame), 5);
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1920, 1080);
    gtk_frame_set_child(GTK_FRAME(frame), drawing_area);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_cb, NULL, NULL);

    //the signal for resizing
    g_signal_connect_after(drawing_area, "resize", G_CALLBACK(resize_cb), NULL);

    //adding text box
    GtkWidget *text_box;
    GtkTextBuffer *buffer;
    text_box = gtk_text_view_new();
    gtk_widget_set_margin_top(GTK_WIDGET(text_box), 4);
    gtk_widget_set_margin_bottom(GTK_WIDGET(text_box), 4);
    gtk_widget_set_margin_start(GTK_WIDGET(text_box), 10);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_box), FALSE);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_box));

    //here we are defining the content for the CustomPostData struct
    CustomPosData *data = new CustomPosData();
    data->xyz = "Hey there";
    data->widget = buffer;

    //Adding button to the UI
    GtkWidget *button = gtk_button_new_with_label("Click Here");
    g_signal_connect(button, "clicked", G_CALLBACK(clicked_button), NULL);

    //trying to link motion position
    GtkEventController *motion = gtk_event_controller_motion_new();

    g_signal_connect(motion, "motion", G_CALLBACK(get_cursor_position), data);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(motion));

    drag = gtk_gesture_drag_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(drag), GDK_BUTTON_PRIMARY);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(drag));

    //signal connect the drag functions
    g_signal_connect(drag, "drag-begin", G_CALLBACK(drag_begin), drawing_area);
    g_signal_connect(drag, "drag-update", G_CALLBACK(drag_update), drawing_area);
    g_signal_connect(drag, "drag-end", G_CALLBACK(drag_end), drawing_area);

    press = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(press), GDK_BUTTON_SECONDARY);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(press));

    g_signal_connect(press, "pressed", G_CALLBACK(pressed), drawing_area);

    //add clicker to grid creation
    GtkWidget *grid_btt_ref = grid_creator();
    g_signal_connect(grid_btt_ref, "clicked", G_CALLBACK(create_grid), drawing_area);
    GtkWidget *box_inst_ref = writ_button();
    gtk_box_append(GTK_BOX(box_inst_ref), grid_btt_ref);

    //defining a new GTK box with Horizontal orientation
    GtkWidget *vert_boxed = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(vert_boxed), box_inst_ref);
    gtk_box_append(GTK_BOX(vert_boxed), frame);

    //GTK Box material controls
    gtk_box_append(GTK_BOX(box), vert_boxed);
    gtk_box_append(GTK_BOX(box), text_box);
//    gtk_box_append(GTK_BOX(box),button); //unused button

    gtk_window_present(GTK_WINDOW(window));
}
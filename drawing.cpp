//
// Created by alwint on 8/2/24.
//
#include <iostream>
#include "drawing.h"
#include <gtk/gtk.h>

static cairo_surface_t *surface = NULL;

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
}

//drag functions and the draw brush
void draw_brush(GtkWidget *widget, double x, double y) {
    cairo_t *cr;
    cr = cairo_create(surface);
    cairo_rectangle(cr, x - 3, y - 3, 6, 6);
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
        GtkWidget *widget
) {
    std::cout << "X: "<<x<<"Y: "<< y << "\n";
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
    gtk_window_set_title(GTK_WINDOW(window), "Drawing area");

    //adding box the frame
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window),box);

    //the signal for closing function
    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    frame = gtk_frame_new(NULL);

    gtk_box_append(GTK_BOX(box),frame);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1920, 1080);
    gtk_frame_set_child(GTK_FRAME(frame), drawing_area);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_cb, NULL, NULL);

    //the signal for resizing
    g_signal_connect_after(drawing_area, "resize", G_CALLBACK(resize_cb), NULL);

    //trying to link motion position
    GtkEventController *motion = gtk_event_controller_motion_new();
    g_signal_connect(motion, "motion", G_CALLBACK(get_cursor_position), drawing_area);
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

    gtk_window_present(GTK_WINDOW(window));
}
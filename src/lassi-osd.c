#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <gdk/gdkx.h>

#include <string.h>

#include "lassi-osd.h"

static gboolean draw_cb(GtkWidget* widget, cairo_t *cr, gpointer user_data) {
    GtkAllocation  allocation = {0, 0, -1, -1};

    gtk_widget_get_allocation (widget, &allocation);

    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

    cairo_arc (cr,
               allocation.width - 5.5,
               5.5,
               5.0, G_PI / -2, 0.0);
    cairo_arc (cr,
               allocation.width - 5.5,
               allocation.height - 5.5,
               5.0, 0.0, G_PI / 2);
    cairo_arc (cr,
               5.5,
               allocation.height - 5.5,
               5.0, G_PI / 2, G_PI);
    cairo_arc (cr,
               5.5,
               5.5,
               5.0, G_PI, G_PI * 3 / 2);
    cairo_close_path (cr);
    cairo_set_source_rgba (cr,
                           1.0 * 0x26 / 0xff,
                           1.0 * 0x26 / 0xff,
                           1.0 * 0x24 / 0xff,
                           1.0 * 0xcfffffff / 0xffffffff);
    cairo_fill_preserve (cr);
    cairo_set_source_rgb (cr,
                          1.0 * 0xba / 0xff,
                          1.0 * 0xbd / 0xff,
                          1.0 * 0xb6 / 0xff);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke (cr);
    cairo_destroy (cr);

    return FALSE;
}

int lassi_osd_init(LassiOsdInfo *osd) {
    GtkWidget *hbox;

    g_assert(osd);

    memset(osd, 0, sizeof(*osd));

    osd->window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_title(GTK_WINDOW(osd->window), "Mango Lassi OSD");
    gtk_window_stick(GTK_WINDOW(osd->window));
    gtk_window_set_keep_above(GTK_WINDOW(osd->window), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(osd->window), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(osd->window), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(osd->window), GDK_WINDOW_TYPE_HINT_NOTIFICATION);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(osd->window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(osd->window), TRUE);
    gtk_window_set_accept_focus(GTK_WINDOW(osd->window), FALSE);
    gtk_window_set_focus_on_map(GTK_WINDOW(osd->window), FALSE);
    gtk_window_set_gravity(GTK_WINDOW(osd->window), GDK_GRAVITY_SOUTH_WEST);
    gtk_widget_set_app_paintable(GTK_WIDGET(osd->window), TRUE);
    g_signal_connect(osd->window, "draw", G_CALLBACK(draw_cb), NULL);

    osd->label = gtk_label_new("Test");
    gtk_misc_set_padding(GTK_MISC(osd->label), 16, 0);
/*     gtk_label_set_line_wrap(GTK_LABEL(osd->label), TRUE);  */
    osd->left_icon = gtk_image_new();
    osd->right_icon = gtk_image_new();

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);

    gtk_box_pack_start(GTK_BOX(hbox), osd->left_icon, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), osd->label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(hbox), osd->right_icon, FALSE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(osd->window), hbox);

    gtk_widget_show(hbox);
    gtk_widget_show(osd->label);

    gtk_widget_realize(GTK_WIDGET(osd->window));

    /*g_debug("WINDOW=%p", osd->window);*/

    return 0;
}

void lassi_osd_done(LassiOsdInfo *osd) {
    g_assert(osd);

    gtk_widget_destroy(osd->window);

    memset(osd, 0, sizeof(*osd));
}

void lassi_osd_set_text(LassiOsdInfo *osd, const char *text, const char *icon_name_left, const char *icon_name_right) {
    char *t;
    int w, h, max_width;

    g_assert(osd);
    g_assert(osd->window);

    /*g_debug("WINDOW=%p", osd->window);*/

    g_debug("Showing text '%s'", text);

    t = g_strdup_printf("<span size=\"large\" color=\"#F5F5F5\">%s</span>", text);
    gtk_label_set_markup(GTK_LABEL(osd->label), t);
    g_free(t);

    if (icon_name_left) {
        gtk_image_set_from_icon_name(GTK_IMAGE(osd->left_icon), icon_name_left, GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(osd->left_icon);
    } else
        gtk_widget_hide(osd->left_icon);

    if (icon_name_right) {
        gtk_image_set_from_icon_name(GTK_IMAGE(osd->right_icon), icon_name_right, GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(osd->right_icon);
    } else
        gtk_widget_hide(osd->right_icon);

    max_width = (gdk_screen_get_width(gtk_widget_get_screen(osd->window))*18)/20;

    /*g_debug("WINDOW=%p", osd->window);*/

    gtk_widget_set_size_request(osd->window, -1, -1);

    gtk_window_get_size(GTK_WINDOW(osd->window), &w, &h);

    /*g_debug("WINDOW=%p", osd->window);*/

    if (w > max_width) {
        gtk_widget_set_size_request(osd->window, max_width, -1);
        w = max_width;
    }

    if (!icon_name_left == !icon_name_right) {
        gtk_label_set_justify(GTK_LABEL(osd->label), GTK_JUSTIFY_CENTER);
        gtk_window_move(GTK_WINDOW(osd->window),
                        (gdk_screen_get_width(gtk_widget_get_screen(osd->window)) - w)/2,
                        (gdk_screen_get_height(gtk_widget_get_screen(osd->window))*9)/10 - h);
    } else if (icon_name_left) {
        gtk_label_set_justify(GTK_LABEL(osd->label), GTK_JUSTIFY_LEFT);
        gtk_window_move(GTK_WINDOW(osd->window),
                        gdk_screen_get_width(gtk_widget_get_screen(osd->window))/20,
                        (gdk_screen_get_height(gtk_widget_get_screen(osd->window))*9)/10 - h);
    } else {
        gtk_label_set_justify(GTK_LABEL(osd->label), GTK_JUSTIFY_RIGHT);
        gtk_window_move(GTK_WINDOW(osd->window),
                        (gdk_screen_get_width(gtk_widget_get_screen(osd->window))*19)/20 - w,
                        (gdk_screen_get_height(gtk_widget_get_screen(osd->window))*9)/10 - h);
    }

    gtk_widget_show(osd->window);

    g_debug("osd shown");
}

void lassi_osd_hide(LassiOsdInfo *osd) {
    g_assert(osd);

    gtk_widget_hide(osd->window);

    g_debug("osd hidden");
}

/* vim:set sw=4 et: */

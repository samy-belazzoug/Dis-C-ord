#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include <gtk/gtk.h>

GtkWidget* create_main_screen(GtkApplication *app, const char* canal_nom, const char* canal_type, const char* user_role);

#endif

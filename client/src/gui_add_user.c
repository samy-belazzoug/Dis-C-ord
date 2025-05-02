#include "gui_add_user.h"

void open_add_user_window(GtkWidget *parent) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Ajouter un utilisateur");
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *label = gtk_label_new("Sélectionner un utilisateur :");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    GtkWidget *combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "user1@example.com");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "user2@example.com");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "user3@example.com");
    gtk_box_pack_start(GTK_BOX(box), combo, FALSE, FALSE, 0);

    GtkWidget *add_button = gtk_button_new_with_label("Ajouter");
    gtk_box_pack_start(GTK_BOX(box), add_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

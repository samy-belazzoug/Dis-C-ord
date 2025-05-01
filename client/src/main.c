#include <gtk/gtk.h>

void on_show_password_toggled(GtkToggleButton *toggle, gpointer entry) {
    gboolean active = gtk_toggle_button_get_active(toggle);
    gtk_entry_set_visibility(GTK_ENTRY(entry), active);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *email_entry;
    GtkWidget *password_entry;
    GtkWidget *show_password_check;
    GtkWidget *login_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Adresse e-mail");
    gtk_box_pack_start(GTK_BOX(main_box), email_entry, FALSE, FALSE, 0);

    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(password_entry), '*');
    gtk_box_pack_start(GTK_BOX(main_box), password_entry, FALSE, FALSE, 0);

    show_password_check = gtk_check_button_new_with_label("Afficher le mot de passe");
    gtk_box_pack_start(GTK_BOX(main_box), show_password_check, FALSE, FALSE, 0);
    g_signal_connect(show_password_check, "toggled", G_CALLBACK(on_show_password_toggled), password_entry);

    login_button = gtk_button_new_with_label("Se connecter");
    gtk_box_pack_start(GTK_BOX(main_box), login_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.mydiscord.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}



#include <gtk/gtk.h>
#include "gui_main.h"

void on_show_password_toggled(GtkToggleButton *toggle, gpointer entry) {
    gboolean active = gtk_toggle_button_get_active(toggle);
    gtk_entry_set_visibility(GTK_ENTRY(entry), active);
}


typedef struct {
    GtkWidget *email_entry;
    GtkWidget *password_entry;
    GtkApplication *app;
    GtkWidget *login_window;
} LoginData;

void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    LoginData *data = (LoginData *)user_data;

    const gchar *email = gtk_entry_get_text(GTK_ENTRY(data->email_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(data->password_entry));

    //  plus tard faut que jajt une vraie vérification des identifiants
    if (g_strcmp0(email, "") != 0 && g_strcmp0(password, "") != 0) {

        gtk_widget_hide(data->login_window);


        create_main_screen(data->app, "#général", "Public", "Admin");
    } else {
        g_print("Erreur : email ou mot de passe vide\n");
    }
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


    LoginData *data = g_malloc(sizeof(LoginData));
    data->email_entry = email_entry;
    data->password_entry = password_entry;
    data->app = app;
    data->login_window = window;

    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), data);

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



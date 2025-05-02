#include "gui_main.h"
#include <string.h>
#include <stdlib.h> 

#include "gui_add_user.h"


typedef struct {
    GtkWidget *entry;
    GtkWidget *text_view;
} SendMessageData;


void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    SendMessageData *data = (SendMessageData *)user_data;
    const gchar *message = gtk_entry_get_text(GTK_ENTRY(data->entry));

    if (strlen(message) == 0) return;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text_view));
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    gtk_text_buffer_insert(buffer, &end_iter, message, -1);
    gtk_text_buffer_insert(buffer, &end_iter, "\n", -1);

    gtk_entry_set_text(GTK_ENTRY(data->entry), "");
}

GtkWidget* create_main_screen(GtkApplication *app, const char* canal_nom, const char* canal_type, const char* user_role) {
    GtkWidget *window, *main_box, *header_box;
    GtkWidget *canal_label, *role_label, *canal_type_label;
    GtkWidget *message_view, *entry, *send_btn;
    GtkWidget *sidebar, *add_user_btn, *top_box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord - Canal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), top_box);


    sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar, 200, -1);
    gtk_box_pack_start(GTK_BOX(top_box), sidebar, FALSE, FALSE, 0);

    GtkWidget *channels_label = gtk_label_new("Canaux");
    gtk_box_pack_start(GTK_BOX(sidebar), channels_label, FALSE, FALSE, 5);



    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(top_box), main_box, TRUE, TRUE, 0);

    // En-tête
    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    canal_label = gtk_label_new(canal_nom);
    canal_type_label = gtk_label_new(canal_type);
    role_label = gtk_label_new(user_role);
    gtk_box_pack_start(GTK_BOX(header_box), canal_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(header_box), canal_type_label, FALSE, FALSE, 10);
    gtk_box_pack_end(GTK_BOX(header_box), role_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(main_box), header_box, FALSE, FALSE, 0);


    message_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_view), FALSE);
    gtk_widget_set_vexpand(message_view, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), message_view, TRUE, TRUE, 0);


    GtkWidget *entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    entry = gtk_entry_new();
    send_btn = gtk_button_new_with_label("Envoyer");
    gtk_box_pack_start(GTK_BOX(entry_box), entry, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(entry_box), send_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), entry_box, FALSE, FALSE, 0);


    SendMessageData *msg_data = g_malloc(sizeof(SendMessageData));
    msg_data->entry = entry;
    msg_data->text_view = message_view;
    g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_button_clicked), msg_data);

    // Bouton "Ajouter un utilisateur" pour les canaux privés
    if (strcmp(canal_type, "Privé") == 0 && 
        (strcmp(user_role, "Admin") == 0 || strcmp(user_role, "Modérateur") == 0)) {
        add_user_btn = gtk_button_new_with_label("Ajouter un utilisateur");
        gtk_box_pack_start(GTK_BOX(main_box), add_user_btn, FALSE, FALSE, 0);
        g_signal_connect(add_user_btn, "clicked", G_CALLBACK(open_add_user_window), window);
    }

    gtk_widget_show_all(window);
    return window;
}


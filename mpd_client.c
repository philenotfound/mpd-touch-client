#include <stdio.h>
#include <unistd.h>
#include <mpd/client.h>
#include <gtk/gtk.h>

bool m_play(struct mpd_connection *);
bool m_toggle_pause(struct mpd_connection *);
bool m_stop(struct mpd_connection *);
bool m_next_song(struct mpd_connection *);
bool m_prev_song(struct mpd_connection *);
void m_srv_status(struct mpd_connection *, struct mpd_status *);
struct mpd_connection * client_connect(const char *, unsigned int, unsigned int);
bool client_disconnect(struct mpd_connection *);

//Globals
struct mpd_connection * m_connection = NULL;
struct mpd_status * m_status = NULL;
struct mpd_song * cur_song;
const char * host = "127.0.0.1";
int song_id;

static void gtk_toggle_pause(GtkWidget *widget, gpointer data) {

    m_srv_status(m_connection, m_status);
    m_toggle_pause(m_connection);

}

static void gtk_prev_song(GtkWidget *widget, gpointer data) {

    m_srv_status(m_connection, m_status);
    m_prev_song(m_connection);

}

static void gtk_next_song(GtkWidget *widget, gpointer data) {
    m_srv_status(m_connection, m_status);
    m_next_song(m_connection);

}

static gboolean
on_delete_event (GtkWidget *widget,
                 GdkEvent  *event,
                 gpointer   data)
{
  /* If you return FALSE in the "delete_event" signal handler,
   * GTK will emit the "destroy" signal. Returning TRUE means
   * you don't want the window to be destroyed.
   *
   * This is useful for popping up 'are you sure you want to quit?'
   * type dialogs.
   */

  g_print ("delete event occurred\n");

  return FALSE;
}


int main(int argc, char ** argv) {

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *next_button;
    GtkWidget *prev_button;
    GtkWidget *toggle_pause_button;

    m_connection = client_connect(host, 0, 0);
    m_status = mpd_run_status(m_connection);

    gtk_init(&argc, &argv);

    //Main window setup
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MPD Touch Client");
    g_signal_connect (window, "delete-event", G_CALLBACK (on_delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    //grid setup
    grid = gtk_grid_new();
    gtk_container_add (GTK_CONTAINER (window), grid);

    //prev_button setup
    prev_button = gtk_button_new_with_label ("Previous Song!");
    g_signal_connect (prev_button, "clicked", G_CALLBACK (gtk_next_song), NULL);
    gtk_widget_set_hexpand (prev_button, TRUE);
    gtk_widget_set_vexpand (prev_button, TRUE);
    gtk_grid_attach (GTK_GRID (grid), prev_button, 0, 0, 1, 1);
    gtk_widget_show (prev_button);

    //next_button setup
    next_button = gtk_button_new_with_label ("Next Song!");
    g_signal_connect (next_button, "clicked", G_CALLBACK (gtk_next_song), NULL);
    gtk_widget_set_hexpand (next_button, TRUE);
    gtk_widget_set_vexpand (next_button, TRUE);
    gtk_grid_attach (GTK_GRID (grid), next_button, 1, 0, 1, 1);
    gtk_widget_show (next_button);

    //toggle_pause_button setup
    toggle_pause_button = gtk_button_new_with_label ("Play/Pause");
    g_signal_connect (toggle_pause_button, "clicked", G_CALLBACK (gtk_toggle_pause), NULL);
    gtk_widget_set_hexpand (toggle_pause_button, TRUE);
    gtk_widget_set_vexpand (toggle_pause_button, TRUE);
    gtk_grid_attach (GTK_GRID (grid), toggle_pause_button, 0, 1, 2, 1);
    gtk_widget_show (toggle_pause_button);

    gtk_widget_show_all(window);
    gtk_window_fullscreen((GtkWindow*)window);


    gtk_main();
    client_disconnect(m_connection);

    return 0;
}


struct mpd_connection * client_connect(const char * host, unsigned int port, unsigned int timeout) {
    struct mpd_connection * m_connection = NULL;
    m_connection = mpd_connection_new(host, port, timeout); //Causing a seg fault

    if(m_connection != NULL)
        printf("YAY!!!\n");
    else
        printf("BOOO!!\n");

    if(mpd_connection_get_error(m_connection) != MPD_ERROR_SUCCESS)
        fprintf(stderr, "Could not connect to MPD: %s\n", mpd_connection_get_error_message(m_connection));

    return m_connection;

}

bool client_disconnect(struct mpd_connection * m_connection) {

    printf("Closing connection\n");
    mpd_connection_free(m_connection);
    m_connection = NULL;

    if(m_connection != NULL)
        printf("YAY!!!\n");
    else
        printf("BOOO!!\n");
}

bool m_play(struct mpd_connection * m_connection) {
    if(mpd_run_play(m_connection)) {
        printf("Playing\n");
        return true;
    } else {
        printf("Not Playing\n");
        return false;
    }
}

bool m_toggle_pause(struct mpd_connection * m_connection) {
    if(mpd_run_toggle_pause(m_connection)) {
        printf("Paused\n");
        return true;
    } else {
        printf("Not Paused\n");
        return false;
    }

}

bool m_stop(struct mpd_connection * m_connection) {
    if(mpd_run_stop(m_connection)) {
        printf("Stopped\n");
        return true;
    } else {
        printf("Not Stopped\n");
        return false;
    }

}

bool m_next_song(struct mpd_connection * m_connection) {
    
    mpd_run_next(m_connection);
}

bool m_prev_song(struct mpd_connection * m_connection) {
    mpd_run_previous(m_connection);
}

void m_srv_status(struct mpd_connection * m_connection, struct mpd_status * m_status) {
    int volume;
    char* m_state_str;

    if (mpd_status_get_state(m_status) == MPD_STATE_PLAY) {
        m_state_str = "playing";
    } else if (mpd_status_get_state(m_status) == MPD_STATE_STOP) {
        m_state_str = "stopped";
    } else if (mpd_status_get_state(m_status) == MPD_STATE_PAUSE) {
        m_state_str = "paused";
    } else {
        m_state_str = "unknown";
    }
    printf("MPD state: %s\n", m_state_str);


    if (mpd_status_get_repeat(m_status) == false)
        printf("repeat: off\n");
    else
        printf("repeat: on\n");

    if (mpd_status_get_random(m_status) == false)
        printf("random: off\n");
    else
        printf("random: on\n");

    if (mpd_status_get_single(m_status) == false)
        printf("single: off\n");
    else
        printf("single: on\n");

    if (mpd_status_get_consume(m_status) == false)
        printf("consume: off\n");
    else
        printf("consume: on\n");

    volume = mpd_status_get_volume(m_status);
    printf("volume: %d\n", volume);
}


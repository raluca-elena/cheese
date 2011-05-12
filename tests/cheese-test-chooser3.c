
#include "cheese-config.h"

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <clutter-gtk/clutter-gtk.h>
#include "cheese-avatar-chooser.h"


//#define CUSTOM_RESPONSE_CODE GTK_RESPONSE_YES
//#define CUSTOM_RESPONSE_CODE GTK_RESPONSE_OK
//#define CUSTOM_RESPONSE_CODE GTK_RESPONSE_APPLY
#define CUSTOM_RESPONSE_CODE 11

static void
response_cb (GtkDialog           *dialog,
             int                  response,
             CheeseAvatarChooser *chooser)
{
  if (response == GTK_RESPONSE_ACCEPT)
  {
    GdkPixbuf *pixbuf;
    g_message ("got pixbuf captured");
    g_object_get (G_OBJECT (chooser), "pixbuf", &pixbuf, NULL);
  }
}


static void
on_open_camera (GtkButton* button, gpointer user_data)
{
  GtkWidget *window;
  
  window = cheese_avatar_chooser_new ();
  g_signal_connect (G_OBJECT (window), "response",
                    G_CALLBACK (response_cb), window);
  gtk_widget_show (window);
}


static void
on_2button_std_code_works (GtkButton* button, gpointer user_data)
{
  GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("will work"),
                                                   GTK_WINDOW (toplevel),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_OK,
                                                   GTK_RESPONSE_ACCEPT,
                                                   GTK_STOCK_CANCEL,
                                                   GTK_RESPONSE_CANCEL,
                                                   NULL);

  gtk_dialog_run (GTK_DIALOG (dialog));
  on_open_camera(button, NULL);
  gtk_widget_destroy (dialog);
}


static void
on_2button_custom_code_works (GtkButton* button, gpointer user_data)
{
  GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("will work"),
                                                   GTK_WINDOW (toplevel),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_OK,
                                                   11,
                                                   GTK_STOCK_CANCEL,
                                                   12,
                                                   NULL);

  gtk_dialog_run (GTK_DIALOG (dialog));
  on_open_camera(button, NULL);
  gtk_widget_destroy (dialog);
}


static void
on_3button_no_works (GtkButton* button, gpointer user_data)
{
  GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("won't work"),
                                                   GTK_WINDOW (toplevel),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_OK,
                                                   GTK_RESPONSE_ACCEPT,
                                                   GTK_STOCK_CANCEL,
                                                   GTK_RESPONSE_CANCEL,
                                                   "Custom message...",
                                                   CUSTOM_RESPONSE_CODE,
                                                   NULL);

  gtk_dialog_run (GTK_DIALOG (dialog));
  on_open_camera(button, NULL);
  gtk_widget_destroy (dialog);
}


static GtkWidget*
create_window (void)
{
  GtkWidget *window;
  GtkWidget *b0, *b2std, *b2cus, *b3;
  GtkWidget *box;

  /* Set up the UI */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "cheese-test-chooser2");

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  b0    = gtk_button_new_with_label ("Open directly works...");
  b2std = gtk_button_new_with_label ("Open indirectly 2 standard buttons works...");
  b2cus = gtk_button_new_with_label ("Open indirectly 2 custom buttons works...");
  b3    = gtk_button_new_with_label ("Open indirectly 3 buttons FREEZES...");


  gtk_box_pack_start (GTK_BOX (box), b0,    FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), b2std, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), b2cus, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), b3,    FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (window), box);

  /* Connect signals */

  /* Show open dialog when opening a file */
  g_signal_connect (b0,    "clicked", G_CALLBACK ( on_open_camera ), b0);
  g_signal_connect (b2std, "clicked", G_CALLBACK ( on_2button_std_code_works ), b2std);
  g_signal_connect (b2cus, "clicked", G_CALLBACK ( on_2button_custom_code_works ), b2std);
  g_signal_connect (b3,    "clicked", G_CALLBACK ( on_3button_no_works ), b3);
  
  /* Exit when the window is closed */
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  
  return window;
}



int
main (int argc, char **argv)
{
  GtkWidget *window;

  g_thread_init (NULL);
  gdk_threads_init ();
  gst_init (&argc, &argv);

  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  gtk_clutter_init (&argc, &argv);

  window = create_window ();
  gtk_widget_show_all (window);
  
  gtk_main ();
  return 0;
}

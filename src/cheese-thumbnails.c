/*
 * Copyright (C) 2007 Copyright (C) 2007 daniel g. siegel <dgsiegel@gmail.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libgnomevfs/gnome-vfs.h>
#include <glib/gprintf.h>
#include <string.h>

#include "cheese.h"
#include "cheese-fileutil.h"
#include "cheese-thumbnails.h"
#include "cheese-window.h"

enum {
  PIXBUF_COLUMN,
  URL_COLUMN
};

struct _thumbnails thumbnails;

void
cheese_thumbnails_init() {
  thumbnails.store = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
}

void
cheese_thumbnails_finalize() {
}

void
cheese_thumbnails_append_photo(gchar *filename) {
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size(filename, THUMB_WIDTH, THUMB_HEIGHT, NULL);
  if (!pixbuf) {
    g_warning("could not load %s\n", filename);
    return;
  }
  g_print("appending %s to thumbnail row\n", filename);
  gtk_list_store_append(thumbnails.store, &thumbnails.iter);
  gtk_list_store_set(thumbnails.store, &thumbnails.iter, PIXBUF_COLUMN, pixbuf, URL_COLUMN, filename, -1);

  GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(thumbnails.store), &thumbnails.iter);
  gtk_icon_view_scroll_to_path(GTK_ICON_VIEW(thumbnails.iconview), path, TRUE, 1.0, 0.5);

  g_object_unref(pixbuf);
}

void
cheese_thumbnails_remove_photo(gchar *filename) {

  gchar *path;
  GtkTreeIter i;

  gtk_tree_model_get_iter_first(GTK_TREE_MODEL(thumbnails.store), &i);
  while (gtk_tree_model_iter_next(GTK_TREE_MODEL(thumbnails.store), &i)) {
    gtk_tree_model_get(GTK_TREE_MODEL(thumbnails.store), &i, URL_COLUMN, &path, -1);
    if (!g_ascii_strcasecmp(path, filename))
      break;
  }
  gtk_list_store_remove(thumbnails.store, &i);
  g_print("removing %s from thumbnail row\n", filename);
}

void
cheese_thumbnails_fill_thumbs()
{
  GDir *dir;
  gchar *path;
  const gchar *name;
  gboolean is_dir;
  GList *filelist = NULL;
  
  gtk_list_store_clear(thumbnails.store);

  dir = g_dir_open(cheese_fileutil_get_photo_path(), 0, NULL);
  if (!dir)
    return;

  while ((name = g_dir_read_name(dir))) {
    if (name[0] != '.') {
      if (!g_str_has_suffix (name, PHOTO_NAME_SUFFIX_DEFAULT))
        continue;
      path = g_build_filename(cheese_fileutil_get_photo_path(), name, NULL);

      is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);

      if (!is_dir)
        filelist = g_list_prepend(filelist, g_strdup(path));
      g_free(path);
    }
  }
  filelist = g_list_sort (filelist, (GCompareFunc)strcmp);
  g_list_foreach (filelist, (GFunc)cheese_thumbnails_append_photo, NULL);

  g_free(dir);
}
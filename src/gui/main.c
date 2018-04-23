/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/gui/main.c
 * \brief   Application driver
 *
 * Main application driver, sets up the application, handles opening any files
 * passed on the command line, and sets up logging.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 *  CbmFM - a file manager for CBM 8-bit emulation files
 *  Copyright (C) 2018  Bas Wassink <b.wassink@ziggo.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*
 */

#include <gtk/gtk.h>

#include "lib/base/errors.h"
#include "lib/base/log.h"


/** \brief  Handler for the 'activate' event of the GtkApplication
 *
 * \param[in]   app     application
 * \param[in]   data    (unused)
 */
static void on_activate(GApplication *app, gpointer data)
{
    GtkWidget *widget;

    widget = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(widget), "CBM-FM 0.1");
    gtk_widget_show(widget);
}


/** \brief  Handler for the 'shutdown' event of the GtkApplication
 *
 * \param[in]   app     application
 * \param[in]   data    (unused)
 */
static void on_shutdown(GApplication *app, gpointer data)
{
    cbmfm_log_close();
}


/** \brief  Handler for the 'open' event of the GtkApplication
 *
 * When the application is started, all command line arguments are assumed to
 * be files to open (due to the G_APPLICATION_HANDLES_OPEN flag). This function
 * is supposed to open these files using auto-detect (not implemented yet).
 *
 * \param[in]   app     application
 * \param[in]   files   array of files to open
 * \param[in]   n_files number of files in array
 * \param[in]   hint    hint for the \a app (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_open(
        GApplication *app,
        gpointer files,
        gint n_files,
        gchar *hint,
        gpointer data)
{
    int i;
    GFile **gfiles = (GFile **)files;

    cbmfm_log_info("got %d files to open:\n", n_files);
    for (i = 0; i < n_files; i++) {
        GFile *gf = gfiles[i];

        if (gf != NULL) {
            char *path = g_file_get_path(gf);
            if (path != NULL) {
                cbmfm_log_info("file %d: '%s'\n", i, path);
                g_free(path);
            }
        }
    }
}


/** \brief  Application driver
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    /* clear library error code */
    cbmfm_errno = 0;
    /* set logging to DEBUG and open log file */
    cbmfm_log_set_level(CBMFM_LOG_DEBUG);
    cbmfm_log_set_file("gui.log");

    app = gtk_application_new("nl.compyx.cbmfm", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_shutdown), NULL);
    g_signal_connect(app, "open", G_CALLBACK(on_open), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

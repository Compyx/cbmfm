/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/gui/main.c
 * \brief   Application driver
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


/** \brief  Application driver
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  0 on success
 */
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    /* clear library error code */
    cbmfm_errno = 0;
    /* set logging to DEBUG */
    cbmfm_log_set_level(CBMFM_LOG_DEBUG);

    app = gtk_application_new("nl.compyx.cbmfm", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

/*
 * Copyright (C) 2007 Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 * Copyright (C) 2013 Jens Georg <mail@jensge.org>
 *
 * Authors: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *          Jens Georg <mail@jensge.org>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libgupnp/gupnp.h>
#include "gui.h"
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include <gmodule.h>
#include <glib/gi18n.h>

static int upnp_port = 0;
static char **interfaces = NULL;

static GOptionEntry entries[] =
{
        { "port", 'p', 0, G_OPTION_ARG_INT, &upnp_port, N_("Network PORT to use for UPnP"), "PORT" },
        { "interface", 'i', 0, G_OPTION_ARG_STRING_ARRAY, &interfaces, N_("Network interfaces to use for UPnP communication"), "INTERFACE" },
        { NULL }
};

static GUPnPContextManager *context_manager;

static void
device_proxy_available_cb (GUPnPControlPoint *cp,
                           GUPnPDeviceProxy  *proxy)
{
        append_device (GUPNP_DEVICE_INFO (proxy));
}

static void
device_proxy_unavailable_cb (GUPnPControlPoint *cp,
                             GUPnPDeviceProxy  *proxy)
{
        remove_device (GUPNP_DEVICE_INFO (proxy));
}

static void
on_context_available (GUPnPContextManager *context_manager,
                      GUPnPContext        *context,
                      gpointer             user_data)
{
        GUPnPControlPoint *cp;

        /* We're interested in everything */
        cp = gupnp_control_point_new (context, "upnp:rootdevice");

        g_signal_connect (cp,
                          "device-proxy-available",
                          G_CALLBACK (device_proxy_available_cb),
                          NULL);
        g_signal_connect (cp,
                          "device-proxy-unavailable",
                          G_CALLBACK (device_proxy_unavailable_cb),
                          NULL);

        gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);

        /* Let context manager take care of the control point life cycle */
        gupnp_context_manager_manage_control_point (context_manager, cp);

        /* We don't need to keep our own reference to the control point */
        g_object_unref (cp);
}

static gboolean
init_upnp (void)
{
        GUPnPWhiteList *white_list;

#if !GLIB_CHECK_VERSION(2, 35, 0)
        g_type_init ();
#endif

        context_manager = gupnp_context_manager_new (NULL, upnp_port);
        g_assert (context_manager != NULL);

        if (interfaces != NULL) {
                white_list = gupnp_context_manager_get_white_list
                                            (context_manager);
                gupnp_white_list_add_entryv (white_list, interfaces);
                gupnp_white_list_set_enabled (white_list, TRUE);
        }

        g_signal_connect (context_manager,
                          "context-available",
                          G_CALLBACK (on_context_available),
                          NULL);

        return TRUE;
}

static void
deinit_upnp (void)
{
        g_object_unref (context_manager);
}

G_MODULE_EXPORT
void
application_exit (void)
{
        deinit_upnp ();
        deinit_ui ();

        gtk_main_quit ();
}

gint
main (gint   argc,
      gchar *argv[])
{
        GError *error = NULL;
        GOptionContext *context = NULL;

        setlocale (LC_ALL, "");
        bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
        bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
        textdomain (GETTEXT_PACKAGE);

        context = g_option_context_new (_("- UPnP AV control point"));
        g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
        g_option_context_add_group (context, gtk_get_option_group (TRUE));

        if (!g_option_context_parse (context, &argc, &argv, &error)) {
                g_print (_("Could not parse options: %s\n"), error->message);

                return -4;
        }

        if (!init_ui (&argc, &argv)) {
           return -2;
        }

        if (!init_upnp ()) {
           return -3;
        }

        gtk_main ();

        return 0;
}


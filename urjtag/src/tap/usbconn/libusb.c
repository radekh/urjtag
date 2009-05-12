/*
 * $Id$
 *
 * Link driver for accessing USB devices via libusb
 *
 * Copyright (C) 2008 K. Waschk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Kolja Waschk, 2008
 *
 */

#include <urjtag/sysdep.h>

#ifdef HAVE_LIBUSB

#include <fcntl.h>
#if __CYGWIN__ || __MINGW32__
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <usb.h>

#include <urjtag/error.h>
#include <urjtag/log.h>
#include <urjtag/usbconn.h>

typedef struct
{
    struct usb_device *dev;
    struct usb_dev_handle *handle;
} urj_usbconn_libusb_param_t;

urj_usbconn_driver_t urj_tap_usbconn_libusb_driver;

/* ---------------------------------------------------------------------- */

static int
libusb_match_desc (struct usb_device *dev, char *desc)
{
    int r = 0;
    char buf[256];
    usb_dev_handle *handle;

    if (desc == NULL)
        return 1;

    handle = usb_open (dev);
    if (handle == NULL)
    {
        urj_error_set (URJ_ERROR_USB, "usb_open() failed: %s", usb_strerror());
        errno = 0;
        return 0;
    }
    if (dev->descriptor.iManufacturer)
    {
        r = usb_get_string_simple (handle, dev->descriptor.iManufacturer, buf,
                                   sizeof (buf));
        if (r > 0)
        {
            if (strstr (buf, desc) == NULL)
                r = 0;
        }
    }
    if (r <= 0 && dev->descriptor.iProduct)
    {
        r = usb_get_string_simple (handle, dev->descriptor.iProduct, buf,
                                   sizeof (buf));
        if (r > 0)
        {
            if (strstr (buf, desc) == NULL)
                r = 0;
        }
    }
    if (r <= 0 && dev->descriptor.iSerialNumber)
    {
        r = usb_get_string_simple (handle, dev->descriptor.iSerialNumber, buf,
                                   sizeof (buf));
        if (r > 0)
        {
            if (strstr (buf, desc) == NULL)
                r = 0;
        }
    }
    usb_close (handle);
    return r > 0 ? 1 : 0;
}


/* ---------------------------------------------------------------------- */

static urj_usbconn_t *
usbconn_libusb_connect (const char **param, int paramc,
                        urj_usbconn_cable_t *template)
{
    struct usb_bus *bus;
    struct usb_device *found_dev = NULL;
    urj_usbconn_t *libusb_conn;
    urj_usbconn_libusb_param_t *libusb_params;

    usb_init ();
    if (usb_find_busses () < 0)
    {
        urj_error_set (URJ_ERROR_USB, "usb_find_busses() failed: %s",
                       usb_strerror());
        errno = 0;
        return NULL;
    }
    if (usb_find_devices () < 0)
    {
        urj_error_set (URJ_ERROR_USB, "usb_find_devices() failed: %s",
                       usb_strerror());
        errno = 0;
        return NULL;
    }

    for (bus = usb_get_busses (); bus && !found_dev; bus = bus->next)
    {
        struct usb_device *dev;

        for (dev = bus->devices; dev && !found_dev; dev = dev->next)
        {
            if (((template->vid < 0)
                 || (dev->descriptor.idVendor == template->vid))
                && ((template->pid < 0)
                    || (dev->descriptor.idProduct == template->pid)))
            {
                if (libusb_match_desc (dev, template->desc))
                {
                    found_dev = dev;
                }
            }
        }
    }

    if (!found_dev)
    {
        urj_error_set (URJ_ERROR_NOTFOUND, "no USB connections");
        return NULL;
    }

    libusb_conn = malloc (sizeof (urj_usbconn_t));
    libusb_params = malloc (sizeof (urj_usbconn_libusb_param_t));
    if (libusb_params == NULL || libusb_conn == NULL)
    {
        urj_error_set (URJ_ERROR_OUT_OF_MEMORY,
                       _("malloc(%zd)/malloc(%zd) fails"),
                       sizeof (urj_usbconn_t),
                       sizeof (urj_usbconn_libusb_param_t));
        if (libusb_params)
            free (libusb_params);
        if (libusb_conn)
            free (libusb_conn);
        return NULL;
    }

    libusb_params->dev = found_dev;
    libusb_params->handle = NULL;
    libusb_conn->params = libusb_params;
    libusb_conn->driver = &urj_tap_usbconn_libusb_driver;
    libusb_conn->cable = NULL;

    return libusb_conn;
}


/* ---------------------------------------------------------------------- */

static int
usbconn_libusb_open (urj_usbconn_t *conn)
{
    urj_usbconn_libusb_param_t *p = conn->params;

    p->handle = usb_open (p->dev);
    if (p->handle == NULL)
    {
        urj_error_set (URJ_ERROR_USB, "usb_open() failed: %s", usb_strerror());
        errno = 0;
    }
    else
    {
#if 1
        usb_set_configuration (p->handle,
                               p->dev->config[0].bConfigurationValue);
#endif
        if (usb_claim_interface (p->handle, 0) != 0)
        {
            usb_close (p->handle);
            urj_error_set (URJ_ERROR_USB, "usb_claim_interface failed: %s",
                           usb_strerror());
            errno = 0;
            p->handle = NULL;
        }
#if 1
        else
        {
            usb_set_altinterface (p->handle, 0);
        }
#endif
    }

    if (p->handle == NULL)
    {
        /* TODO: disconnect? */
        return URJ_STATUS_FAIL;
    }

    return URJ_STATUS_OK;
}

/* ---------------------------------------------------------------------- */

static int
usbconn_libusb_close (urj_usbconn_t *conn)
{
    urj_usbconn_libusb_param_t *p = conn->params;
    if (p->handle != NULL)
    {
        usb_release_interface (p->handle, 0);
        usb_close (p->handle);
    }
    p->handle = NULL;
    return URJ_STATUS_OK;
}

/* ---------------------------------------------------------------------- */

static void
usbconn_libusb_free (urj_usbconn_t *conn)
{
    free (conn->params);
    free (conn);
}

/* ---------------------------------------------------------------------- */

urj_usbconn_driver_t urj_tap_usbconn_libusb_driver = {
    "libusb",
    usbconn_libusb_connect,
    usbconn_libusb_free,
    usbconn_libusb_open,
    usbconn_libusb_close,
    NULL,
    NULL
};

#endif /* HAVE_LIBUSB */

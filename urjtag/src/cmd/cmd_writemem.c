/*
 * $Id$
 *
 * Written by Kent Palmkvist <kentp@isy.liu.se>, 2005
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
 */

#include <urjtag/sysdep.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <urjtag/error.h>
#include <urjtag/bus.h>

#include <urjtag/cmd.h>

#include "cmd.h"

static int
cmd_writemem_run (urj_chain_t *chain, char *params[])
{
    uint32_t adr;
    uint32_t len;
    FILE *f;

    if (urj_cmd_params (params) != 4)
    {
        urj_error_set (URJ_ERROR_SYNTAX,
                       "%s: #parameters should be %d, not %d",
                       params[0], 4, urj_cmd_params (params));
        return URJ_STATUS_FAIL;
    }

    if (!urj_bus)
    {
        urj_error_set (URJ_ERROR_ILLEGAL_STATE, _("Bus missing"));
        return URJ_STATUS_FAIL;
    }

    if (urj_cmd_get_number (params[1], &adr) != URJ_STATUS_OK
        || urj_cmd_get_number (params[2], &len) != URJ_STATUS_OK)
        return URJ_STATUS_FAIL;

    f = fopen (params[3], "r");
    if (!f)
    {
        urj_error_set (URJ_ERROR_IO,  _("Unable to open file `%s': %s"),
                       params[3], strerror(errno));
        errno = 0;
        return URJ_STATUS_FAIL;
    }
    urj_bus_writemem (urj_bus, f, adr, len);
    fclose (f);

    return URJ_STATUS_OK;
}

static void
cmd_writemem_help (void)
{
    printf (_("Usage: %s ADDR LEN FILENAME\n"
              "Write to device memory starting at ADDR the FILENAME file.\n"
              "\n"
              "ADDR       start address of the written memory area\n"
              "LEN        written memory length\n"
              "FILENAME   name of the input file\n"
              "\n"
              "ADDR and LEN could be in decimal or hexadecimal (prefixed with 0x) form.\n"
              "NOTE: This is NOT useful for FLASH programming!\n"),
            "writemem");
}

const urj_cmd_t urj_cmd_writemem = {
    "writemem",
    N_("write content of file to the memory"),
    cmd_writemem_help,
    cmd_writemem_run
};

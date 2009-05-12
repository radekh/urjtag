/*
 * $Id$
 *
 * Copyright (C) 2004 Marcel Telka
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
 * Written by Marcel Telka <marcel@telka.sk>, 2004.
 *
 */

#include <urjtag/sysdep.h>

#include <stdio.h>
#include <string.h>

#include <urjtag/error.h>
#include <urjtag/tap.h>

#include <urjtag/cmd.h>

#include "cmd.h"

static int
cmd_reset_run (urj_chain_t *chain, char *params[])
{
    if (urj_cmd_params (params) != 1)
    {
        urj_error_set (URJ_ERROR_SYNTAX,
                       "%s: #parameters should be %d, not %d",
                       params[0], 1, urj_cmd_params (params));
        return URJ_STATUS_FAIL;
    }

    if (urj_cmd_test_cable (chain) != URJ_STATUS_OK)
        return URJ_STATUS_FAIL;

    return urj_tap_reset_bypass (chain);
}

static void
cmd_reset_help (void)
{
    printf (_("Usage: %s\n" "Reset current JTAG chain.\n"), "reset");
}

const urj_cmd_t urj_cmd_reset = {
    "reset",
    N_("reset JTAG chain"),
    cmd_reset_help,
    cmd_reset_run
};

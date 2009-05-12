/*
 * $Id: urjtag.h 1525 2009-04-23 15:56:49Z rfhh $
 *
 * Global opaque types that had better be predefined
 * Copyright (C) 2009, Rutger Hofman
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
 * Author: Rutger Hofman, VU Amsterdam
 *
 */

#ifndef URJ_URJ_TYPES_H
#define URJ_URJ_TYPES_H

typedef struct urj_bus urj_bus_t;
typedef struct urj_bus_driver urj_bus_driver_t;
typedef struct urj_chain urj_chain_t;
typedef struct urj_cable urj_cable_t;
typedef struct urj_usbconn urj_usbconn_t;
typedef struct urj_parport urj_parport_t;
typedef struct urj_part urj_part_t;
typedef struct urj_parts urj_parts_t;
typedef struct urj_part_signal urj_part_signal_t;
typedef struct urj_part_salias urj_part_salias_t;
typedef struct urj_part_instruction urj_part_instruction_t;
typedef struct urj_data_register urj_data_register_t;
typedef struct urj_bsbit urj_bsbit_t;
typedef struct urj_tap_register urj_tap_register_t;

/**
 * Log levels
 */
typedef enum urj_log_level {
    URJ_LOG_LEVEL_ALL,          /**< every single bit as it is transmitted */
    URJ_LOG_LEVEL_COMM,         /**< low level communication details */
    URJ_LOG_LEVEL_DEBUG,        /**< more details of interest for developers */
    URJ_LOG_LEVEL_DETAIL,       /**< verbose output */
    URJ_LOG_LEVEL_NORMAL,       /**< just noteworthy info */
    URJ_LOG_LEVEL_WARNING,      /**< unmissable warnings */
    URJ_LOG_LEVEL_ERROR,        /**< only fatal errors */
    URJ_LOG_LEVEL_SILENT,       /**< suppress logging output */
} urj_log_level_t;

#define URJ_STATUS_OK             0
#define URJ_STATUS_FAIL           1
#define URJ_STATUS_MUST_QUIT    (-2)

#endif /* URJ_URJ_TYPES_H */

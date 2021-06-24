/*
 * File      : fal_def.h
 * This file is part of FAL (Flash Abstraction Layer) package
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
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
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_DEF_H_
#define _FAL_DEF_H_

#include <stdint.h>
#include <stdio.h>

#define FAL_SW_VERSION                 "0.3.0"

#ifndef FAL_MALLOC
#define FAL_MALLOC                     malloc
#endif

#ifndef FAL_CALLOC
#define FAL_CALLOC                     calloc
#endif

#ifndef FAL_REALLOC
#define FAL_REALLOC                    realloc
#endif

#ifndef FAL_FREE
#define FAL_FREE                       free
#endif

#ifndef FAL_DEBUG
#define FAL_DEBUG                      0
#endif



/* FAL flash and partition device name max length */
#ifndef FAL_DEV_NAME_MAX
#define FAL_DEV_NAME_MAX 24
#endif

struct fal_flash_dev
{
    char name[FAL_DEV_NAME_MAX];

    /* flash device start address and len  */
    uint32_t addr;
    size_t len;
    /* the block size in the flash for erase minimum granularity */
    size_t blk_size;

    struct
    {
        int (*init)(void);
        int (*read)(long offset, uint8_t *buf, size_t size);
        int (*write)(long offset, const uint8_t *buf, size_t size);
        int (*erase)(long offset, size_t size);
    } ops;
};
typedef struct fal_flash_dev *fal_flash_dev_t;

/**
 * FAL partition
 */
struct fal_partition
{
    uint32_t magic_word;

    /* partition name */
    char name[FAL_DEV_NAME_MAX];
    /* flash device name for partition */
    char flash_name[FAL_DEV_NAME_MAX];

    /* partition offset address on flash device */
    long offset;
    size_t len;

    uint32_t reserved;
};
typedef struct fal_partition *fal_partition_t;

#endif /* _FAL_DEF_H_ */

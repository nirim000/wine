/*
 * Server-side device management
 *
 * Copyright (C) 1999 Alexandre Julliard
 */

/*
 * FIXME:
 * all this stuff is a simple hack to avoid breaking
 * client-side device support.
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winerror.h"
#include "winbase.h"

#include "handle.h"
#include "thread.h"
#include "request.h"

struct device
{
    struct object  obj;             /* object header */
    int            id;              /* client identifier */
};

static void device_dump( struct object *obj, int verbose );
static int device_get_info( struct object *obj, struct get_file_info_reply *reply );

static const struct object_ops device_ops =
{
    sizeof(struct device),
    device_dump,
    no_add_queue,
    NULL,  /* should never get called */
    NULL,  /* should never get called */
    NULL,  /* should never get called */
    no_read_fd,
    no_write_fd,
    no_flush,
    device_get_info,
    no_destroy
};

static struct device *create_device( int id )
{
    struct device *dev;
    if ((dev = alloc_object( &device_ops )))
    {
        dev->id = id;
    }
    return dev;
}

static void device_dump( struct object *obj, int verbose )
{
    struct device *dev = (struct device *)obj;
    assert( obj->ops == &device_ops );
    fprintf( stderr, "Device id=%08x\n", dev->id );
}

static int device_get_info( struct object *obj, struct get_file_info_reply *reply )
{
    struct device *dev = (struct device *)obj;
    assert( obj->ops == &device_ops );
    memset( reply, 0, sizeof(*reply) );
    reply->type = FILE_TYPE_UNKNOWN;
    reply->attr = dev->id;  /* hack! */
    return 1;
}

/* create a device */
DECL_HANDLER(create_device)
{
    struct device *dev;
    struct create_device_reply *reply = push_reply_data( current, sizeof(*reply) );

    if ((dev = create_device( req->id )))
    {
        reply->handle = alloc_handle( current->process, dev, req->access, req->inherit );
        release_object( dev );
    }
    else reply->handle = -1;
}

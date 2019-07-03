#ifndef __CAMERA_H__
#define __CAMERA_H__

struct camera_args
{
        int camera_node_id;
        int priv_w;
        int priv_h;
        char priv_format[5];
};



int test_camera(struct camera_args *args);

#endif

/*
 * main.cpp
 *
 *  Created on: Nov 18, 2022
 *      Author: wus1
 */

#include <gst/video/gstvideometa.h>
#include "GstMyAppMeta.h"
#include "HostYuvFrm.h"

using namespace std;
using namespace app;

int test_gst_meta();
int main()
{
    test_gst_meta();
    return 0;
}


int test_gst_meta()
{
    GstMyAppMeta *meta = NULL;
    GType myType;
    const GstMetaInfo *myInfo;

    GstVideoCropMeta *meta2 = NULL;
    GType myType2;
    const GstMetaInfo *myInfo2;

    //get sample image from uData
    app::HostYuvFrmPtr curFrm(new HostYuvFrm(1920, 1080));
    curFrm->setToRand();
    curFrm->wrtFrmNumOnImg();
    curFrm->dump("./", "tmp");


    gst_init(NULL, NULL);

    //----------------------------------
    //get Gtype and GstMetaInfo
    //----------------------------------
    myType = gst_my_app_meta_api_get_type();
    myInfo = gst_my_app_meta_get_info();
    g_print("myType=%s, myInfo(api=%s,type=%s,size=%d)\n",
           g_type_name(myType), g_type_name(myInfo->api), g_type_name(myInfo->type), myInfo->size);

    myType2 = gst_video_crop_meta_api_get_type();
    myInfo2 = gst_video_crop_meta_get_info();
    g_print("myType2=%s, myInfo2(api=%s,type=%s,size=%d)\n",
           g_type_name(myType2), g_type_name(myInfo2->api), g_type_name(myInfo2->type), myInfo2->size);

    //----------------------------------
    //create a buffer
    //----------------------------------
    GstBuffer *buffer = gst_buffer_new_allocate(NULL, curFrm->sz_, NULL);
    gst_buffer_fill(buffer, 0, curFrm->buf_, curFrm->sz_);


    //----------------------------------
    //attach GstMyAppMeta
    //----------------------------------
    gint sz = 32;
    gchar *klv = (gchar *)g_malloc(sz);
    for (int i = 0; i < sz; i++) klv[i]='x';

    std::string s = std::to_string(12345);
    std::cout << s << std::endl;
    for (int i = 0; i < s.size(); i++) klv[i] = s[i];

    //either way works
#if   1
    meta = gst_buffer_add_my_app_meta_full(buffer,sz, klv);
    g_print("A---meta=%0x\n", meta);
#else  
    meta = gst_buffer_add_my_app_meta(buffer);
    g_print("A---meta=%0x\n", meta);
    meta->bufSize = sz;
    meta->buf = g_strdup(klv);
#endif    
    g_free(klv);

    //----------------------------------
    //attach GstVideoCropMeta 
    //----------------------------------
    meta2 =  gst_buffer_add_video_crop_meta(buffer);
    g_print("A---meta2=%0x\n", meta2);
    meta2->x = 8;
    meta2->y = 8;
    meta2->width = 120;
    meta2->height = 80;

    //
    //now <buffer> can be passed between elements in pipeline
    // ...
    // ...
    //

    //----------------------------------
    //in the next we can get meta from buffer
    //----------------------------------
    meta = gst_buffer_get_my_app_meta(buffer);
    g_print("B---meta=%0x\n", meta2);
    if (meta)
    {
        g_print("meta->sz=%d:[", meta->bufSize);
        for (int i = 0; i < meta->bufSize; ++i)
        {
            printf("%c,", meta->buf[i]);
        }
        printf("]\n");
    }

    meta2 = gst_buffer_get_video_crop_meta(buffer);
    g_print("B---meta2=%0x\n", meta2);
    if (meta2)
    {
        g_print("meta2:(x=%d, y=%d, w=%d, h=%d)\n", meta2->x, meta2->y, meta2->width, meta2->height);
    }
    gst_buffer_unref(buffer);

    g_print("well done!\n");
    return 0;
}

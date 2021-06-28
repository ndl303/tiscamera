/*
 * Copyright 2014 The Imaging Source Europe GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TCAM_GSTTCAMSRC_H
#define TCAM_GSTTCAMSRC_H

#include "../../tcam.h"

#include <girepository.h>
#include <gst/gst.h>
#include <string>
#include <vector>

using namespace tcam;

#ifdef __cplusplus
extern "C"
{
#endif


    G_BEGIN_DECLS


#define GST_TYPE_TCAM_SRC          (gst_tcam_src_get_type())
#define GST_TCAM_SRC(obj)          (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_TCAM_SRC, GstTcamSrc))
#define GST_TCAM_SRC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_TCAM_SRC, GstTcamSrc))
#define GST_IS_TCAM_SRC(obj)       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_TCAM_SRC))
#define GST_IS_TCAM_SRC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_TCAM_SRC))

    typedef struct _GstTcamSrc GstTcamSrc;
    typedef struct _GstTcamSrcClass GstTcamSrcClass;

    struct _GstTcamSrc
    {
        GstBin parent;

        GstElement* active_source;

        // convenience container
        // for all source elements
        // tcamsrc can address
        GSList* source_list;

        GstElement* main_src;
        GstElement* pimipi_src;
        GstElement* tegra_src;

        std::string device_serial;
        TCAM_DEVICE_TYPE device_type;

        gint cam_buffers;
        gboolean drop_incomplete_frames;
        gboolean do_timestamp;
        gint num_buffers;

        GstPad* pad;
    };


    struct _GstTcamSrcClass
    {
        GstBinClass parent_class;
    };

    GType gst_tcam_src_get_type(void);

    G_END_DECLS


#ifdef __cplusplus
}
#endif


#endif /* TCAM_GSTTCAMSRC_H */

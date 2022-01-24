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

#include "CaptureDeviceImpl.h"
#include "DeviceIndex.h"

#include "logging.h"
#include "utils.h"

#include <exception>

using namespace tcam;


struct bad_device : std::exception
{
    const char* what () const noexcept { return "Device did not comply with commands.";}
};


CaptureDeviceImpl::CaptureDeviceImpl ()
    : pipeline(nullptr), property_handler(nullptr), device(nullptr), index_()
{}


CaptureDeviceImpl::CaptureDeviceImpl (const DeviceInfo& _device)
    : pipeline(nullptr), property_handler(nullptr), device(nullptr), index_()
{
    if (!open_device(_device))
    {
        tcam_log(TCAM_LOG_ERROR, "Unable to open device");
        throw bad_device();
    }

}


CaptureDeviceImpl::~CaptureDeviceImpl ()
{
    close_device();
}


bool CaptureDeviceImpl::open_device (const DeviceInfo& device_desc)
{
    if (is_device_open())
    {
        bool ret = close_device();
        if (ret == false)
        {
            tcam_log(TCAM_LOG_ERROR, "Unable to close previous device.");
            // setError(Error("A device is already open", EPERM));
            return false;
        }
    }

    open_device_info = device_desc;

    device = openDeviceInterface(open_device_info);

    if (device == nullptr)
    {
        return false;
    }

    pipeline = std::make_shared<PipelineManager>();
    pipeline->setSource(device);

    property_handler = std::make_shared<PropertyHandler>();

    property_handler->set_properties(device->getProperties(), pipeline->getFilterProperties());

    const auto serial = open_device_info.get_serial();
    index_.register_device_lost(deviceindex_lost_cb,
                                this,
                                serial);
    return true;
}


bool CaptureDeviceImpl::is_device_open () const
{
    if (device != nullptr)
    {
        return true;
    }

    return false;
}


DeviceInfo CaptureDeviceImpl::get_device () const
{
    return this->open_device_info;
}


bool CaptureDeviceImpl::register_device_lost_callback (tcam_device_lost_callback callback, void* user_data)
{
    if (!is_device_open())
    {
        return false;
    }

    struct device_lost_cb_data data;
    data.callback = callback;
    data.user_data = user_data;

    device_lost_callback_data_.push_back(data);

    return device->register_device_lost_callback(callback, user_data);
}


void CaptureDeviceImpl::deviceindex_lost_cb (const DeviceInfo& info, void* user_data)
{
    auto self = (CaptureDeviceImpl*) user_data;

    const auto i = info.get_info();

    tcam_info("Received lost from index");

    for (const auto& data : self->device_lost_callback_data_)
    {
        (*data.callback)(&i, data.user_data);
    }
}


bool CaptureDeviceImpl::close_device ()
{
    if (!is_device_open())
    {
        return true;
    }

    std::string name = open_device_info.get_name();

    pipeline->destroyPipeline();

    pipeline = nullptr;

    open_device_info = DeviceInfo ();
    device.reset();
    property_handler = nullptr;

    tcam_log(TCAM_LOG_INFO, "Closed device %s.", name.c_str());

    return true;
}


std::vector<Property*> CaptureDeviceImpl::get_available_properties ()
{
    if (!is_device_open())
    {
        return std::vector<Property*>();
    }

    std::vector<Property*> props;

    for ( const auto& p : property_handler->get_properties())
    {
        props.push_back(&*p);
    }

    return props;
}


std::vector<VideoFormatDescription> CaptureDeviceImpl::get_available_video_formats () const
{
    if (!is_device_open())
    {
        return std::vector<VideoFormatDescription>();
    }

    return pipeline->getAvailableVideoFormats();
}


bool CaptureDeviceImpl::set_video_format (const VideoFormat& new_format)
{
    if (!is_device_open())
    {
        return false;
    }

    pipeline->setVideoFormat(new_format);

    return this->device->set_video_format(new_format);
}


VideoFormat CaptureDeviceImpl::get_active_video_format () const
{
    if(!is_device_open())
    {
        return VideoFormat();
    }

    return device->get_active_video_format();
}


bool CaptureDeviceImpl::start_stream (std::shared_ptr<SinkInterface> sink)
{
    if (!is_device_open())
    {
        tcam_log(TCAM_LOG_ERROR, "Device is not open");
        return false;
    }

    if (!pipeline->setSink(sink))
    {
        return false;
    }

    return pipeline->set_status(TCAM_PIPELINE_PLAYING);
}


bool CaptureDeviceImpl::stop_stream ()
{
    if (!is_device_open())
    {
        return false;
    }

    return pipeline->set_status(TCAM_PIPELINE_STOPPED);
}

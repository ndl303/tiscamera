/*
 * Copyright 2021 The Imaging Source Europe GmbH
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

#pragma once

#include "../../lib/dutils_image/src/dutils_img_filter/transform/transform_base.h" // img_filter::transform_function_type

#include <dutils_img/dutils_img.h>
#include <dutils_img_pipe/auto_alg_pass.h>
#include <functional>
#include <gst-helper/gst_signal_helper.h>
#include <gst-helper/helper_functions.h>
#include <tcamprop_system/tcamprop_provider_funcbased.h>

struct GstTCamConvert;

namespace tcamconvert
{
class tcamconvert_context_base : public tcamprop_system::property_list_interface
{
    using wb_func = void (*)(const img::img_descriptor& dst,
                             const img_filter::whitebalance_params& params);

public:
    img::img_type src_type_;
    img::img_type dst_type_;

    void on_input_pad_linked();
    void on_input_pad_unlinked();
public:
    tcamconvert_context_base( GstTCamConvert* self );

    bool setup(img::img_type src_type, img::img_type dst_type);

    // Inherited via property_interface
    virtual std::vector<std::string_view> get_property_list() final;
    virtual tcamprop_system::property_interface* find_property(std::string_view name) final;

    void transform(const img::img_descriptor& src, const img::img_descriptor& dst);
    void filter( const img::img_descriptor& src );

    bool try_connect_to_source(bool force);
private:
    img_filter::transform_function_type transfrom_binary_mono_func_ = nullptr;

    std::function<void(const img::img_descriptor& dst,
                       const img::img_descriptor& src,
                       img_filter::filter_params& params)>
        transform_binary_color_func_;

    wb_func transform_unary_func_ = nullptr;

private:
    enum class color_mode
    {
        mono,
        bayer,
    };

    color_mode get_color_mode() const noexcept;
    void update_balancewhite_values_from_source();

    tcamprop_system::property_list_funcbased prop_list_;

    bool apply_wb_ = true;

    auto_alg::wb_channel_factors wb_channels_ = {};
private:
    void    init_from_source();
    bool    init_from_source_done_ = false;

    void    on_device_opened();
    void    on_device_closed();

    gst_helper::gst_device_connect_signal signal_handle_device_open_;
    gst_helper::gst_device_connect_signal signal_handle_device_close_;

    gst_helper::gst_ptr<GstElement> src_element_ptr_;

    GstTCamConvert* self_reference_ = nullptr;
};
} // namespace tcamconvert

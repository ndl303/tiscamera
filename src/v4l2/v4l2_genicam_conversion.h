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

#include "../error.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <functional>

namespace tcam::v4l2
{

    struct converter_scale
{
    std::function<double(double) > to_device;
    std::function<double(double) > from_device;
};


enum class MappingType
{
    None,
    IntToEnum,
    Scale,
};

converter_scale find_scale(uint32_t v4l2_id);

outcome::result<std::map<int, std::string>> find_menu_entries(uint32_t v4l2_id);

} // namespace tcam::v4l2
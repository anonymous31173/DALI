// Copyright (c) 2017-2018, NVIDIA CORPORATION. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dali/pipeline/operators/color/color_twist.h"
#include <vector>
#include <string>

namespace dali {

DALI_SCHEMA(ColorTransformBase)
    .DocStr(R"code(Base Schema for color transformations operators.)code")
    .AddOptionalArg("image_type",
        R"code(`dali.types.DALIImageType`
        The color space of input and output image)code", DALI_RGB);

DALI_SCHEMA(Brightness)
    .DocStr(R"code(Changes the brightness of an image)code")
    .NumInput(1)
    .NumOutput(1)
    .AddOptionalArg("brightness",
        R"code(`float` or `float tensor`
        Brightness change factor.
        Values >= 0 are accepted. For example:
          `0` - black image,
          `1` - no change
          `2` - increase brightness twice
          )code", 1.f)
    .AddParent("ColorTransformBase");

DALI_SCHEMA(Contrast)
    .DocStr(R"code(Changes the color contrast of the image.)code")
    .NumInput(1)
    .NumOutput(1)
    .AddOptionalArg("contrast",
        R"code(`float` or `float tensor`
        Contrast change factor.
        Values >= 0 are accepted. For example:
          `0` - gray image,
          `1` - no change
          `2` - increase contrast twice
          )code", 1.f)
    .AddParent("ColorTransformBase");

DALI_SCHEMA(Hue)
    .DocStr(R"code(Changes the hue level of the image.)code")
    .NumInput(1)
    .NumOutput(1)
    .AddOptionalArg("hue",
        R"code(`float` or `float tensor`
        Hue change in angles.
        )code", 0.f)
    .AddParent("ColorTransformBase");

DALI_SCHEMA(Saturation)
    .DocStr(R"code(Changes saturation level of the image.)code")
    .NumInput(1)
    .NumOutput(1)
    .AddOptionalArg("saturation",
        R"code(`float` or `float tensor`
        Saturation change factor.
        Values >= 0 are supported. For example:
          `0` - completely desaturated image
          `1` - no change to image's saturation
          )code", 1.f)
    .AddParent("ColorTransformBase");

}  // namespace dali

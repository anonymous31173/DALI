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


#include "dali/pipeline/operators/displacement/displacement_filter.h"

namespace dali {

DALI_SCHEMA(DisplacementFilter)
  .DocStr("Base schema for displacement operators")
  .AddOptionalArg("mask",
      R"code(`int` or `int tensor`
      Whether to apply this augmentation to the input image.
        0 - do not apply this transformation
        1 - apply this transformation
        )code", 1)
  .AddOptionalArg("interp_type",
      R"code(`dali.types.DALIInterpType`
      Type of interpolation used)code",
      DALI_INTERP_NN)
  .AddOptionalArg("fill_value",
      R"code(`float`
      Color value used for padding pixels.)code",
      0.f);

}  // namespace dali

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

#ifndef DALI_UTIL_OCV_H_
#define DALI_UTIL_OCV_H_

#include <opencv2/opencv.hpp>

#include "dali/common.h"

namespace dali {

int OCVInterpForDALIInterp(DALIInterpType type, int *ocv_type);

template <typename T>
cv::Mat CreateMatFromPtr(int H,
                         int W,
                         int type,
                         const T * ptr,
                         size_t step = cv::Mat::AUTO_STEP) {
  // Note: OpenCV can't take a const pointer to wrap even when the cv::Mat is const. This
  // is kinda icky to const_cast away the const-ness, but there isn't another way
  // (that I know of) without making the input argument non-const.
  return cv::Mat(H, W, type, const_cast<T*>(ptr), step);
}

}  // namespace dali

#endif  // DALI_UTIL_OCV_H_

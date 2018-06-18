// Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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

#ifndef DALI_PIPELINE_OPERATORS_READER_LOADER_LOADER_H_
#define DALI_PIPELINE_OPERATORS_READER_LOADER_LOADER_H_

#include <list>
#include <map>
#include <mutex>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "dali/common.h"
#include "dali/error_handling.h"
#include "dali/pipeline/operators/op_spec.h"
#include "dali/pipeline/data/tensor.h"

namespace dali {

template <class Backend>
class Loader {
 public:
  explicit Loader(const OpSpec& options)
    : shuffle_(options.GetArgument<bool>("random_shuffle")),
      initial_buffer_fill_(shuffle_ ? options.GetArgument<int>("initial_fill") : 1),
      initial_empty_size_(2 * options.GetArgument<int>("batch_size")),
      tensor_init_bytes_(options.GetArgument<int>("tensor_init_bytes")),
      seed_(options.GetArgument<Index>("seed")),
      shard_id_(options.GetArgument<int>("shard_id")),
      num_shards_(options.GetArgument<int>("num_shards")) {
    DALI_ENFORCE(initial_empty_size_ > 0, "Batch size needs to be greater than 0");
    // initialize a random distribution -- this will be
    // used to pick from our sample buffer
    dis = std::uniform_int_distribution<>(0, initial_buffer_fill_);
    std::seed_seq seq({seed_});
    e_ = std::default_random_engine(seq);
  }

  virtual ~Loader() {
    // delete all the temporary tensors
    while (!sample_buffer_.empty()) {
      Tensor<Backend> * t = sample_buffer_.back();
      delete t;
      sample_buffer_.pop_back();
    }
    while (!empty_tensors_.empty()) {
      Tensor<Backend> * t = empty_tensors_.back();
      delete t;
      empty_tensors_.pop_back();
    }
  }

  // Get a random read sample
  Tensor<Backend>* ReadOne() {
    TimeRange tr("[Loader] ReadOne", TimeRange::kGreen1);
    // perform an iniital buffer fill if it hasn't already happened
    if (!initial_buffer_filled_) {
      TimeRange tr("[Loader] Filling initial buffer", TimeRange::kBlue1);
      // Read an initial number of samples to fill our
      // sample buffer
      for (int i = 0; i < initial_buffer_fill_; ++i) {
        Tensor<Backend>* tensor = new Tensor<CPUBackend>();
        tensor->set_pinned(false);
        // Initialize tensors to a set size to limit expensive reallocations
        tensor->Resize({tensor_init_bytes_});
        tensor->template mutable_data<uint8_t>();

        ReadSample(tensor);
        sample_buffer_.push_back(tensor);
      }

      TimeRange tr2("[Loader] Filling empty list", TimeRange::kOrange);
      // need some entries in the empty_tensors_ list
      for (int i = 0; i < initial_empty_size_; ++i) {
        Tensor<Backend>* tensor = new Tensor<CPUBackend>();
        tensor->set_pinned(false);
        // Force allocation for empties
        tensor->Resize({tensor_init_bytes_});
        tensor->template mutable_data<uint8_t>();

        empty_tensors_.push_back(tensor);
      }

      initial_buffer_filled_ = true;
    }
    // choose the random index
    int idx = shuffle_ ? dis(e_) % sample_buffer_.size() : 0;
    Tensor<Backend>* elem = sample_buffer_[idx];

    // swap end and idx, return the tensor to empties
    std::swap(sample_buffer_[idx], sample_buffer_[sample_buffer_.size()-1]);
    // remove last element
    sample_buffer_.pop_back();

    // now grab an empty tensor, fill it and add to filled buffers
    // empty_tensors_ needs to be thread-safe w.r.t. ReturnTensor()
    // being called by multiple consumer threads
    Tensor<Backend>* t;
    {
      std::lock_guard<std::mutex> lock(return_mutex_);
      DALI_ENFORCE(empty_tensors_.size() > 0, "No empty tensors - did you forget to return them?");
      t = empty_tensors_.back();
      empty_tensors_.pop_back();
    }
    ReadSample(t);
    sample_buffer_.push_back(t);

    return elem;
  }

  // return a tensor to the empty pile
  // called by multiple consumer threads
  void ReturnTensor(Tensor<Backend>* tensor) {
    std::lock_guard<std::mutex> lock(return_mutex_);
    empty_tensors_.push_back(tensor);
  }

  // Read an actual sample from the FileStore,
  // used to populate the sample buffer for "shuffled"
  // reads.
  virtual void ReadSample(Tensor<Backend>* tensor) = 0;

  // Give the size of the data accessed through the Loader
  virtual Index Size() = 0;

 protected:
  std::vector<Tensor<Backend>*> sample_buffer_;

  std::list<Tensor<Backend>*> empty_tensors_;

  // number of samples to initialize buffer with
  // ~1 minibatch seems reasonable
  bool shuffle_;
  const int initial_buffer_fill_;
  const int initial_empty_size_;
  const int tensor_init_bytes_;
  bool initial_buffer_filled_ = false;

  // rng
  std::default_random_engine e_;
  std::uniform_int_distribution<> dis;
  Index seed_;

  // control return of tensors
  std::mutex return_mutex_;

  // sharding
  const int shard_id_;
  const int num_shards_;
};

};  // namespace dali

#endif  // DALI_PIPELINE_OPERATORS_READER_LOADER_LOADER_H_

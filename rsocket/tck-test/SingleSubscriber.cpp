// Copyright 2004-present Facebook. All Rights Reserved.

#include "rsocket/tck-test/SingleSubscriber.h"

#include <thread>

#include <folly/Format.h>

using namespace folly;

namespace rsocket {
namespace tck {

void SingleSubscriber::request(int n) {
  LOG(INFO) << "... requesting " << n << ". No request() for Single.";
}

void SingleSubscriber::cancel() {
  LOG(INFO) << "... canceling ";
  canceled_ = true;
  if (auto subscription = std::move(subscription_)) {
    subscription->cancel();
  }
}

void SingleSubscriber::onSubscribe(
    std::shared_ptr<yarpl::single::SingleSubscription> subscription) noexcept {
  VLOG(4) << "OnSubscribe in SingleSubscriber";
  subscription_ = subscription;
}

void SingleSubscriber::onSuccess(Payload element) noexcept {
  LOG(INFO) << "... received onSuccess from Publisher: " << element;
  {
    const std::unique_lock<std::mutex> lock(mutex_);
    const std::string data =
        element.data ? element.data->moveToFbString().toStdString() : "";
    const std::string metadata = element.metadata
        ? element.metadata->moveToFbString().toStdString()
        : "";
    values_.push_back(std::make_pair(data, metadata));
    ++valuesCount_;
  }
  valuesCV_.notify_one();
  {
    const std::unique_lock<std::mutex> lock(mutex_);
    completed_ = true;
  }
  terminatedCV_.notify_one();
}

void SingleSubscriber::onError(folly::exception_wrapper ex) noexcept {
  LOG(INFO) << "... received onError from Publisher";
  {
    const std::unique_lock<std::mutex> lock(mutex_);
    errors_.push_back(std::move(ex));
    errored_ = true;
  }
  terminatedCV_.notify_one();
}

} // namespace tck
} // namespace rsocket

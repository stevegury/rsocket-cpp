// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <folly/io/async/EventBase.h>

#include "rsocket/framing/FrameTransport.h"

namespace rsocket {

// This class is a wrapper around FrameTransport which ensures all methods of
// FrameTransport get executed in a particular EventBase.
//
// This is currently used in the server where the resumed Transport of the
// client is on a different EventBase compared to the EventBase on which the
// original RSocketStateMachine was constructed for the client.  Here the
// RSocketStateMachine uses this class to schedule events of the Transport in
// the new EventBase.
class ScheduledFrameTransport : public FrameTransport {
 public:
  ScheduledFrameTransport(
      std::shared_ptr<FrameTransport> frameTransport,
      folly::EventBase* transportEvb,
      folly::EventBase* stateMachineEvb)
      : transportEvb_(transportEvb),
        stateMachineEvb_(stateMachineEvb),
        frameTransport_(std::move(frameTransport)) {}

  ~ScheduledFrameTransport();

  void setFrameProcessor(std::shared_ptr<FrameProcessor>) override;
  void outputFrameOrDrop(std::unique_ptr<folly::IOBuf>) override;
  void close() override;
  bool isConnectionFramed() const override;

 private:
  DuplexConnection* getConnection() override {
    DLOG(FATAL)
        << "ScheduledFrameTransport doesn't support getConnection method, "
           "because it can create safe usage issues when EventBase of the "
           "transport and the RSocketClient is not the same.";
    return nullptr;
  }

 private:
  folly::EventBase* const transportEvb_;
  folly::EventBase* const stateMachineEvb_;
  std::shared_ptr<FrameTransport> frameTransport_;
};

} // namespace rsocket

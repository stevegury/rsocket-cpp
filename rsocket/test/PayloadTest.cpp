// Copyright 2004-present Facebook. All Rights Reserved.

#include <folly/io/Cursor.h>
#include <folly/io/IOBuf.h>

#include <gtest/gtest.h>

#include "rsocket/Payload.h"
#include "rsocket/framing/Frame.h"
#include "rsocket/framing/FrameSerializer_v1_0.h"

using namespace ::testing;
using namespace ::rsocket;

TEST(PayloadTest, EmptyMetadata) {
  Payload p("some error message");
  EXPECT_NE(p.data, nullptr);
  EXPECT_EQ(p.metadata, nullptr);
}

TEST(PayloadTest, Clear) {
  Payload p("hello");
  ASSERT_TRUE(p);

  p.clear();
  ASSERT_FALSE(p);
}

TEST(PayloadTest, Clone) {
  Payload orig("data", "metadata");

  // Clone copies both
  Payload clone = orig.clone();
  EXPECT_NE(clone.data, nullptr);
  EXPECT_NE(clone.metadata, nullptr);

  EXPECT_EQ(clone.data->moveToFbString(), "data");
  EXPECT_EQ(clone.metadata->moveToFbString(), "metadata");

  // Clone now empty, orig unchanged
  clone.clear();
  EXPECT_EQ(clone.data, nullptr);
  EXPECT_EQ(clone.metadata, nullptr);
  EXPECT_NE(orig.data, nullptr);
  EXPECT_NE(orig.metadata, nullptr);

  // no data
  Payload nodata = orig.clone();
  nodata.data.reset();
  clone = nodata.clone();
  EXPECT_EQ(clone.data, nullptr);
  EXPECT_NE(clone.metadata, nullptr);
  // orig unchanged
  EXPECT_NE(orig.data, nullptr);
  EXPECT_NE(orig.metadata, nullptr);

  // no metadata
  Payload nometa("data", "");
  // This constructor doesn't set metadata if it is empty
  clone = nometa.clone();
  EXPECT_NE(clone.data, nullptr);
  EXPECT_EQ(clone.metadata, nullptr);

  // neither
  std::unique_ptr<folly::IOBuf> data_;
  std::unique_ptr<folly::IOBuf> metadata_;
  Payload none(std::move(data_), std::move(metadata_));
  clone = none.clone();
  EXPECT_EQ(clone.data, nullptr);
  EXPECT_EQ(clone.metadata, nullptr);
}

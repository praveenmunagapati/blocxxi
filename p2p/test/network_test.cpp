//        Copyright The Authors 2018.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <p2p/kademlia/channel.h>
#include <p2p/kademlia/message_serializer.h>
#include <p2p/kademlia/network.h>

using testing::NiceMock;
using testing::AtLeast;
using testing::Return;
using testing::_;

namespace blocxxi {
namespace p2p {
namespace kademlia {

class MockChannel {
 public:
  using RecvCallBackType = std::function<void(
      std::error_code const &, IpEndpoint const &, BufferReader const &)>;

  using SendCallbackType = std::function<void(std::error_code const &)>;

  MOCK_METHOD1(AsyncReceive, void(RecvCallBackType));
  MOCK_METHOD3(AsyncSend, void(std::vector<uint8_t> const &, IpEndpoint const &,
                               SendCallbackType));
  MOCK_CONST_METHOD0(LocalEndpoint, IpEndpoint());
};

TEST(NetworkTest, Simple) {
  boost::asio::io_context context;
  auto v4 = AsyncUdpChannel::ipv4(context, "127.0.0.1", "3000");
  auto v6 = AsyncUdpChannel::ipv6(context, "::1", "3000");
  auto serializer =
      std::make_unique<MessageSerializer>(Node::IdType::RandomHash());

  Network<AsyncUdpChannel, MessageSerializer> network(
      context, std::move(serializer), std::move(v4), std::move(v6));
  context.run();
}

TEST(NetworkTest, CallingStartWithNoReceiveHandlerIsFatal) {
  boost::asio::io_context context;
  auto v4 = std::make_unique<NiceMock<MockChannel>>();
  auto v6 = std::make_unique<NiceMock<MockChannel>>();
  auto serializer =
      std::make_unique<MessageSerializer>(Node::IdType::RandomHash());
  Network<MockChannel, MessageSerializer> network(
      context, std::move(serializer), std::move(v4), std::move(v6));
  /*
   Assertion failed. Please file a bugreport at ...

   file:   '/Users/abdessattar/Projects/blocxxi/p2p ...
   line: 118
   function: void blocxxi::p2p::kademlia::Network<blocxxi::p2p::...
  */
  auto regex =
      "Assertion failed.*\n"
      ".*\n"
      "file:.*\n"
      "line:.*\n"
      "function:.*\n";
  ASSERT_DEATH(network.Start(), regex);
}

TEST(NetworkTest, StartSchedulesRceiveOnAllchannels) {
  boost::asio::io_context context;
  auto v4 = std::make_unique<NiceMock<MockChannel>>();
  auto v6 = std::make_unique<NiceMock<MockChannel>>();

  ON_CALL(*v4, LocalEndpoint())
      .WillByDefault(Return(IpEndpoint{"127.0.0.1", 4242}));
  ON_CALL(*v6, LocalEndpoint()).WillByDefault(Return(IpEndpoint{"::1", 4242}));

  EXPECT_CALL(*v4, AsyncReceive(_)).Times(AtLeast(1));
  EXPECT_CALL(*v6, AsyncReceive(_)).Times(AtLeast(1));

  auto serializer =
      std::make_unique<MessageSerializer>(Node::IdType::RandomHash());
  Network<MockChannel, MessageSerializer> network(
      context, std::move(serializer), std::move(v4), std::move(v6));

  network.OnMessageReceived([](IpEndpoint const &, BufferReader const &) {});
  network.Start();
}

}  // namespace kademlia
}  // namespace p2p
}  // namespace blocxxi

#pragma once

namespace TX {
enum class NetError {
  ConnectTimeout,
  ConnectRefused,
  ConnectReset,
  ConnectAborted,
  ReceiveTimeout,
  ReceiveRefused,
  ReceiveReset,
  ReceiveAborted,
  SendTimeout,
  SendRefused,
  SendReset,
  SendAborted,
};
}

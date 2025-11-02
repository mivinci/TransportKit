#include "KFC/Own.h"
#include "KFC/Preclude.h"
#include "KFC/Testing.h"

KFC_NAMESPACE_BEG

struct A0 {};
struct A1 : A0 {};
struct A1Disposer final : Disposer {
  void disposePtr(void *ptr) override { delete static_cast<A1 *>(ptr); }
};

TEST(OwnTest, upcast) {
  A1Disposer d;
  Own<A0> a0(new A1, &d);
}

KFC_NAMESPACE_END

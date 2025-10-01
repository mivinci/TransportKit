#include <map>

#include "KFC/Ref.h"
#include "KFC/Testing.h"

namespace KFC {

struct Ref_Test : testing::Test {
  void SetUp() override { X_deleted = 0; }
  static int X_deleted;
};

int Ref_Test::X_deleted = 0;

struct X : RefCounted<X> {
  ~X() override { Ref_Test::X_deleted++; }
};

struct X1 final : X {};
struct X2 final : X {};

TEST_F(Ref_Test, Ref) {
  {
    Ref a(*new X);
    EXPECT_EQ(a->refCount(), 1);
    Ref<X> b = a;
    EXPECT_EQ(a->refCount(), 2);
    EXPECT_EQ(b->refCount(), 2);
    Ref c(*new X);
    EXPECT_EQ(c->refCount(), 1);
    b = c;
    EXPECT_EQ(a->refCount(), 1);
    EXPECT_EQ(b->refCount(), 2);
    EXPECT_EQ(c->refCount(), 2);
    Ref d = std::move(b);
    EXPECT_EQ(b.ptr(), nullptr);
    EXPECT_EQ(c->refCount(), 2);
    EXPECT_EQ(d->refCount(), 2);
  }
  EXPECT_EQ(X_deleted, 2);
}

TEST_F(Ref_Test, RefPtr) {
  {
    RefPtr a(new X);
    EXPECT_EQ(a->refCount(), 1);
    a = nullptr;
    EXPECT_EQ(a, nullptr);
    EXPECT_EQ(X_deleted, 1);
    RefPtr b(new X);
    a = b;
    EXPECT_EQ(a->refCount(), 2);
    EXPECT_EQ(b->refCount(), 2);
    RefPtr<X> c;
    EXPECT_EQ(c, nullptr);
  }
  EXPECT_EQ(X_deleted, 2);
}

TEST_F(Ref_Test, RefContainer) {
  std::map<int, Ref<X>> m;
  Ref a = adoptRef(*new X);
  m.emplace(0, a);
  EXPECT_EQ(a->refCount(), 2);
  EXPECT_EQ(m.at(0), a);
  // EXPECT_EQ(m[0], a);
  // m[0] will do a `Ref<X> a` causing a compiler error since there's no
  // constructor with empty arguments for Ref. weired.

  std::set<Ref<X>> s;
  s.emplace(a);
  EXPECT_EQ(a->refCount(), 3);
  EXPECT_EQ(s.find(a).operator*(), a);

  std::vector<Ref<X>> v;
  v.emplace_back(a);
  EXPECT_EQ(a->refCount(), 4);
  EXPECT_EQ(v.at(0), a);
  EXPECT_EQ(v[0], a);
}

TEST_F(Ref_Test, RefInheritance) {
  {
    Ref<X> x = adoptRef(*new X1);
    EXPECT_EQ(x->refCount(), 1);
    Ref<X> y = x;
    EXPECT_EQ(x->refCount(), 2);
    Ref<X> z = std::move(x);
    EXPECT_EQ(x.ptr(), nullptr);
    EXPECT_EQ(y->refCount(), 2);
    EXPECT_EQ(z->refCount(), 2);
    z = adoptRef(*new X2);
    EXPECT_EQ(y->refCount(), 1);
    EXPECT_EQ(z->refCount(), 1);
    Ref<X1> u = adoptRef(*new X1);
    EXPECT_EQ(u->refCount(), 1);
    z = u;
    EXPECT_EQ(u->refCount(), 2);
    EXPECT_EQ(z->refCount(), 2);
  }
  EXPECT_EQ(X_deleted, 3);
}

TEST_F(Ref_Test, RefPtrInheritance) {
  {
    RefPtr<X> x = adoptRef(new X1);
    EXPECT_EQ(x->refCount(), 1);
    RefPtr<X> y = x;
    EXPECT_EQ(x->refCount(), 2);
    RefPtr<X> z = std::move(x);
    EXPECT_EQ(x.get(), nullptr);
    EXPECT_EQ(y->refCount(), 2);
    EXPECT_EQ(z->refCount(), 2);
    z = adoptRef(new X2);
    EXPECT_EQ(y->refCount(), 1);
    EXPECT_EQ(z->refCount(), 1);
    RefPtr<X1> u = adoptRef(new X1);
    EXPECT_EQ(u->refCount(), 1);
    z = u;
    EXPECT_EQ(u->refCount(), 2);
    EXPECT_EQ(z->refCount(), 2);
  }
  EXPECT_EQ(X_deleted, 3);
}
} // namespace KFC

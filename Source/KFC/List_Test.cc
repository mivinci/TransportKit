#include "KFC/List.h"
#include "KFC/Testing.h"

namespace KFC {
struct Mock {
  explicit Mock(const int v) : m_value(v) {}
  int m_value;
  ListLink<Mock> m_link{};
};

TEST(ListTest, Basic) {
  Mock a(1), b(2), c(3);

  List<Mock, &Mock::m_link> list;
  EXPECT_TRUE(list.empty());
  EXPECT_EQ(list.size(), 0);

  list.add(a);
  EXPECT_FALSE(list.empty());
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front().m_value, 1);

  list.add(b);
  EXPECT_EQ(list.size(), 2);

  auto it = list.begin();
  EXPECT_TRUE(it != list.end());
  EXPECT_EQ(it->m_value, 1);
  ++it;
  EXPECT_TRUE(it != list.end());
  EXPECT_EQ(it->m_value, 2);
  ++it;
  EXPECT_TRUE(it == list.end());

  list.addFront(c);
  EXPECT_EQ(list.size(), 3);
  EXPECT_EQ(list.front().m_value, 3);

  list.remove(a);
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front().m_value, 3);

  list.remove(c);
  EXPECT_EQ(list.size(), 1);
  EXPECT_EQ(list.front().m_value, 2);

  list.remove(b);
  EXPECT_TRUE(list.empty());
  EXPECT_EQ(list.size(), 0);
}

TEST(ListTest, RemoveWhileIterating) {
  Mock a(1), b(2), c(3);

  List<Mock, &Mock::m_link> list;
  EXPECT_TRUE(list.empty());
  EXPECT_EQ(list.size(), 0);

  list.add(a);
  list.add(b);
  list.add(c);
  EXPECT_EQ(list.size(), 3);

  for (auto &v : list) {
    if (v.m_value == 2) {
      list.remove(v);
    }
  }
  EXPECT_EQ(list.size(), 2);
  EXPECT_EQ(list.front().m_value, 1);
}
} // namespace KFC

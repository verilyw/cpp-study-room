#include "../src/skiplist.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace kvstore {

TEST(SkipNodeTest, SkipNodeLinkage) {
  // test if the SkipNode's linkage is functioning correctly

  /*
   create a linkage like:
        (6, 20)
           |
  -oo - (2,4) - +00
   */

  auto handle_start = new SkipNode<int, int>(0, 0, true);
  auto handle_end = new SkipNode<int, int>(0, 0, true);
  auto center = new SkipNode<int, int>(2, 4);
  handle_start->After() = center;
  handle_end->Before() = center;
  center->Before() = handle_start;
  center->After() = handle_end;
  center->Above() = new SkipNode<int, int>(6, 20);
  center->Above()->Below() = center;

  EXPECT_EQ(center->Key(), 2);
  EXPECT_EQ(center->Value(), 4);
  EXPECT_EQ(center->Above()->Key(), 6);
  EXPECT_EQ(center->Below(), nullptr);
  // clean up
  delete center->Before();
  delete center->After();
  delete center->Above();
  delete center;
}

TEST(SkipNodeTest, SkipNodeSearch) {
  // test if the SkipNode's SkipSearch is working correctly
  // find the biggest node of key that's smaller or equal to given key

  // create such a topology of SkipNode linkages
  /*
    -oo <-> 2 <-------------------------> 9 <-> +oo
     |      |                             |       |
    -oo <-> 2 <---------5 <-> 7---------> 9 <-> +oo
     |      |           |     |           |       |
    -oo <-> 2 <-> 4 <-> 5 <-> 7---------> 9 <-> +oo
  */
  auto head_1 = new SkipNode<int, int>(0, 0, true);
  auto head_2 = new SkipNode<int, int>(0, 0, true);
  auto head_3 = new SkipNode<int, int>(0, 0, true);
  auto tail_1 = new SkipNode<int, int>(0, 0, true);
  auto tail_2 = new SkipNode<int, int>(0, 0, true);
  auto tail_3 = new SkipNode<int, int>(0, 0, true);

  auto node_2_1 = new SkipNode<int, int>(2, 0);
  auto node_2_2 = new SkipNode<int, int>(2, 0);
  auto node_2_3 = new SkipNode<int, int>(2, 0);

  auto node_4_1 = new SkipNode<int, int>(4, 0);

  auto node_5_1 = new SkipNode<int, int>(5, 0);
  auto node_5_2 = new SkipNode<int, int>(5, 0);

  auto node_7_1 = new SkipNode<int, int>(7, 0);
  auto node_7_2 = new SkipNode<int, int>(7, 0);

  auto node_9_1 = new SkipNode<int, int>(9, 0);
  auto node_9_2 = new SkipNode<int, int>(9, 0);
  auto node_9_3 = new SkipNode<int, int>(9, 0);

  head_1->After() = node_2_1;
  node_2_1->After() = node_4_1;
  node_4_1->After() = node_5_1;
  node_5_1->After() = node_7_1;
  node_7_1->After() = node_9_1;
  node_9_1->After() = tail_1;

  tail_1->Before() = node_9_1;
  node_9_1->Before() = node_7_1;
  node_7_1->Before() = node_5_1;
  node_5_1->Before() = node_4_1;
  node_4_1->Before() = node_2_1;
  node_2_1->Before() = head_1;

  // -------------------------------------
  head_2->After() = node_2_2;
  node_2_2->After() = node_5_2;
  node_5_2->After() = node_7_2;
  node_7_2->After() = node_9_2;
  node_9_2->After() = tail_2;

  tail_2->Before() = node_9_2;
  node_9_2->Before() = node_7_2;
  node_7_2->Before() = node_5_2;
  node_5_2->Before() = node_2_2;
  node_2_2->Before() = head_2;

  // -------------------------------------
  head_3->After() = node_2_3;
  node_2_3->After() = node_9_3;
  node_9_3->After() = tail_3;

  tail_3->Before() = node_9_3;
  node_9_3->Before() = node_2_3;
  node_2_3->Before() = head_3;

  // -----------------------------------
  head_1->Above() = head_2;
  head_2->Above() = head_3;
  tail_1->Above() = tail_2;
  tail_2->Above() = tail_3;

  node_2_1->Above() = node_2_2;
  node_2_2->Above() = node_2_3;
  node_5_1->Above() = node_5_2;
  node_7_1->Above() = node_7_2;
  node_9_1->Above() = node_9_2;
  node_9_2->Above() = node_9_3;

  //--------------------------------------
  head_3->Below() = head_2;
  head_2->Below() = head_1;
  tail_3->Below() = tail_2;
  tail_2->Below() = tail_1;

  node_2_3->Below() = node_2_2;
  node_2_2->Below() = node_2_1;
  node_5_2->Below() = node_5_1;
  node_7_2->Below() = node_7_1;
  node_9_3->Below() = node_9_2;
  node_9_2->Below() = node_9_1;

  auto ans1 = head_3->SkipSearch(6).first;
  EXPECT_EQ(ans1->Key(), 5);

  auto ans2 = head_3->SkipSearch(5).first;
  EXPECT_EQ(ans2->Key(), 5);

  auto ans3 = head_3->SkipSearch(10).first;
  EXPECT_EQ(ans3->Key(), 9);

  auto ans4 = head_3->SkipSearch(3).first;
  EXPECT_EQ(ans4->Key(), 2);

  auto ans5 = head_3->SkipSearch(1).first;
  EXPECT_EQ(ans5->IsSentinel(), true);

  // clean up
  delete head_1;
  delete head_2;
  delete head_3;
  delete tail_1;
  delete tail_2;
  delete tail_3;
  delete node_2_1;
  delete node_2_2;
  delete node_2_3;
  delete node_4_1;
  delete node_5_1;
  delete node_5_2;
  delete node_7_1;
  delete node_7_2;
  delete node_9_1;
  delete node_9_2;
  delete node_9_3;
}

TEST(SkipListTest, InsertSearchTest) {
  SkipList<int, int> skip_list;

  skip_list.SetMaxHeight(5);
  EXPECT_EQ(skip_list.MaxHeight(), 5);
  EXPECT_EQ(skip_list.Height(), 1);
  EXPECT_EQ(skip_list.Size(), 0);

  skip_list.Insert(1, 12);
  skip_list.Insert(4, 13);
  skip_list.Insert(5, 53);
  skip_list.Insert(-2, 2);

  auto res1 = skip_list.Search(4);
  EXPECT_EQ(res1->Key(), 4);
  EXPECT_EQ(res1->Value(), 13);

  auto res2 = skip_list.Search(0);
  EXPECT_EQ(res2->Key(), -2);
  EXPECT_EQ(res2->Value(), 2);

  auto res3 = skip_list.Search(6);
  EXPECT_EQ(res3->Key(), 5);
  EXPECT_EQ(res3->Value(), 53);
}

TEST(SkipListTest, SkipListHeavyInsertTest) {
  SkipList<int, int> skip;
  int test_size = 10000;
  for (int i = 0; i < test_size; i++) {
    skip.Insert(i, i);
  }

  EXPECT_EQ(skip.Size(), test_size);

  for (int i = 0; i < test_size; ++i) {
    EXPECT_EQ(skip.Search(i)->Value(), i);
  }
}

TEST(SkipListTest, SkipListSameKeyInsertTest) {
  SkipList<int, int> skip;
  skip.Insert(1, 12);
  skip.Insert(4, 13);
  skip.Insert(5, 53);
  skip.Insert(-2, 2);

  skip.Insert(-2, 4);
  skip.Insert(-2, 5);

  auto res1 = skip.Search(-2);
  EXPECT_EQ(res1->Key(), -2);
  EXPECT_EQ(res1->Value(), 5);

  skip.Insert(4, 4);
  skip.Insert(4, 9);
  auto res2 = skip.Search(4);
  EXPECT_EQ(res2->Key(), 4);
  EXPECT_EQ(res2->Value(), 9);
}

TEST(SkipListTest, SKipListRemoveTest) {
  SkipList<int, int> skip;
  skip.Insert(1, 12);
  skip.Insert(4, 13);
  skip.Insert(5, 42);
  skip.Insert(-2, 2);

  EXPECT_EQ(skip.Remove(6), false);
  EXPECT_EQ(skip.Remove(5), true);
  EXPECT_EQ(skip.Remove(5), false);
  EXPECT_NE(skip.Search(5)->Key(), 5);
}
}  // namespace kvstore
/************************************************************************
* Copyright (C) 2021 Trevor Hanz
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
************************************************************************/

#include <GraphicsPipeline/GP.h>
#include "../src/Utils/List.h"
#include "../src/Utils/RefCounter.h"

#include "gtest/gtest.h"

struct test_node
{
  gp_list_node      mNode;
  int               mValue;
};

TEST(List, order)
{
  gp_list l;
  gp_list_init(&l);
  
  for(int i=0; i<100; ++i)
  {
    test_node* node = new test_node;
    node->mValue = i;
    gp_list_push_back(&l, (gp_list_node*)node);
  }
  
  int i=0;
  test_node* node = (test_node*)gp_list_front(&l);
  while(node != (test_node*)gp_list_end(&l))
  {
    ASSERT_EQ(node->mValue, i++);
    
    node = (test_node*)gp_list_node_next((gp_list_node*)node);
  }
}

TEST(List, order_reverse)
{
  gp_list l;
  gp_list_init(&l);
  
  for(int i=0; i<100; ++i)
  {
    test_node* node = new test_node;
    node->mValue = i;
    gp_list_push_front(&l, (gp_list_node*)node);
  }
  
  int i=0;
  test_node* node = (test_node*)gp_list_back(&l);
  while(node != (test_node*)gp_list_begin(&l))
  {
    ASSERT_EQ(node->mValue, i++);
    
    node = (test_node*)gp_list_node_prev((gp_list_node*)node);
  }
}

TEST(List, empty)
{
  gp_list l;
  gp_list_init(&l);
  
  const int num_nodes = 10000;
  
  for(int i=0; i<num_nodes; ++i)
  {
    test_node* node = new test_node;
    node->mValue = i;
    gp_list_push_front(&l, (gp_list_node*)node);
  }
  
  int count = 0;
  test_node* node = (test_node*)gp_list_front(&l);
  while(node != (test_node*)gp_list_end(&l))
  {
    test_node* temp = node;
    node = (test_node*)gp_list_node_next((gp_list_node*)node);
    gp_list_remove(&l, (gp_list_node*)temp);
    
    delete temp;
    
    ++count;
  }
  
  ASSERT_EQ(count, num_nodes);
  ASSERT_EQ(gp_list_front(&l), gp_list_end(&l));
}

int _node_test_compare(gp_list_node* first, gp_list_node* second)
{
  return ((test_node*)first)->mValue > ((test_node*)second)->mValue;
}

TEST(List, sort)
{
  gp_list l;
  gp_list_init(&l);
  
  for(int i=0; i<10000; ++i)
  {
    test_node* node = new test_node;
    node->mValue = rand()%1024;
    gp_list_push_front(&l, (gp_list_node*)node);
  }
  
  gp_list_sort(&l, _node_test_compare);
  
  test_node* node = (test_node*)gp_list_front(&l);
  int last_value = node->mValue;
  while(node != (test_node*)gp_list_end(&l))
  {
    ASSERT_LE(last_value, node->mValue);
    last_value = node->mValue;
    
    node = (test_node*)gp_list_node_next((gp_list_node*)node);
  }
}

TEST(List, swap)
{
  gp_list l;
  gp_list_init(&l);
  
  const int size = 10;
  
  test_node* nodes[size];
  
  for(int i=0; i<size; ++i)
  {
    nodes[i] = new test_node;
    nodes[i]->mValue = i;
    gp_list_push_back(&l, (gp_list_node*)nodes[i]);
  }
  
  gp_list_swap(&l, (gp_list_node*)nodes[1], (gp_list_node*)nodes[3]);
  gp_list_swap(&l, (gp_list_node*)nodes[5], (gp_list_node*)nodes[6]);
  gp_list_swap(&l, (gp_list_node*)nodes[9], (gp_list_node*)nodes[0]);
  
  int results[] = {9, 3, 2, 1, 4, 6, 5, 7, 8, 0};
  
  int i = 0;
  test_node* node = (test_node*)gp_list_front(&l);
  while(node != (test_node*)gp_list_end(&l))
  {
    ASSERT_EQ(results[i++], node->mValue);
    
    node = (test_node*)gp_list_node_next((gp_list_node*)node);
  }
}

TEST(RefCounter, init)
{
  gp_refcounter counter;
  gp_ref_init(&counter);
  ASSERT_EQ(counter.mRefCount, 1);
}

TEST(RefCounter, decrement)
{
  gp_refcounter counter;
  gp_ref_init(&counter);
  
  for(int i=0; i<100; ++i)
    gp_ref_inc(&counter);
  
  for(int i=0; i<100; ++i)
    ASSERT_EQ(gp_ref_dec(&counter), 0);
  
  ASSERT_EQ(gp_ref_dec(&counter), 1);
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

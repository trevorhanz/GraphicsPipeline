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

#include "List.h"

#include <stdlib.h>

void gp_list_init(gp_list* list)
{
  list->mBegin = (gp_list_node*)malloc(sizeof(gp_list_node));
  list->mEnd = (gp_list_node*)malloc(sizeof(gp_list_node));
  
  list->mBegin->mPrev = 0;
  list->mBegin->mNext = list->mEnd;
  
  list->mEnd->mPrev = list->mBegin;
  list->mEnd->mNext = 0;
}

void gp_list_free(gp_list* list)
{
  free(list->mBegin);
  free(list->mEnd);
}

void gp_list_push_front(gp_list* list, gp_list_node* node)
{
  node->mNext = list->mBegin->mNext;
  node->mPrev = list->mBegin;
  list->mBegin->mNext->mPrev = node;
  list->mBegin->mNext = node;
}

void gp_list_push_back(gp_list* list, gp_list_node* node)
{
  node->mNext = list->mEnd;
  node->mPrev = list->mEnd->mPrev;
  list->mEnd->mPrev->mNext = node;
  list->mEnd->mPrev = node;
}

void gp_list_remove(gp_list* list, gp_list_node* node)
{
  node->mPrev->mNext = node->mNext;
  node->mNext->mPrev = node->mPrev;
  
  node->mNext = 0;
  node->mPrev = 0;
}

void gp_list_swap(gp_list* list, gp_list_node* first, gp_list_node* second)
{
  gp_list_node* tmp;
  
  tmp = first->mNext;
  first->mNext->mPrev = second;
  first->mNext = second->mNext;
  second->mNext->mPrev = first;
  second->mNext = tmp;
  
  tmp = first->mPrev;
  first->mPrev->mNext = second;
  first->mPrev = second->mPrev;
  second->mPrev->mNext = first;
  second->mPrev = tmp;
}

gp_list_node* _gp_list_sort_partition(gp_list* list, gp_list_node* start, gp_list_node* end, gp_list_compare_t compare)
{
  gp_list_node* pivot = end->mPrev;
  
  gp_list_node* i = start->mNext;
  while(i != pivot)
  {
    if(compare(i, pivot))
    {
      gp_list_node* tmp = i->mPrev;
      
      // Remove i from current position
      i->mPrev->mNext = i->mNext;
      i->mNext->mPrev = i->mPrev;
      
      // Add i after pivot
      i->mNext = pivot->mNext;
      i->mPrev = pivot;
      pivot->mNext->mPrev = i;
      pivot->mNext = i;
      
      i = tmp;
    }
    i = i->mNext;
  }
  
  return pivot;
}

void _gp_list_sort(gp_list* list, gp_list_node* start, gp_list_node* end, gp_list_compare_t compare)
{
  if(start->mNext != end)
  {
    gp_list_node* pivot = _gp_list_sort_partition(list, start, end, compare);
    _gp_list_sort(list, start, pivot, compare);
    _gp_list_sort(list, pivot, end, compare);
  }
}

void gp_list_sort(gp_list* list, gp_list_compare_t compare)
{
  _gp_list_sort(list, list->mBegin, list->mEnd, compare);
}

unsigned int gp_list_size(gp_list* list)
{
  unsigned int result = 0;
  
  gp_list_node* node = list->mBegin;
  while(node)
  {
    ++result;
    node = node->mNext;
  }
  
  return result;
}

gp_list_node* gp_list_find(gp_list* list, gp_list_node_compare func, void* userdata)
{
  gp_list_node* node = list->mBegin;
  while(node)
  {
    if(func(node, userdata)) return node;
    node = node->mNext;
  }
  
  return 0;
}

gp_list_node* gp_list_front(gp_list* list)
{
  return list->mBegin->mNext;
}

gp_list_node* gp_list_back(gp_list* list)
{
  return list->mEnd->mPrev;
}

gp_list_node* gp_list_begin(gp_list* list)
{
  return list->mBegin;
}

gp_list_node* gp_list_end(gp_list* list)
{
  return list->mEnd;
}

gp_list_node* gp_list_node_next(gp_list_node* node)
{
  return node->mNext;
}

gp_list_node* gp_list_node_prev(gp_list_node* node)
{
  return node->mPrev;
}

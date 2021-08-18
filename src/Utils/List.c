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

void gp_list_init(gp_list* list)
{
  list->mBegin = 0;
  list->mEnd = 0;
}

void gp_list_push_front(gp_list* list, gp_list_node* node)
{
  node->mNext = list->mBegin;
  node->mPrev = 0;
  if(list->mBegin)
  {
    list->mBegin->mPrev = node;
  }
  else
  {
    list->mEnd = node;
  }
  list->mBegin = node;
}

void gp_list_push_back(gp_list* list, gp_list_node* node)
{
  node->mNext = 0;
  node->mPrev = list->mEnd;
  if(list->mEnd)
  {
    list->mEnd->mNext = node;
  }
  else
  {
    list->mBegin = node;
  }
  list->mEnd = node;
}

void gp_list_remove(gp_list* list, gp_list_node* node)
{
  if(node->mNext) node->mNext->mPrev = node->mPrev;
  else list->mEnd = node->mPrev;
  
  if(node->mPrev) node->mPrev->mNext = node->mNext;
  else list->mBegin = node->mNext;
  
  node->mNext = 0;
  node->mPrev = 0;
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
  return list->mBegin;
}

gp_list_node* gp_list_back(gp_list* list)
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

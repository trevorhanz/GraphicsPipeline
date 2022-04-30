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

#ifndef __GP_LIST_H__
#define __GP_LIST_H__

typedef struct _gp_list gp_list;
typedef struct _gp_list_node gp_list_node;
typedef int (*gp_list_node_compare)(gp_list_node* node, void* userdata);

struct _gp_list
{
  gp_list_node*             mBegin;
  gp_list_node*             mEnd;
};

struct _gp_list_node
{
  gp_list_node*             mPrev;
  gp_list_node*             mNext;
};

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*gp_list_compare_t)(gp_list_node* first, gp_list_node* second);

void gp_list_init(gp_list* l);
void gp_list_free(gp_list* l);

void gp_list_push_front(gp_list* list, gp_list_node* node);
void gp_list_push_back(gp_list* list, gp_list_node* node);
void gp_list_remove(gp_list* list, gp_list_node* node);

void gp_list_swap(gp_list* list, gp_list_node* first, gp_list_node* second);

void gp_list_sort(gp_list* list, gp_list_compare_t compare);

unsigned int gp_list_size(gp_list* list);

gp_list_node* gp_list_find(gp_list* list, gp_list_node_compare func, void* userdata);

gp_list_node* gp_list_front(gp_list* list);           //!< First node in the list.
gp_list_node* gp_list_back(gp_list* list);            //!< Last node in the list.
gp_list_node* gp_list_begin(gp_list* list);           //!< Dummy node before first node.
gp_list_node* gp_list_end(gp_list* list);             //!< Dummy node after last node.
gp_list_node* gp_list_node_next(gp_list_node* node);
gp_list_node* gp_list_node_prev(gp_list_node* node);

#ifdef __cplusplus
}
#endif

#endif // __GP_LIST_H__

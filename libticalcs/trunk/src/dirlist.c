/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Li�vin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Utility functions for directory list (tree management).
	All functions can be applied on both trees (vars & apps) because:
	- tree type is stored in the tree itself (TreeInfo),
	- vars & apps use the same tree format.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ticonv.h>
#include "gettext.h"
#include "ticalcs.h"
#include "logging.h"

/* Dirlist format:

- Vars:

  top (data = TreeInfo)
  |
  + folder (data = NULL if TI8x, data = VarEntry if TI9x)
	  |
	  +- var1 (data = VarEntry)
	  +- var2 (data = VarEntry)

- Apps:

  top (data = TreeInfo)
  |
  + no folder (data = NULL)
	|
	+- app1 (data = VarEntry)
	+- app2 (data = VarEntry)

*/

static gboolean free_varentry(GNode* node, gpointer data)
{
	if(node)
	{
		if(node->data)
		{
			VarEntry* ve = node->data;

			tifiles_ve_delete(ve);
		}
	}

	return FALSE;
}

/**
 * ticalcs_dirlist_destroy:
 * @tree: the tree to destroy.
 *
 * Destroy the whole tree create by #ticalcs_calc_get_dirlist.
 *
 * Return value: none.
 **/
TIEXPORT3 void TICALL ticalcs_dirlist_destroy(GNode** tree)
{
	if (*tree != NULL) 
	{
		TreeInfo *ti;

		if((*tree)->children != NULL)
		{
			g_node_traverse(*tree, G_IN_ORDER, G_TRAVERSE_LEAVES, -1, free_varentry, NULL);
		}
			
		ti = (TreeInfo *)((*tree)->data);
		g_free((*tree)->data);
		g_node_destroy(*tree);

		*tree = NULL;
	}
}

/**
 * ticalcs_dirlist_display:
 * @tree: the tree to display.
 *
 * Display to stdout the tree content formatted in a tab.
 *
 * Return value: none.
 **/
TIEXPORT3 void TICALL ticalcs_dirlist_display(GNode* tree)
{
	GNode *vars = tree;
	TreeInfo *info = (TreeInfo *)(tree->data);
	int i, j, k;
	char *utf8;
  
	if (tree == NULL)
		return;

  printf(  "+------------------+----------+----+----+----------+----------+\n");
  printf(_("| B. name          | T. name  |Attr|Type| Size     | Folder   |\n"));
  printf(  "+------------------+----------+----+----+----------+----------+\n");

  for (i = 0; i < (int)g_node_n_children(vars); i++)	// parse folders
  {
    GNode *parent = g_node_nth_child(vars, i);
    VarEntry *fe = (VarEntry *) (parent->data);

    if (fe != NULL) 
	{
		utf8 = ticonv_varname_to_utf8(info->model, fe->name, -1);

      printf("| ");
      for (k = 0; k < 8; k++)
		printf("%02X", (uint8_t) (fe->name)[k]);
      printf(" | ");	
      printf("%8s", utf8);
      printf(" | ");
      printf("%2i", fe->attr);
      printf(" | ");
      printf("%02X", fe->type);
      printf(" | ");
      printf("%08X", fe->size);
      printf(" | ");
      printf("%8s", fe->folder);
      printf(" |");
	  printf("\n");

	  g_free(utf8);
    }

    for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse variables
    {
      GNode *child = g_node_nth_child(parent, j);
      VarEntry *ve = (VarEntry *) (child->data);

	  utf8 = ticonv_varname_to_utf8(info->model, ve->name, ve->type);

      printf("| ");
      for (k = 0; k < 8; k++) 
		printf("%02X", (uint8_t) (ve->name)[k]);
      printf(" | ");
      printf("%8s", utf8);
      printf(" | ");
      printf("%2i", ve->attr);
      printf(" | ");
      printf("%02X", ve->type);
      printf(" | ");
      printf("%08X", ve->size);
      printf(" | ");
      printf("%8s", ve->folder);
      printf(" |");
	  printf("\n");

	  g_free(utf8);
    }
  }
  if (!i)
  {
	  if(!strcmp(info->type, VAR_NODE_NAME))
		printf(_("| No variables     |\n"));
	  else if(!strcmp(info->type, APP_NODE_NAME))
		printf(_("| No applications  |\n"));
  }
  printf(_("+------------------+----------+----+----+----------+----------+"));
  printf("\n");
}

/**
 * ticalcs_dirlist_ve_exist:
 * @tree: the tree to parse.
 * @s: the full name of the variable or application to search for.
 *
 * Parse the tree for the given varname & folder or appname.
 *
 * Return value: a pointer on the #VarEntry found or NULL if not found.
 **/
TIEXPORT3 VarEntry *TICALL ticalcs_dirlist_ve_exist(GNode* tree, VarEntry *s)
{
	int i, j;
	GNode *vars = tree;
	TreeInfo *info = (TreeInfo *)(tree->data);

	if (tree == NULL)
		return NULL;

	if (strcmp(info->type, VAR_NODE_NAME) && strcmp(info->type, APP_NODE_NAME))
		return NULL;

	for (i = 0; i < (int)g_node_n_children(vars); i++)	// parse folders
	{
		GNode *parent = g_node_nth_child(vars, i);
		VarEntry *fe = (VarEntry *) (parent->data);

		if ((fe != NULL) && strcmp(fe->name, s->folder))
			continue;

		for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse variables
		{
			GNode *child = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (child->data);

			if (!strcmp(ve->name, s->name))
				return ve;
		}
	}

	return NULL;
}

/**
 * ticalcs_dirlist_ve_count:
 * @tree: a tree (var or app).
 *
 * Count how many entries (vars or apps) are listed in the tree.
 *
 * Return value: the number of entries.
 **/
TIEXPORT3 int TICALL ticalcs_dirlist_ve_count(GNode* tree)
{
	int i, j;
	GNode *vars = tree;
	int nvars = 0;
	TreeInfo *info = (TreeInfo *)(tree->data);

	if (tree == NULL)
		return 0;

	if (strcmp(info->type, VAR_NODE_NAME) && strcmp(info->type, APP_NODE_NAME))
		return 0;

	for (i = 0; i < (int)g_node_n_children(vars); i++)	// parse folders
	{
		GNode *parent = g_node_nth_child(vars, i);

		for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse variables
			nvars++;
	}

	return nvars;
}

/**
 * ticalcs_dirlist_ram_used:
 * @tree: a tree (var only).
 *
 * Count how much memory is used by variables listed in the tree.
 *
 * Return value: size of all variables in bytes.
 **/
TIEXPORT3 int TICALL ticalcs_dirlist_ram_used(GNode* tree)
{
	int i, j;
	GNode *vars = tree;
	uint32_t mem = 0;
	TreeInfo *info = (TreeInfo *)(tree->data);

	if (tree == NULL)
		return 0;

	if (strcmp(info->type, VAR_NODE_NAME))
		return 0;
	
	for (i = 0; i < (int)g_node_n_children(vars); i++)	// parse folders
	{
		GNode *parent = g_node_nth_child(vars, i);

		for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse variables
		{
			GNode *child = g_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (child->data);

			if(ve->attr != ATTRB_ARCHIVED)
				mem += ve->size;
		}
	}

	return mem;
}

/**
 * ticalcs_dirlist_flash_used:
 * @tree: a tree (app only).
 *
 * Count how much memory is used by archived variables and apps listed in the trees.
 *
 * Return value: size of all FLASH in bytes.
 **/
TIEXPORT3 int TICALL ticalcs_dirlist_flash_used(GNode* vars, GNode* apps)
{
	int i, j;
	uint32_t mem = 0;
	TreeInfo *info1 = (TreeInfo *)(vars->data);
	TreeInfo *info2 = (TreeInfo *)(apps->data);

	if (!vars && !apps)
		return 0;

	if (!strcmp(info1->type, VAR_NODE_NAME))
	{
		for (i = 0; i < (int)g_node_n_children(vars); i++)	// parse folders
		{
			GNode *parent = g_node_nth_child(vars, i);

			for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse variables
			{
				GNode *child = g_node_nth_child(parent, j);
				VarEntry *ve = (VarEntry *) (child->data);

				if(ve->attr == ATTRB_ARCHIVED)
					mem += ve->size;
			}
		}
	}

	if (!strcmp(info2->type, APP_NODE_NAME))
	{
		for (i = 0; i < (int)g_node_n_children(apps); i++) 
		{
			GNode *parent = g_node_nth_child(apps, i);

			for (j = 0; j < (int)g_node_n_children(parent); j++)	//parse apps
			{
				GNode *child = g_node_nth_child(parent, i);
				VarEntry *ve = (VarEntry *) (child->data);

				mem += ve->size;
			}
		}
	}

	return mem;
}

/**
 * ticalcs_dirlist_ve_add:
 * @tree: source tree.
 * @entry: entry to add.
 *
 * Add an entry into the main tree (if it doesn't exist yet).
 *
 * Return value: none.
 **/
TIEXPORT3 void TICALL ticalcs_dirlist_ve_add(GNode* tree, VarEntry *entry)
{
	TreeInfo *info = (TreeInfo *)(tree->data);
	int i, j;
	int found = 0;

	GNode *parent = NULL;
	VarEntry *fe = NULL;

	GNode *child;
	VarEntry *ve;

	char *folder;
	
	if (tree == NULL)
		return;

	if (strcmp(info->type, VAR_NODE_NAME) && strcmp(info->type, APP_NODE_NAME))
		return;

	if(!strcmp(entry->folder, "") && tifiles_has_folder(info->model))
		folder = "main";
	else
		folder = entry->folder;

	// If TI8x tree is empty, create pseudo-folder (NULL)
	if(!g_node_n_children(tree) && !tifiles_has_folder(info->model))
	{
		parent = g_node_new(NULL);
		g_node_append(tree, parent);
	}

	// Check for one folder at least...
	if(g_node_n_children(tree) > 0)
	{
		// Parse folders
		for (found = 0, i = 0; i < (int)g_node_n_children(tree); i++)
		{
			parent = g_node_nth_child(tree, i);
			fe = (VarEntry *) (parent->data);

			if(fe == NULL)
				break;

			if(!strcmp(fe->name, folder))
			{
				found = !0;
				break;
			}
		}
	}
		
	// folder doesn't exist? => create!
	if((!found && fe) || 
	   (!g_node_n_children(tree) && tifiles_has_folder(info->model)))
	{	
		fe = tifiles_ve_create();
		strcpy(fe->name, entry->folder);
		fe->type = TI89_DIR;

		parent = g_node_new(fe);
		g_node_append(tree, parent);
	}

	if(!strcmp(entry->name, ""))
		return;

	// next, add variables beneath this folder
	for(found = 0, j = 0; j < (int)g_node_n_children(parent); j++)
	{
		child = g_node_nth_child(parent, j);
		ve = (VarEntry *) (child->data);

		if(!strcmp(ve->name, entry->name))
		{
			found = !0;
			break;
		}
	}

	if(!found)
	{
		ve = tifiles_ve_dup(entry);
		child = g_node_new(ve);
		g_node_append(parent, child);
	}

	if(fe && found)
		fe->size++;
}


/**
 * ticalcs_dirlist_ve_del:
 * @tree: source tree.
 * @entry: entry to remove.
 *
 * Remove an entry into the main tree (if it doesn't exist yet).
 *
 * Return value: none.
 **/
TIEXPORT3 void TICALL ticalcs_dirlist_ve_del(GNode* tree, VarEntry *entry)
{
	TreeInfo *info = (TreeInfo *)(tree->data);
	int i, j;
	int found = 0;

	GNode *parent = NULL;
	VarEntry *fe = NULL;

	GNode *child = NULL;
	VarEntry *ve;

	char *folder;
	
	if (tree == NULL)
		return;

	if (strcmp(info->type, VAR_NODE_NAME))
		return;

	if(!strcmp(entry->folder, "") && tifiles_has_folder(info->model))
		folder = "main";
	else
		folder = entry->folder;

	// Parse folders
	for (found = 0, i = 0; i < (int)g_node_n_children(tree); i++)
	{
		parent = g_node_nth_child(tree, i);
		fe = (VarEntry *) (parent->data);

		if(fe == NULL)
			break;

		if(!strcmp(fe->name, folder))
		{
			found = !0;
			break;
		}
	}

	if(!found && fe)
		return;
		
	// next, delete variables beneath this folder
	for(found = 0, j = 0; j < (int)g_node_n_children(parent); j++)
	{
		child = g_node_nth_child(parent, j);
		ve = (VarEntry *) (child->data);

		if(!strcmp(ve->name, entry->name))
		{
			found = !0;
			break;
		}
	}

	if(found)
	{
		tifiles_ve_delete(child->data);
		g_node_destroy(child);
	}

	if(fe && found)
		fe->size--;
}

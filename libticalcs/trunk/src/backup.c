/* Hey EMACS -*- linux-c -*- */
/* $Id: link_nul.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
	Pseudo-backup support (common).
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "ticonv.h"
#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "pause.h"

#ifdef __WIN32__
#undef snprintf
#define snprintf _snprintf
#endif

int tixx_recv_backup(CalcHandle* handle, BackupContent* content)
{
	int i, j, k;
	int i_max, j_max;
	TNode *vars, *apps;
	int nvars, ivars = 0;
	int b = 0;
	FileContent **group;
	FileContent *single;

	// Do a directory list and check for something to backup
	TRYF(handle->calc->get_dirlist(handle, &vars, &apps));
	nvars = ticalcs_dirlist_num_vars(vars);
	if(!nvars)
		return ERR_NO_VARS;

	// Check whether the last folder is empty
	b = t_node_n_children(t_node_nth_child(vars, t_node_n_children(vars) - 1));
	PAUSE(100); // needed by TI84+/USB

	// Create a group file
	k = 0;
	group = tifiles_content_create_group(nvars);

	// Receive all vars except for FLASH apps
	i_max = t_node_n_children(vars);
	for(i = 0; i < i_max; i++) 
	{
		TNode *parent = t_node_nth_child(vars, i);

		j_max = t_node_n_children(parent);
		for(j = 0; j < j_max; j++) 
		{
			TNode *node = t_node_nth_child(parent, j);
			VarEntry *ve = (VarEntry *) (node->data);

			// we need to group files !
			TRYF(handle->calc->is_ready(handle));
			group[k] = tifiles_content_create_regular(handle->model);
			TRYF(handle->calc->recv_var(handle, 0, group[k++], ve));

			update_->cnt2 = ++ivars;
			update_->max2 = nvars;
			update_->pbar();
		}
	}

	ticalcs_dirlist_destroy(&vars);
	ticalcs_dirlist_destroy(&apps);

	tifiles_group_contents(group, &single);
	tifiles_content_delete_group(group);

	// Swap content and single because we have a pointer on an allocated content
	{
		FileContent* cnt = (FileContent *)content;

		memcpy(content, single, sizeof(FileContent));
		cnt->entries = single->entries;
		strcpy(cnt->comment, tifiles_comment_set_group());
	}

	return 0;
}
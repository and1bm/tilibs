#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
 * $Id$
 */

#ifdef __MACOSX__

#define VERSION "3.7.6"

#define HAVE_SYS_PERM_H
#define HAVE_SYS_IPC_H  /* needed for virtual linking */
#define HAVE_SYS_SHM_H
#define HAVE_VPRINTF
#define HAVE_STDINT_H

#endif __MACOSX__

#endif /* !__CONFIG_H__ */
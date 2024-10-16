/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      File I/O.
 *
 *      By Shawn Hargreaves.
 *
 *      See readme.txt for copyright information.
 */


#ifndef ALLEGRO_FILE_H
#define ALLEGRO_FILE_H

#include "base.h"

#ifdef __cplusplus
   extern "C" {
#endif

AL_FUNC(char *, fix_filename_case, (char *path));
AL_FUNC(char *, fix_filename_slashes, (char *path));
AL_FUNC(char *, canonicalize_filename, (char *dest, AL_CONST char *filename, int size));
AL_FUNC(char *, make_absolute_filename, (char *dest, AL_CONST char *path, AL_CONST char *filename, int size));
AL_FUNC(char *, make_relative_filename, (char *dest, AL_CONST char *path, AL_CONST char *filename, int size));
AL_FUNC(int, is_relative_filename, (AL_CONST char *filename));
AL_FUNC(char *, replace_filename, (char *dest, AL_CONST char *path, AL_CONST char *filename, int size));
AL_FUNC(char *, replace_extension, (char *dest, AL_CONST char *filename, AL_CONST char *ext, int size));
AL_FUNC(char *, append_filename, (char *dest, AL_CONST char *path, AL_CONST char *filename, int size));
AL_FUNC(char *, get_filename, (AL_CONST char *path));
AL_FUNC(char *, get_extension, (AL_CONST char *filename));
AL_FUNC(void, put_backslash, (char *filename));

#ifndef EOF
   #define EOF    (-1)
#endif

#define F_READ          "r"
#define F_WRITE         "w"

#define F_BUF_SIZE      4096           /* 4K buffer for caching data */

#define PACKFILE_FLAG_WRITE      1     /* the file is being written */
#define PACKFILE_FLAG_CHUNK      4     /* file is a sub-chunk */
#define PACKFILE_FLAG_EOF        8     /* reached the end-of-file */
#define PACKFILE_FLAG_ERROR      16    /* an error has occurred */


typedef struct PACKFILE_VTABLE PACKFILE_VTABLE;
typedef struct PACKFILE PACKFILE;

struct LZSS_PACK_DATA;
struct LZSS_UNPACK_DATA;


struct _al_normal_packfile_details
{
   int hndl;                           /* DOS file handle */
   int flags;                          /* PACKFILE_FLAG_* constants */
   unsigned char *buf_pos;             /* position in buffer */
   int buf_size;                       /* number of bytes in the buffer */
   long todo;                          /* number of bytes still on the disk */
   struct PACKFILE *parent;            /* nested, parent file */
   char *filename;                     /* name of the file */
   unsigned char buf[F_BUF_SIZE];      /* the actual data buffer */
};


struct PACKFILE                           /* our very own FILE structure... */
{
   AL_CONST PACKFILE_VTABLE *vtable;
   void *userdata;
   int is_normal_packfile;

   /* The following is only to be used for the "normal" PACKFILE vtable,
    * i.e. what is implemented by Allegro itself. If is_normal_packfile is
    * false then the following is not even allocated. This must be the last
    * member in the structure.
    */
   struct _al_normal_packfile_details normal;
};


struct PACKFILE_VTABLE
{
   AL_METHOD(int, pf_fclose, (void *userdata));
   AL_METHOD(int, pf_getc, (void *userdata));
   AL_METHOD(int, pf_ungetc, (int c, void *userdata));
   AL_METHOD(long, pf_fread, (void *p, long n, void *userdata));
   AL_METHOD(int, pf_putc, (int c, void *userdata));
   AL_METHOD(long, pf_fwrite, (AL_CONST void *p, long n, void *userdata));
   AL_METHOD(int, pf_fseek, (void *userdata, int offset));
   AL_METHOD(int, pf_feof, (void *userdata));
   AL_METHOD(int, pf_ferror, (void *userdata));
};


#define uconvert_tofilename(s, buf)      uconvert(s, U_CURRENT, buf, get_filename_encoding(), sizeof(buf))

AL_FUNC(void, set_filename_encoding, (int encoding));
AL_FUNC(int, get_filename_encoding, (void));

AL_FUNC(void, packfile_password, (AL_CONST char *password));
AL_FUNC(PACKFILE *, pack_fopen, (AL_CONST char *filename, AL_CONST char *mode));
AL_FUNC(PACKFILE *, pack_fopen_vtable, (AL_CONST PACKFILE_VTABLE *vtable, void *userdata));
AL_FUNC(int, pack_fclose, (PACKFILE *f));
AL_FUNC(int, pack_fseek, (PACKFILE *f, int offset));
AL_FUNC(PACKFILE *, pack_fopen_chunk, (PACKFILE *f, int pack));
AL_FUNC(PACKFILE *, pack_fclose_chunk, (PACKFILE *f));
AL_FUNC(int, pack_getc, (PACKFILE *f));
AL_FUNC(int, pack_putc, (int c, PACKFILE *f));
AL_FUNC(int, pack_feof, (PACKFILE *f));
AL_FUNC(int, pack_ferror, (PACKFILE *f));
AL_FUNC(int, pack_igetw, (PACKFILE *f));
AL_FUNC(long, pack_igetl, (PACKFILE *f));
AL_FUNC(int, pack_iputw, (int w, PACKFILE *f));
AL_FUNC(long, pack_iputl, (long l, PACKFILE *f));
AL_FUNC(int, pack_mgetw, (PACKFILE *f));
AL_FUNC(long, pack_mgetl, (PACKFILE *f));
AL_FUNC(int, pack_mputw, (int w, PACKFILE *f));
AL_FUNC(long, pack_mputl, (long l, PACKFILE *f));
AL_FUNC(long, pack_fread, (void *p, long n, PACKFILE *f));
AL_FUNC(long, pack_fwrite, (AL_CONST void *p, long n, PACKFILE *f));
AL_FUNC(int, pack_ungetc, (int c, PACKFILE *f));
AL_FUNC(char *, pack_fgets, (char *p, int max, PACKFILE *f));
AL_FUNC(int, pack_fputs, (AL_CONST char *p, PACKFILE *f));
AL_FUNC(void *, pack_get_userdata, (PACKFILE *f));



#ifdef __cplusplus
   }
#endif

#endif          /* ifndef ALLEGRO_FILE_H */



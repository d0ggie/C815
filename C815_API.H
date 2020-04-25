#ifndef __C815_API_H
#define __C815_API_H

#define \
TRACE()                                                                        \
  ({                                                                           \
printf("@ %.256s:%u\n", __FILE__, __LINE__);                                   \
  })


#ifdef _DEBUG
#  define \
FDUMP__ALT(_object, _size)                                                     \
  ({                                                                           \
  FILE *                                                                       \
  const                                                                        \
  __file__hndl =                                                               \
  fopen("DUMP\\" #_object ".RAW", "wb");                                       \
  if (!!                                                                       \
  __file__hndl)                                                                \
    {                                                                          \
  fwrite__alt(                                                                 \
   _object, _size,                                                             \
  __file__hndl);                                                               \
  fclose(                                                                      \
  __file__hndl);                                                               \
    }                                                                          \
  })
#else
#  define \
FDUMP__ALT(...)
#endif // ..

#define \
FDUMP(_object)                                                                 \
  ({                                                                           \
FDUMP__ALT(_object, sizeof(_object));                                          \
  })

#ifdef _DEBUG
#  define \
FTELL(_file__hndl)                                                             \
  ({                                                                           \
printf("OFFSET: %08X @ %.256s:%u.\n", (unsigned int)                           \
  ftell(_file__hndl), __FILE__, __LINE__);                                     \
  })
#else
#  define \
FTELL(...)
#endif // ..


static
inline
void
GLogic__FAILURE(
  bool *
  const
    retc
  )
  {
 *retc = false;
  return;
  }

static
inline
void
GLogic__SUCCESS(
  bool *
  const
    retc
  )
  {
 *retc = true;
  return;
  }

__attribute__((__noreturn__))
static
inline
void
GLogic__Abort(
  const
  char *
  const
    text__fmt, ...
  )
  {
  char
    text[256];
  va_list
    text__ap;
  va_start(
    text__ap,
    text__fmt);
  vsnprintf(
    text,
      sizeof(
    text),
    text__fmt,
    text__ap);
  va_end(
    text__ap);

  fprintf(stderr,
    "oops ``%.*s'', aborting.\n",
      (int) sizeof(
    text),
    text);
  exit(
    EXIT_FAILURE);
  }


static
inline
void *
GAlloc(
  size_t
    size
  )
  {
  void *
  const
    data =
  malloc(size);
  if (!
    data)
  GLogic__Abort(
    "unable to allocate `%lu' byte(s)",
    size);
  return
    data;
  }

static
inline
void *
GAlloZ(
  size_t
    size)
  {
  void *
  const
    data =
GAlloc(
    size);
  memset(
    data, 0,
    size);
  return
    data;
  }

static
inline
void *
GAlloD(
  size_t
    size,
  const
  void *
    data__src
  )
  {
  void *
  const
    data =
GAlloc(
    size);
  memcpy(
    data,
    data__src,
    size);
  return
    data;
  }

static
inline
void
GAlloc__Free(
  void **
    data
  )
  {
  if (
    data)
    {
  void *
  const
  __data =
   *data;
  if (!!
  __data)
  free(
  __data);
   *data = NULL;
    }

  return;
  }

  // Note: Transparent union does unfortunately not work with `__cleanup__'.
static
inline
void
GAlloc__FreeU8(
  uint8_t **
    data
  )
  {
GAlloc__Free((void **)
    data);
  return;
  }

static
inline
void
GAlloc__FreeI8(
  int8_t **
    data
  )
  {
GAlloc__Free((void **)
    data);
  return;
  }

static
inline
void
GAlloc__FreeUI16(
  uint16_t **
    data)
  {
GAlloc__Free((void **)
    data);
  return;
  }

static
inline
void
GAlloc__FreeI16(
  int16_t **
    data)
  {
GAlloc__Free((void **)
    data);
  return;
  }


static
inline
void
GStdIO__Close(
  FILE **
    file__hndl
  )
  {
  if (!!
    file__hndl)
    {
  FILE *
  __file__hndl =
   *file__hndl;
  if (!!
  __file__hndl)
    {
  fclose(
  __file__hndl);
   *file__hndl = NULL;
    }
    }

  return;
  }

static
inline
FILE *
fopen__alt(
  const
  char *
  const
    name,
  const
  char *
  const
    mode
  )
  {
  FILE *
    file__hndl =
  fopen(
    name, mode);
  if (!
    file__hndl)
  GLogic__Abort(
    "unable to open `%.128s'.",
    name);
  return
    file__hndl;
  }

static
inline
void
fread__alt(
  void *
  const
    buff,
  size_t
    size,
  FILE *
  const
    file__hndl)
  {
  size_t
  __size;
  if ((
  __size =
fread(
    buff,
    1, size,
    file__hndl))
      !=
    size)
GLogic__Abort(
  "read only `%lu' bytes, wanted to read `%lu'.",
  __size,
    size);

  return;
  }

#define \
FREAD(_object, _file__hndl)                                                    \
  ({                                                                           \
fread__alt(&(_object), sizeof(_object), _file__hndl);                          \
  })


static
inline
void
fwrite__alt(
  const
  void *
  const
    buff,
  size_t
    size,
  FILE *
  const
    file__hndl)
  {
  size_t
  __size;
  if ((
  __size =
fwrite(
    buff,
    1, size,
    file__hndl))
      !=
    size)
GLogic__Abort(
  "wrote only `%lu' bytes, wanted to write `%lu'.",
  __size,
    size);

  return;
  }

#define \
FWRITE(_object, _file__hndl)                                                   \
  ({                                                                           \
fwrite__alt(&(_object), sizeof(_object), _file__hndl);                         \
  })

#endif//__C815_API_H

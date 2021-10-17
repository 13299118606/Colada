
#ifndef VTKEIGEN_EXPORT_H
#define VTKEIGEN_EXPORT_H

#ifdef VTKEIGEN_STATIC_DEFINE
#  define VTKEIGEN_EXPORT
#  define VTKEIGEN_NO_EXPORT
#else
#  ifndef VTKEIGEN_EXPORT
#    ifdef vtkEigen_EXPORTS
        /* We are building this library */
#      define VTKEIGEN_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define VTKEIGEN_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef VTKEIGEN_NO_EXPORT
#    define VTKEIGEN_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef VTKEIGEN_DEPRECATED
#  define VTKEIGEN_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VTKEIGEN_DEPRECATED_EXPORT
#  define VTKEIGEN_DEPRECATED_EXPORT VTKEIGEN_EXPORT VTKEIGEN_DEPRECATED
#endif

#ifndef VTKEIGEN_DEPRECATED_NO_EXPORT
#  define VTKEIGEN_DEPRECATED_NO_EXPORT VTKEIGEN_NO_EXPORT VTKEIGEN_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VTKEIGEN_NO_DEPRECATED
#    define VTKEIGEN_NO_DEPRECATED
#  endif
#endif

#endif /* VTKEIGEN_EXPORT_H */

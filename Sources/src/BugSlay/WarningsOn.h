/* ----------------------------------------------------------------------
        */

#ifndef _WARNINGSON_H
#define _WARNINGSON_H

/* Nonstandard extension : nameless struct/union */
#pragma warning ( default : 4201 )

/* Unreferenced in inline function removed */
/* #pragma warning ( default : 4514 ) */

/* conditional expression is constant */
/* #pragma warning ( default : 4127 ) */

/* default constructor could not be generated */
#pragma warning ( default : 4510 )

/* struct '' can never be instantiated - user defined constructor */
/* required */
#pragma warning ( default : 4610 )

/* unreferenced local function has been removed */
#pragma warning ( default : 4505 )

/* STL spews errors just including them! */
/* C++ language change: to explicitly specialize class template ''
    */
#pragma warning ( default : 4663 )
/* '' : signed/unsigned mismatch */
#pragma warning ( default : 4018 )
/* '' : unreferenced formal parameter */
#pragma warning ( default : 4100 )

#endif  /* _WARNINGSON_H */
#ifndef _WXSQLITE3MACROS_H_
#define _WXSQLITE3MACROS_H_

#include "wx/wxsqlite3.h"

#define mmBEGINSQL_LITE_EXCEPTION   \
    try                             \
    {                               \


#define mmENDSQL_LITE_EXCEPTION     \
    }                               \
    catch (wxSQLite3Exception& e)   \
    {                               \
        wxASSERT(false);            \
    }                               \

#define mmENDSQL_LITE_IGNOREEXCEPTION     \
    }                                     \
    catch (wxSQLite3Exception& e)         \
    {                                     \
                                          \
    }                                     \

#endif

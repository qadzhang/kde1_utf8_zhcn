#ifdef UNICODE
typedef SQLWCHAR        SQLTCHAR;
#else
typedef SQLCHAR         SQLTCHAR;
#endif

#define SQL_WCHAR              (-8)
#define SQL_WVARCHAR           (-9)
#define SQL_WLONGVARCHAR       (-10)
#define SQL_C_WCHAR            SQL_WCHAR

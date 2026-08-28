# TQt sql module

sql {

	!table {
		message(table must be enabled for sql support)
		REQUIRES += table
	}
	
	SQL_P	    = sql
	HEADERS     += $$SQL_H/ntqsql.h \
		    $$SQL_H/ntqsqlquery.h \
		    $$SQL_H/ntqsqldatabase.h \
		    $$SQL_H/ntqsqlfield.h \
		    $$SQL_H/ntqsqlrecord.h \
		    $$SQL_H/ntqsqlcursor.h \
		    $$SQL_H/ntqsqlform.h \
		    $$SQL_H/ntqeditorfactory.h \
		    $$SQL_H/ntqsqleditorfactory.h \
		    $$SQL_H/ntqsqldriver.h \
		    $$SQL_P/qsqldriverinterface_p.h \
		    $$SQL_P/qsqlextension_p.h \
		    $$SQL_H/ntqsqldriverplugin.h \
		    $$SQL_H/ntqsqlerror.h \
		    $$SQL_H/ntqsqlresult.h \
		    $$SQL_H/ntqsqlindex.h \
		    $$SQL_H/ntqsqlpropertymap.h \
		    $$SQL_P/qsqlmanager_p.h \
		    $$SQL_H/ntqdatatable.h \
		    $$SQL_H/ntqdataview.h \
		    $$SQL_H/ntqdatabrowser.h \
		    $$SQL_H/ntqsqlselectcursor.h 

	SOURCES     += $$SQL_CPP/qsqlquery.cpp \
		    $$SQL_CPP/qsqldatabase.cpp \
		    $$SQL_CPP/qsqlfield.cpp \
		    $$SQL_CPP/qsqlrecord.cpp \
		    $$SQL_CPP/qsqlform.cpp \
		    $$SQL_CPP/qsqlcursor.cpp \
		    $$SQL_CPP/qeditorfactory.cpp \
		    $$SQL_CPP/qsqleditorfactory.cpp \
		    $$SQL_CPP/qsqldriver.cpp \
		    $$SQL_CPP/qsqlextension_p.cpp \
		    $$SQL_CPP/qsqldriverplugin.cpp \
		    $$SQL_CPP/qsqlerror.cpp \
		    $$SQL_CPP/qsqlresult.cpp \
		    $$SQL_CPP/qsqlindex.cpp \
		    $$SQL_CPP/qsqlpropertymap.cpp \
		    $$SQL_CPP/qsqlmanager_p.cpp \
		    $$SQL_CPP/qdatatable.cpp \
		    $$SQL_CPP/qdataview.cpp \
		    $$SQL_CPP/qdatabrowser.cpp \
		    $$SQL_CPP/qsqlselectcursor.cpp \
		    $$SQL_CPP/drivers/cache/qsqlcachedresult.cpp

	contains(sql-drivers, all ) {
		sql-driver += psql mysql odbc oci tds db2 sqlite ibase
	}			

	contains(sql-drivers, psql) {
		HEADERS += $$SQL_CPP/drivers/psql/qsql_psql.h
		SOURCES += $$SQL_CPP/drivers/psql/qsql_psql.cpp
		DEFINES += QT_SQL_POSTGRES
		unix {
			!contains( LIBS, .*pq.* ) {
				LIBS *= -lpq
			}
		}
		win32 {
			!contains( LIBS, .*libpq.* ) {
				LIBS *= libpqdll.lib
			}
#			win32-msvc: { 
#				LIBS *= delayimp.lib
#				QMAKE_LFLAGS += /DELAYLOAD:libpqdll.dll
#			}
		}
	}

	contains(sql-drivers, mysql) {
		HEADERS += $$SQL_CPP/drivers/mysql/qsql_mysql.h
		SOURCES += $$SQL_CPP/drivers/mysql/qsql_mysql.cpp
		DEFINES += QT_SQL_MYSQL
		unix {
			!contains( LIBS, .*mysql.* ) {
				LIBS    *= -lmysqlclient
			}
		}
		win32 {
			!contains( LIBS, .*mysql.* ) {
				LIBS    *= libmysql.lib
			}
#			win32-msvc: { 
#				LIBS *= delayimp.lib
#				QMAKE_LFLAGS += /DELAYLOAD:libmysql.dll
#			}
		}
	}
	
	contains(sql-drivers, odbc) {
		HEADERS += $$SQL_CPP/drivers/odbc/qsql_odbc.h
		SOURCES += $$SQL_CPP/drivers/odbc/qsql_odbc.cpp
		DEFINES += QT_SQL_ODBC

		mac {
			!contains( LIBS, .*odbc.* ) {
				LIBS        *= -lodbc
			}
		}

		unix {
			!contains( LIBS, .*odbc.* ) {
				LIBS        *= -lodbc
			}
		}

		win32 {
			LIBS     *= odbc32.lib
		}

	}

	contains(sql-drivers, oci) {
		HEADERS += $$SQL_CPP/drivers/oci/qsql_oci.h
		SOURCES += $$SQL_CPP/drivers/oci/qsql_oci.cpp
		DEFINES += QT_SQL_OCI
		unix {
			!contains( LIBS, .*clnts.* ) {
			    LIBS += -lclntsh -lwtc8
			}
		}
		win32 {
			LIBS += oci.lib
#			win32-msvc: { 
#				LIBS *= delayimp.lib
#				QMAKE_LFLAGS += /DELAYLOAD:oci.dll
#			}
		}
	}

	contains(sql-drivers, tds) {
		HEADERS += $$SQL_CPP/drivers/tds/qsql_tds.h \
			   $$SQL_CPP/drivers/shared/qsql_result.h
		SOURCES += $$SQL_CPP/drivers/tds/qsql_tds.cpp \
			   $$SQL_CPP/drivers/shared/qsql_result.cpp
		DEFINES += QT_SQL_TDS
		unix {
			LIBS += -L$SYBASE/lib -lsybdb
		}
		win32 {
			LIBS += NTWDBLIB.LIB
#			win32-msvc: { 
#				LIBS *= delayimp.lib
#				QMAKE_LFLAGS += /DELAYLOAD:ntwdblib.dll
#			}
		}
	}

	contains(sql-drivers, db2) {
		HEADERS += $$SQL_CPP/drivers/db2/qsql_db2.h
		SOURCES += $$SQL_CPP/drivers/db2/qsql_db2.cpp
		DEFINES += QT_SQL_DB2
		unix {
			LIBS += -ldb2
		}
		win32 {
			LIBS += db2cli.lib
		}
	}

	contains(sql-drivers, ibase) {
                HEADERS += $$SQL_CPP/drivers/ibase/qsql_ibase.h
                SOURCES += $$SQL_CPP/drivers/ibase/qsql_ibase.cpp
                DEFINES += QT_SQL_IBASE
                unix {
                        LIBS *= -lfbclient
                }
                win32 {
			LIBS *= gds32_ms.lib
                }
	}

        contains(sql-drivers, sqlite) {
	    !contains( LIBS, .*sqlite.* ) {

                INCLUDEPATH += $$SQL_CPP/../3rdparty/sqlite/

                HEADERS += $$SQL_CPP/../3rdparty/sqlite/btree.h \
                $$SQL_CPP/../3rdparty/sqlite/config.h \
                $$SQL_CPP/../3rdparty/sqlite/hash.h \
                $$SQL_CPP/../3rdparty/sqlite/opcodes.h \
                $$SQL_CPP/../3rdparty/sqlite/os.h \
                $$SQL_CPP/../3rdparty/sqlite/pager.h \
                $$SQL_CPP/../3rdparty/sqlite/parse.h \
                $$SQL_CPP/../3rdparty/sqlite/sqlite.h \
                $$SQL_CPP/../3rdparty/sqlite/sqliteInt.h \
                $$SQL_CPP/../3rdparty/sqlite/vdbe.h \
		$$SQL_CPP/../3rdparty/sqlite/vdbeInt.h

                SOURCES += $$SQL_CPP/../3rdparty/sqlite/attach.c \
                $$SQL_CPP/../3rdparty/sqlite/auth.c \
                $$SQL_CPP/../3rdparty/sqlite/btree.c \
                $$SQL_CPP/../3rdparty/sqlite/btree_rb.c \
                $$SQL_CPP/../3rdparty/sqlite/build.c \
                $$SQL_CPP/../3rdparty/sqlite/copy.c \
		$$SQL_CPP/../3rdparty/sqlite/date.c \
                $$SQL_CPP/../3rdparty/sqlite/delete.c \
                $$SQL_CPP/../3rdparty/sqlite/expr.c \
                $$SQL_CPP/../3rdparty/sqlite/func.c \
                $$SQL_CPP/../3rdparty/sqlite/hash.c \
                $$SQL_CPP/../3rdparty/sqlite/insert.c \
                $$SQL_CPP/../3rdparty/sqlite/main.c \
                $$SQL_CPP/../3rdparty/sqlite/opcodes.c \
                $$SQL_CPP/../3rdparty/sqlite/os.c \
                $$SQL_CPP/../3rdparty/sqlite/pager.c \
                $$SQL_CPP/../3rdparty/sqlite/parse.c \
                $$SQL_CPP/../3rdparty/sqlite/pragma.c \
                $$SQL_CPP/../3rdparty/sqlite/printf.c \
                $$SQL_CPP/../3rdparty/sqlite/random.c \
                $$SQL_CPP/../3rdparty/sqlite/select.c \
                $$SQL_CPP/../3rdparty/sqlite/shell.c \
                $$SQL_CPP/../3rdparty/sqlite/table.c \
                $$SQL_CPP/../3rdparty/sqlite/tokenize.c \
                $$SQL_CPP/../3rdparty/sqlite/trigger.c \
                $$SQL_CPP/../3rdparty/sqlite/update.c \
                $$SQL_CPP/../3rdparty/sqlite/util.c \
                $$SQL_CPP/../3rdparty/sqlite/vacuum.c \
                $$SQL_CPP/../3rdparty/sqlite/vdbe.c \
		$$SQL_CPP/../3rdparty/sqlite/vdbeaux.c \
                $$SQL_CPP/../3rdparty/sqlite/where.c
	    }

            HEADERS += $$SQL_CPP/drivers/sqlite/qsql_sqlite.h
            SOURCES += $$SQL_CPP/drivers/sqlite/qsql_sqlite.cpp
            DEFINES += QT_SQL_SQLITE
        }

        contains(sql-drivers, sqlite3) {
            HEADERS += $$SQL_CPP/drivers/sqlite3/qsql_sqlite3.h
            SOURCES += $$SQL_CPP/drivers/sqlite3/qsql_sqlite3.cpp
            DEFINES += QT_SQL_SQLITE3
            unix {
                !contains( LIBS, .*sqlite3.* ) {
                    LIBS *= -lsqlite3
                }
            }
        }
}


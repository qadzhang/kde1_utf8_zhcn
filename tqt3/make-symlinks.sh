#!/bin/sh
cd include
rm -f q*.h ntq*.h
ln -s ../src/*/q*.h ../src/*/ntq*.h .
ln -s ../src/sql/drivers/*/qsql_*.h .
ln -s ../extensions/*/src/ntq*.h .
ln -s ../tools/assistant/lib/ntqassistantclient.h .
ln -s ../tools/designer/uilib/ntqwidgetfactory.h .
rm -f q*_p.h
cd private
rm -f q*_p.h tq*_p.h
ln -s ../../src/*/q*_p.h ../../src/*/tq*_p.h .

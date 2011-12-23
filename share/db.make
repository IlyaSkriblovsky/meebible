all: langs.sqlite nwt.sqlite bo.sqlite blv.sqlite ccarabic.sqlite

langs.sqlite: langs.sql
	sqlite3 $@ <$<


nwt.sqlite: nwt.sql
	sqlite3 $@ <$<


bo.sqlite: bo.sql
	sqlite3 $@ <$<


blv.sqlite: blv.sql
	sqlite3 $@ <$<


ccarabic.sqlite: ccarabic.sql
	sqlite3 $@ <$<


clean:
	rm -f langs.sqlite nwt.sqlite bo.sqlite blv.sqlite ccarabic.sqlite

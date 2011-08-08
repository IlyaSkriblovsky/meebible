all: langs.sqlite nwt.sqlite bo.sqlite

langs.sqlite: langs.sql
	sqlite3 $@ <$<


nwt.sqlite: nwt.sql
	sqlite3 $@ <$<


bo.sqlite: bo.sql
	sqlite3 $@ <$<


clean:
	rm -f langs.sqlite nwt.sqlite bo.sqlite

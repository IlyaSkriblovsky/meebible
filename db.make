all: langs.sqlite nwt.sqlite bo.sqlite

langs.sqlite: $(SRCDIR)/createlangs.sql
	sqlite3 $@ <$<


nwt.sqlite: $(SRCDIR)/createnwt.sql
	sqlite3 $@ <$<


bo.sqlite: $(SRCDIR)/createborus.sql
	sqlite3 $@ <$<


clean:
	rm -f langs.sqlite nwt.sqlite bo.sqlite

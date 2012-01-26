DBS = langs.sqlite trans.sqlite

all: $(DBS)

%.sqlite: %.sql
	sqlite3 $@ <$<

clean:
	rm -f $(DBS)

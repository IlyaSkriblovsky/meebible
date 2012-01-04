DBS = langs.sqlite nwt.sqlite bo.sqlite blv.sqlite ccarabic.sqlite kjbo.sqlite

all: $(DBS)

%.sqlite: %.sql
	sqlite3 $@ <$<

clean:
	rm -f $(DBS)

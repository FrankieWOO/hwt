.PHONY: clean

default: m2html

m2html: m2html.zip
	unzip $< &&	chown -R $(shell whoami) $@ && chmod -R +w $@ && touch m2html

clean:
	rm -rf m2html



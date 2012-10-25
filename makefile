
SUBDIRS=commModule
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Making all in $$list"; \
		(cd $$subdir && make all); \
	done;
.PHONY: clean
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir"; \
		(cd $$subdir && make clean); \
	done

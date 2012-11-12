
SUBDIRS=commModule lgModule logModule
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Making all in $$subdir"; \
		(cd $$subdir && make all); \
		echo ; \
	done;
.PHONY: clean
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir"; \
		(cd $$subdir && make clean); \
		echo ; \
	done

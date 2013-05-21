
markdown = Markdown
objects = asm-rules.html

all : $(objects)
	
    
$(objects): %.html : %.md
	$(markdown) < $< > $@
	
clean:
	rm $(objects)
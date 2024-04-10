test: src/main.ino prepare_includes
	pio run -t upload
	pio device monitor

prepare_includes:
	rm include/index.h
	cp include/template_index.h include/index.h
	sed -i '/"(/r web/index.html' include/index.h
	sed -i '/<style>/r web/style.css' include/index.h
	sed -i 's/<link rel="stylesheet" href=style.css>//' include/index.h

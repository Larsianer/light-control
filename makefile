test: src/main.cpp prepare_includes
	pio run -t upload -e d1_mini

ota: src/main.cpp prepare_includes
	pio run -t upload -e d1_mini_ota

prepare_includes:
	rm include/index.h
	cp include/template_index.h include/index.h
	sed -i '/"(/r web/index.html' include/index.h
	sed -i '/<style>/r web/style.css' include/index.h
	sed -i 's/<link rel="stylesheet" href=style.css>//' include/index.h

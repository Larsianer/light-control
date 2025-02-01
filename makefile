test: src/main.cpp
	pio run -t upload -e d1_mini

ota: src/main.cpp
	pio run -t upload -e d1_mini_ota

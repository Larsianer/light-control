test: src/main.cpp
	pio run -t upload -e d1_mini

ota_bookshelf: src/main.cpp
	sed -i '1c #define BOOKSHELF' src/main.cpp
	pio run -t upload -e d1_mini_ota --upload-port 192.168.2.182

ota_desk: src/main.cpp
	sed -i '1c #define DESK' src/main.cpp
	pio run -t upload -e d1_mini_ota --upload-port 192.168.2.189

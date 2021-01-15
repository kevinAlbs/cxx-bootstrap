configure:
	cmake -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_PREFIX_PATH=/Users/kevin.albertson/install/mongo-cxx-driver-help20788 \
		-DCMAKE_CXX_STANDARD=17 \
		-B./cmake-build -S./

bootstrap:
	cmake --build cmake-build --target bootstrap

help20788:
	cmake --build cmake-build --target help20788
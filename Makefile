mongo-config-ubuntu:
	cmake -S modules/mongo-cxx-driver -B modules/mongo-cxx-driver/build -DCMAKE_INSTALL_PREFIX="prebuild/mongocxx" -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -DCMAKE_CXX_FLAGS_INIT="-fPIC" -DCMAKE_C_FLAGS_INIT="-fPIC" -DCMAKE_BUILD_TYPE=Release

mongo-config-msvc:
	cmake -S modules/mongo-cxx-driver -B modules/mongo-cxx-driver/build -DCMAKE_INSTALL_PREFIX="prebuild/mongocxx" -DENABLE_BSONCXX_POLY_USE_IMPLS=ON -DBUILD_VERSION="3.10.2" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

mongo-install:
	cmake --build modules/mongo-cxx-driver/build -t install --config Release -j8

mongo-msvc: mongo-config-msvc mongo-install

mongo-ubuntu: mongo-config-ubuntu mongo-install

git-update:
	git fetch
	git pull

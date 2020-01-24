cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)

set(sample_dir ../src/)
set(sample_pnplib
        ${sample_dir}csv_reader.cpp
        ${sample_dir}csv_writer.cpp
        ${sample_dir}model_registration.cpp
        ${sample_dir}mesh.cpp
        ${sample_dir}model.cpp
        ${sample_dir}pnp_problem.cpp
        ${sample_dir}utils.cpp
        ${sample_dir}robust_matcher.cpp
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( pnp_registration ${sample_dir}main_registration.cpp ${sample_pnplib} )
add_executable( pnp_detection ${sample_dir}main_detection.cpp ${sample_pnplib} )

target_link_libraries( pnp_registration ${OpenCV_LIBS} )
target_link_libraries( pnp_detection ${OpenCV_LIBS} )

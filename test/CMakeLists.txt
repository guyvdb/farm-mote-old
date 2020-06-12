cmake_minimum_required(VERSION 3.5)




project(test VERSION 1.0
             DESCRIPTION "Host Test Harness"
             LANGUAGES C
)


include_directories(
  contrib/unity
)


add_executable(frame_unit_test
  contrib/unity/unity.c 
  ../components/fmframe/frame.c
  ../components/fmframe/framebuf.c
  ../components/fmframe/framelst.c
  ../components/fmframe/unittest.c
) 
                



cmake_minimum_required(VERSION 3.5)




project(test VERSION 1.0
             DESCRIPTION "Host Test Harness"
             LANGUAGES C
)


include_directories(
  contrib/unity
)


add_executable(frame_test
  contrib/unity/unity.c 
  ../components/frame/test.c
  ../components/frame/frame.c
  ../components/frame/framebuf.c
) 
                



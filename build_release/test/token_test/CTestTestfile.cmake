# CMake generated Testfile for 
# Source directory: /mnt/d/ClionProject/KaleidoscopeLanguage/test/token_test
# Build directory: /mnt/d/ClionProject/KaleidoscopeLanguage/build_release/test/token_test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(token_parser_test "/mnt/d/ClionProject/KaleidoscopeLanguage/build_release/bin/kalecc" "-i" "/mnt/d/ClionProject/KaleidoscopeLanguage/test/token_test/tokens.k" "--token_test")
set_tests_properties(token_parser_test PROPERTIES  WORKING_DIRECTORY "/mnt/d/ClionProject/KaleidoscopeLanguage/build_release/test/token_test" _BACKTRACE_TRIPLES "/mnt/d/ClionProject/KaleidoscopeLanguage/test/token_test/CMakeLists.txt;3;add_test;/mnt/d/ClionProject/KaleidoscopeLanguage/test/token_test/CMakeLists.txt;0;")

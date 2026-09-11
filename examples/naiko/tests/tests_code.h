#pragma once

inline const char* test_code_print_empty =              "";
inline const char* test_code_print_skip_whitespace =    "  foo";
inline const char* test_code_print_correct_identifier = "LET foobar = 123";
inline const char* test_code_print_digit =              "PRINT 123";
inline const char* test_code_array =                    "LET mem[123]";
inline const char* test_code_print_string =             "PRINT \"hello, world!\"";


inline const char* test_code_array_extended =           "LET mem[123]"
                                                        "SET mem[23] = 1"
                                                        "PRINT mem[23]";

inline const char* test_code_print_multiple_print =     "PRINT \"hello, world!\"\n"
                                                        "PRINT \"second line\"\n"
                                                        "PRINT \"and a third...\"\n";

inline const char* test_code_basic_let_and_set =        "LET foobar = 123\n"
                                                        "SET foobar = 321\n";

inline const char* test_code_basic_if =                 "IF 10 > 0 THEN\n"
                                                        "PRINT \"yes!\"\n"
                                                        "ENDIF\n";

inline const char* test_code_basic_operations_ext =     "LET foo = 3 - 2\n"
                                                        "LET bar = foo * 3 + 2\n";

inline const char* test_code_basic_operations =         "LET bar = 3 + 2\n"
                                                        "LET foo = bar * 3 + 2\n"
                                                        "IF foo > 0 THEN\n"
                                                            "PRINT \"yes!\"\n"
                                                        "ENDIF\n";

inline const char* test_code_big_code =                 "PRINT \"How many fibonacci numbers do you want?\"\n"
                                                        "LET nums = 123\n"
                                                        "LET a = 0\n"
                                                        "LET b = 1\n"
                                                        "WHILE nums > 0 REPEAT\n"
                                                        "    PRINT a\n"
                                                        "    LET c = a + b\n"
                                                        "    SET a = b\n"
                                                        "    SET b = c\n"
                                                        "    SET nums = nums - 1\n"
                                                        "    PRINT nums\n"
                                                        "ENDWHILE";

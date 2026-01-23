#pragma once

inline const char* test_code_print_empty =              "";
inline const char* test_code_print_skip_whitespace =    "  foo";
inline const char* test_code_print_correct_symbol =     "LET foobar = 123";
inline const char* test_code_print_digit =              "PRINT 123";
inline const char* test_code_print_string =             "PRINT \"hello, world!\"";

inline const char* test_code_print_multiple_print =     "PRINT \"hello, world!\"\n"
                                                        "PRINT \"second line\"\n"
                                                        "PRINT \"and a third...\"\n";

inline const char* test_code_basic_if =                 "IF 10 > 0 THEN\n"
                                                        "PRINT \"yes!\"\n"
                                                        "ENDIF\n";

inline const char* test_code_basic_operations =         "LET bar = 123\n"
                                                        "LET foo = bar * 3 + 2\n"
                                                        "IF foo > 0 THEN\n"
                                                            "PRINT \"yes!\"\n"
                                                        "ENDIF\n";

inline const char* test_code_big_code =                 "PRINT \"How many fibonacci numbers do you want?\"\n"
                                                        "INPUT nums\n"
                                                        "LET a = 0\n"
                                                        "LET b = 1\n"
                                                        "WHILE nums > 0 REPEAT\n"
                                                        "    PRINT a\n"
                                                        "    LET c = a + b\n"
                                                        "    LET a = b\n"
                                                        "    LET b = c\n"
                                                        "    LET nums = nums - 1\n"
                                                        "ENDWHILE";

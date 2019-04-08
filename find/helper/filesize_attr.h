//
// Created by maxlundin on 22/03/19.
//

#pragma once

#include "filesize_attr.h"

#include <iostream>
#include <string>
#include <map>

namespace filesize_attr {

    struct caseInsencitiveCMP : std::binary_function<std::string, std::string, bool> {
        struct nocase_compare : public std::binary_function<char, char, bool> {
            bool operator()(const char &c1, const char &c2) const {
                return tolower(c1) < tolower(c2);
            }
        };

        bool operator()(const std::string &s1, const std::string &s2) const {
            return std::lexicographical_compare
                    (s1.begin(), s1.end(),
                     s2.begin(), s2.end(),
                     nocase_compare());
        }
    };


    const std::pair<std::string, size_t> gigabyte = {"gb", 1024 * 1024 * 1024};
    const std::pair<std::string, size_t> megabyte = {"mb", 1024 * 1024};
    const std::pair<std::string, size_t> kilobyte = {"kb", 1024};
    const std::pair<std::string, size_t> byte = {"byte", 1};
    const std::pair<std::string, size_t> nothing = {"", 1};

    const std::map<std::string, size_t, caseInsencitiveCMP> attributes{
            gigabyte,
            megabyte,
            kilobyte,
            byte,
            nothing,
    };
}

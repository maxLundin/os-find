//
// Created by maxlundin on 22/03/19.
//

#include "filesize_attr.h"
#include "filter.h"

#include <queue>
#include <cstring>
#include <bitset>
#include <map>

std::pair<int8_t, size_t> check_if_string_is_valid_size(const std::string &str) {

    size_t pos = 1;
    size_t number = 0;

    int8_t mode = -2;
    switch (str[0]) {
        case '-':
            mode = -1;
            break;
        case '=':
            mode = 0;
            break;
        case '+':
            mode = 1;
            break;
        default:
            break;
    }

    while (isdigit(str[pos])) {
        number *= 10;
        number += str[pos++] - '0';
    }
    std::string sub;
    for (size_t i = pos; i < str.size(); ++i) {
        sub += str[i];
    }
    auto pair_attr = filesize_attr::attributes.find(sub);
    if (pair_attr == filesize_attr::attributes.end()) {
        return std::make_pair(-2, 0);
    } else {
        number *= pair_attr->second;
        return std::make_pair(mode, number);
    }
}

bool is_string_a_number(const std::string &str) {
    for (char s : str) {
        if (!isdigit(s)) {
            return false;
        }
    }
    return true;
}

file_check_argumets parse_arguments(const int args, char *argv[]) {
    int pos_arg = 2;
    std::string arg;
    file_check_argumets res{};
    while (pos_arg < args) {
        if (strcmp(argv[pos_arg], "-inum") == 0) {
            arg = std::string(argv[++pos_arg]);
            if (!is_string_a_number(arg)) {
                throw std::runtime_error("Wrong argument type for -inum \n" + arg + "is not a number");
            }
            res.inode_number = stoi(arg);
        } else if (strcmp(argv[pos_arg], "-name") == 0) {
            res.name = std::string(argv[++pos_arg]);
        } else if (strcmp(argv[pos_arg], "-size") == 0) {
            arg = std::string(argv[++pos_arg]);
            if (auto file_size_filter = check_if_string_is_valid_size(arg); file_size_filter.first == -2) {
                throw std::runtime_error("Wrong argument type for -isize \n" + arg + " is not a valid size");
            } else {
                res.mode = file_size_filter.first;
                res.size = file_size_filter.second;
            }
        } else if (strcmp(argv[pos_arg], "-nlinks") == 0) {
            arg = std::string(argv[++pos_arg]);
            if (!is_string_a_number(arg)) {
                throw std::runtime_error("Wrong argument type for -inum \n" + arg + "is not a number");
            }
            res.nlink_number = stoi(arg);
        } else if (strcmp(argv[pos_arg], "-exec") == 0) {
            res.exec_file = std::string(argv[++pos_arg]);
        }
        ++pos_arg;
    }
    return res;
}


struct stat get_info(const std::string &file_name) {
    struct stat file_stats{};
    stat(file_name.c_str(), &file_stats);
    return file_stats;
}

int main(int argc, char *argv[], const char **envp) {
    if (argc < 2) {
        std::cerr << "No arguments provided" << std::endl;
        return 1;
    }
    std::vector<std::string> output;
    file_check_argumets arg;
    try {
        arg = parse_arguments(argc, argv);
    } catch (std::runtime_error &parse_error) {
        std::cerr << parse_error.what() << std::endl;
        return 1;
    }
    std::queue<std::pair<DIR *, std::string>> queue;
    queue.push(std::make_pair(opendir(argv[1]), std::string(argv[1])));
    if (queue.front().first == nullptr) {
        std::cerr << "Directory is not valid" << std::endl;
        return 0;
    }
    if (queue.front().second.back() == '/') {
        queue.front().second.pop_back();
    }
    while (!queue.empty()) {
        auto dir = queue.front();
        queue.pop();
        if (dir.first == nullptr) {
            std::cerr << "access denied: " << dir.second << std::endl;
            continue;
        }
        dirent *entry = readdir(dir.first);
        while (entry != nullptr) {
            if (DT_DIR == entry->d_type) {
                if ((strncmp(entry->d_name, ".", PATH_MAX) == 0) ||
                    (strncmp(entry->d_name, "..", PATH_MAX) == 0)) {
                    entry = readdir(dir.first);
                } else {
                    std::string newDIR = dir.second + "/" + entry->d_name;
                    queue.push(std::make_pair(opendir(newDIR.c_str()), newDIR));
                }
            } else if (DT_REG == entry->d_type) {
                std::string filename = dir.second + '/' + entry->d_name;

                filter_t filter(filename, get_info(filename));
                if (filter.filter_all(arg, envp) && arg.exec_file.empty()) {
                    std::cout << filename << std::endl;
                }

            }
            entry = readdir(dir.first);
        }

        closedir(dir.first);
    }
}
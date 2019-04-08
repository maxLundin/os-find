#include <utility>

#include <utility>

//
// Created by maxlundin on 23/03/19.
//


#pragma once

#include "launcher.cpp"

#include <string>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>

struct file_check_argumets {
    std::string name = "";
    size_t size = 0;
    int inode_number = -1;
    int64_t nlink_number = -1;
    int8_t mode = -2;
    std::string exec_file = "";

    file_check_argumets() = default;

    file_check_argumets(std::string name,
                        const size_t size,
                        const int inode_number,
                        const int64_t nlink_number,
                        const int8_t mode,
                        std::string exec_file = "") : name(std::move(name)),
                                                      size(size),
                                                      inode_number(inode_number),
                                                      nlink_number(nlink_number),
                                                      mode(mode),
                                                      exec_file(std::move(exec_file)) {}
};


class filter_t {
public:
    filter_t(const std::string &file, struct stat file_stat) : filename(file), file_stat(file_stat) {
    }

    bool filter_all(const file_check_argumets &args, const char **m_envp) {
        bool result = filter_inode(args.inode_number)
                      && filter_name(args.name)
                      && filter_size(args.size, args.mode)
                      && filter_nlinks(args.nlink_number);
        if (!args.exec_file.empty() && result) {
            std::vector<const char *> arguments;
            arguments.push_back(args.exec_file.c_str());
            arguments.push_back(filename.c_str());
            launcher::launch(arguments, m_envp, false);
        }
        return result;
    }

    bool filter_all(const int inode_number,
                    const std::string &name,
                    const size_t size, const int8_t mode,
                    const int64_t nlink_number, const char **m_envp) {
        file_check_argumets checker(name, size, inode_number, nlink_number, mode);
        return filter_all(checker, m_envp);
    }

    bool filter_nlinks(const int64_t number) {
        if (number == -1) {
            return true;
        } else {
            return (uint32_t) number == file_stat.st_nlink;
        }
    }

    bool filter_size(const size_t size, const int8_t mode = 1) {
        if (mode == -2) {
            return true;
        }
        if (mode > 0) {
            return (int) size < file_stat.st_size;
        } else if (mode == 0) {
            return (int) size == file_stat.st_size;
        } else {
            return (int) size > file_stat.st_size;
        }
    }

    bool filter_name(const std::string &name) {
        if (name.empty()) {
            return true;
        } else {
            return filename.find(name) != std::string::npos;
        }
    }

    bool filter_inode(const int64_t inode_number) {
        if (inode_number == -1) {
            return true;
        } else {
            return (uint32_t) inode_number == file_stat.st_ino;
        }
    }

private:
    const std::string &filename;
    struct stat file_stat;
};



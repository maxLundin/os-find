//
// Created by max on 3/25/19.
//
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <cstring>

namespace launcher {
    void print_error(const std::string &msg) {
        std::cout << msg << std::endl << strerror(errno) << std::flush << std::endl;
    }

    void launchActivity(const std::vector<char *> &args1, bool out_flag = true) {
        if (args1[0] == nullptr) {
            print_error("first argument is nullptr");
            return;
        }
        pid_t ret = fork();
        if (ret < 0) {
            if (out_flag)
                print_error("Unable to fork() =(");
        } else if (ret == 0) {
            if (execvp(args1[0], args1.data()) == -1) {
                if (out_flag)
                    print_error("Execution error");
                exit(-1);
            }
            exit(0);
        } else {
            int exitStatus;
            if (waitpid(ret, &exitStatus, 0) == -1) {
                if (out_flag)
                    print_error("Error during execution");
            } else {
                if (out_flag) {
                    if (exitStatus != 0) {
                        print_error("Exit status is not 0");
                    }
                    std::cout << "Programme exited with exit code: " << exitStatus << std::endl;
                }
            }
        }
    }

    void launch(const std::vector<const char *> &args1, bool out_flag = true) {
        std::vector<char *> arg_real;
        arg_real.reserve(args1.size());
        for (const char *chars : args1) {
            arg_real.push_back(const_cast<char *>(chars));
        }
        arg_real.push_back(nullptr);
        launchActivity(arg_real, out_flag);
    }
}
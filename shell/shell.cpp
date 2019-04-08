#include "launcher.cpp"


namespace shell_OS {

    std::vector<char *> parseArgs(const std::string &stringArgs) {
        std::stringstream stream(stringArgs);
        std::string arg;
        std::vector<char *> ans;
        while (stream >> arg) {
            char *str = new char[arg.size() + 1];
            memcpy(str, arg.c_str(), arg.size() * sizeof(char));
            str[arg.size()] = '\0';
            ans.push_back(str);
        }
        ans.push_back(nullptr);
        return ans;
    }

    void hello() {
        std::cout << "~/";
        std::cout.flush();
    }

    std::string genstring(size_t size) {
        std::string empty_space;
        empty_space.reserve(size + 30);
        for (size_t s = 0; s < size + 30; ++s) {
            empty_space += " ";
        }
        return empty_space;
    }

    void cleanscr(size_t size) {
        std::string empty_space = genstring(size);
        std::cout << '\r' << empty_space << '\r';
        hello();
    }

    class controller {
    public:

        explicit controller(const char **envp) {
            while (*envp) {
                tmp_envp.push_back(*envp);
                ++envp;
            }
        }

        void lock() {
            launcher::launch({"/bin/stty", "-echo"}, tmp_envp.data(), false);
            launcher::launch({"/bin/stty", "raw"}, tmp_envp.data(), false);
            launcher::launch({"/usr/bin/setterm", "-cursor", "off"}, tmp_envp.data(), false);
        }


        void unlock() {
            launcher::launch({"/bin/stty", "echo"}, tmp_envp.data(), false);
            launcher::launch({"/bin/stty", "cooked"}, tmp_envp.data(), false);
            launcher::launch({"/usr/bin/setterm", "-cursor", "on"}, tmp_envp.data(), false);
        }

        ~controller() {
            unlock();
        }

        std::vector<const char *> tmp_envp;
    };
}

using shell_OS::cleanscr;

int main(int args, char *argv[], const char *envp[]) {
    const std::string up = {27, 91, 65};
    const std::string down = {27, 91, 66};
    const std::string right = {27, 91, 67};
    const std::string left = {27, 91, 68};
    bool exit_flag = false;

    size_t cursor_posititon = 0;

    std::vector<std::string> queue = {""};
    size_t queue_position = 1;

    while (true) {
        std::string inputString;
        cleanscr(inputString.length());
        std::cout << inputString << std::flush;
        {
            shell_OS::controller controller1(envp);
            controller1.lock();
            cursor_posititon = 0;
            while (true) {
                cleanscr(inputString.length());
                std::cout << inputString << std::flush;
                inputString.insert(inputString.begin() + cursor_posititon, getchar());
                if (cursor_posititon >= 2) {
                    if (inputString.substr(cursor_posititon - 2, 3) == up) {
                        cleanscr(inputString.length());
                        if (queue_position != 0) {
                            cleanscr(inputString.length());
                            inputString = queue[--queue_position];
                            cursor_posititon = inputString.length();
                            continue;
                        }
                        inputString.erase(inputString.begin() + cursor_posititon);
                        inputString.erase(inputString.begin() + cursor_posititon - 1);
                        inputString.erase(inputString.begin() + cursor_posititon - 2);
                        cursor_posititon -= 2;
                        continue;
                    }
                    if (inputString.substr(cursor_posititon - 2, 3) == down) {
                        cleanscr(inputString.length());
                        if (queue_position < queue.size() - 1) {
                            cleanscr(inputString.length());
                            inputString = queue[++queue_position];
                            cursor_posititon = inputString.length();
                            continue;
                        }
                        if (queue_position == queue.size() - 1) {
                            inputString = "";
                            cursor_posititon = 0;
                            continue;
                        }
                        inputString.erase(inputString.begin() + cursor_posititon);
                        inputString.erase(inputString.begin() + cursor_posititon - 1);
                        inputString.erase(inputString.begin() + cursor_posititon - 2);
                        cursor_posititon -= 2;
                        continue;
                    }
                    if (inputString.substr(cursor_posititon - 2, 3) == right) {
                        inputString.erase(inputString.begin() + cursor_posititon);
                        inputString.erase(inputString.begin() + cursor_posititon - 1);
                        inputString.erase(inputString.begin() + cursor_posititon - 2);
                        cursor_posititon -= 2;
                        if (cursor_posititon < inputString.length()) {
                            ++cursor_posititon;
                        }
                        continue;
                    }
                    if (inputString.substr(cursor_posititon - 2, 3) == left) {
//                        std::cout << std::endl << "\r" << inputString << std::endl;
                        inputString.erase(inputString.begin() + cursor_posititon);
                        inputString.erase(inputString.begin() + cursor_posititon - 1);
                        inputString.erase(inputString.begin() + cursor_posititon - 2);
                        cursor_posititon -= 2;
                        if (cursor_posititon > 0) {
                            --cursor_posititon;
                        }
                        continue;
                    }
                }
                if (inputString[cursor_posititon] == 127) {
                    inputString.erase(inputString.begin() + cursor_posititon);
                    if (inputString.length() >= 1) {
                        if (cursor_posititon != 0)
                            --cursor_posititon;
                        inputString.erase(inputString.begin() + cursor_posititon);
//                        inputString.pop_back();
                    }
                    continue;
                }
                if (inputString[cursor_posititon] == 13) {
                    inputString.erase(inputString.begin() + cursor_posititon);
                    break;
                }
                if (inputString[cursor_posititon] == 3) {
                    std::cout << std::endl << std::endl;
                    std::cout << "\rKeyboard interrupt.." << std::flush;
                    std::cout << "exiting..." << std::flush;
                    exit_flag = true;
                    break;
                }
                ++cursor_posititon;
            }

            std::cout << "\r\n" << std::flush;
        }

        if (exit_flag) {
            return 0;
        }

        queue.push_back(inputString);
        ++queue_position;

        if (inputString == "exit") {
            std::cout << "\rexiting..." << std::endl;
            exit(0);
        }
        std::vector<char *> args1 = shell_OS::parseArgs(inputString);

        launcher::launchActivity(args1, envp);

        for (const char *str : args1) {
            delete[] str;
        }

        std::cout << std::flush;
//        hello();

    }
}

#ifndef ARGPARSER_CPP
#define ARGPARSER_CPP
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

class ArgumentParser {
public:
    ArgumentParser() = default;

    // Function to add argument and its associated action (callback)
    void add_argument(const std::string& name, const std::string& help = "", bool is_flag = false, const std::string& default_value = "") {
        arguments_[name] = {help, is_flag, default_value, ""};
    }

    // Function to parse the command line arguments
    bool parse(int argc, char* argv[]) {
        bool found_input = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arguments_.find(arg) != arguments_.end()) {
                if (arguments_[arg].is_flag) {
                    parsed_flags_.insert(arg);
                } else {
                    if (i + 1 < argc) {
                        arguments_[arg].value = argv[++i];
                    } else {
                        std::cerr << "Error: Argument '" << arg << "' expects a value.\n";
                        return false;
                    }
                }
            } else {
                // If it's not a flag or argument, treat it as the input file
                if (!found_input) {
                    // Assign the first non-flag argument as the input file
                    input_file_ = arg;
                
                    found_input = true;
                }
            }
        }
        
        
        // Set default values for other arguments if not provided by the user
        for (auto& [arg, data] : arguments_) {
            if (data.value.empty() && !data.default_value.empty()) {
                data.value = data.default_value;
            }
        }
        
        return true;
    }

    // Function to check if a flag is set
    bool is_flag_set(const std::string& flag) const {
        return parsed_flags_.find(flag) != parsed_flags_.end();
    }

    // Function to get the value of an argument
    std::string get_value(const std::string& arg) const {
        if (arguments_.find(arg) != arguments_.end()) {
            return arguments_.at(arg).value;
        }
        return "";
    }

    // Function to get the input file
    std::string get_input_file() const {
        std::string input_file = this->get_value("--file");
        if(!input_file.empty()) return input_file;
        return this->input_file_;
    }

    // Function to show the help message
    void show_help() const {
        std::cout << "Usage:\n";
        for (const auto& [arg, data] : arguments_) {
            std::cout << "  " << arg;
            if (!data.is_flag) {
                std::cout << " <value>";
            }
            std::cout << " - " << data.help << "\n";
        }
    }

private:
    struct ArgumentData {
        std::string help;
        bool is_flag;
        std::string default_value;
        std::string value;
    };

    std::unordered_map<std::string, ArgumentData> arguments_;
    std::unordered_set<std::string> parsed_flags_;
    std::string input_file_;
};
#endif /*ARGPARSER_CPP*/
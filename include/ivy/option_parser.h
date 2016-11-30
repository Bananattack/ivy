#ifndef IVY_OPTION_PARSER_H
#define IVY_OPTION_PARSER_H

#include <string>
#include <vector>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <initializer_list>

namespace ivy {
    template <typename T>
    struct OptionDefinition {
        OptionDefinition() = delete;

        OptionDefinition(T type, std::string longname, bool parameterized)
        : type(type), longname(longname), shortname(0), parameterized(parameterized) {}

        OptionDefinition(T type, std::string longname, char shortname, bool parameterized)
        : type(type), longname(longname), shortname(shortname), parameterized(parameterized) {}

        T type;
        std::string longname;
        char shortname;
        bool parameterized;
    };

    template <typename T>
    struct OptionData
    {
        OptionData(T type, std::string value)
        : type(type), value(value) {}

        T type;
        std::string value;
    };

    template <typename T>
    class OptionParser
    {
        public:
            OptionParser(std::initializer_list<OptionDefinition<T>> definitions)
            : definitions(definitions) {
                init();
            }

            OptionParser(const std::vector<OptionDefinition<T>>& definitions)
            : definitions(definitions) {
                init();
            }

            std::string getError() const {
                return errorMessage;
            }

            std::vector<OptionData<T>> getOptions() const {
                return options;
            }

            bool parse(int argc, char** argv) {
                options.clear();
                errorMessage.clear();

                bool positionalMode = false;
                std::size_t activeOption = 0;

                for(int n = 1; n != argc; ++n) {
                    auto argument = argv[n];

                    if(!argument) {
                        continue;
                    }

                    if(positionalMode) {
                        options.push_back({T(), argument});
                        continue;
                    }

                    if(auto definition = getOptionDefinition(activeOption)) { 
                        activeOption = 0;

                        if(argument[0] != '-') {
                            options.push_back({definition->type, argument});
                            continue;
                        } else {
                            options.push_back({definition->type, ""});
                        }
                    }

                    if(argument[0] == '-') {
                        if(argument[1] == 0) {
                            activeOption = shortnames['-'];
                            if(auto definition = getOptionDefinition(activeOption)) {
                                if(!definition->parameterized) {
                                    activeOption = 0;
                                }
                            } else {
                                return error(std::string() + "invalid option '-'");
                            }
                        }
                        else if(argument[1] == '-') {
                            if(argument[2] == 0) {
                                positionalMode = true;
                            } else {
                                std::string key, value;
                                bool hasValue = parseLongnameArgument(argument, key, value);
                                auto it = longnames.find(key);

                                if(it != longnames.end()) {
                                    activeOption = it->second;
                                } else {
                                    activeOption = 0;
                                }

                                if(auto definition = getOptionDefinition(activeOption)) {
                                    if(definition->parameterized) {
                                        if(hasValue) {
                                            options.push_back({definition->type, value});
                                            activeOption = 0;
                                        }
                                    } else {
                                        if(hasValue) {
                                            return error(std::string() + "option '" + definition->longname + "' takes no value, but '" + argument + "' was specified");
                                        } else {
                                            options.push_back({definition->type, ""});
                                            activeOption = 0;
                                        }
                                    }
                                } else {
                                    if(hasValue) {
                                        return error(std::string() + "invalid option '--" + key + "' (specified by argument '" + argument + "')");
                                    } else {
                                        return error(std::string() + "invalid option '--" + key + "'");
                                    }
                                }
                            }
                        } else {
                            for(std::size_t i = 1, length = std::strlen(argument); i != length; ++i) {
                                activeOption = shortnames[static_cast<unsigned char>(argument[i])];

                                if(auto definition = getOptionDefinition(activeOption)) {
                                    if(definition->parameterized) {
                                        if(i != length - 1) {
                                            options.push_back({definition->type, argument + i + 1});
                                            activeOption = 0;
                                        }
                                        break;
                                    } else {
                                        options.push_back({definition->type, ""});
                                        activeOption = 0;
                                    }
                                } else {
                                    return error(std::string() + "invalid option '-" + argument[i] + "'");
                                }
                            }
                        }
                    } else {
                        options.push_back({T(), argument});
                    }
                }
                
                return true;
            }

        private:
            void init() {
                memset(shortnames, 0, sizeof(shortnames));

                for(std::size_t i = 0, size = definitions.size(); i != size; ++i) {
                    const auto& definition(definitions[i]);
                    if(definition.longname.length() != 0) {
                        longnames[definition.longname] = i + 1;
                    }
                    if(definition.shortname != 0) {
                        shortnames[static_cast<unsigned char>(definition.shortname)] = i + 1;
                    }
                }
            }

            bool error(const std::string& message) {
                errorMessage = message;
                return false;
            }

            const OptionDefinition<T>* getOptionDefinition(std::size_t index) const {
                return index != 0 ? &definitions[index - 1] : nullptr;
            }

            bool parseLongnameArgument(const char* argument, std::string& key, std::string& value) const {
                auto length = std::strlen(argument);
                auto assignmentDelimiter = std::find(argument + 2, argument + length, '=');

                if(assignmentDelimiter != argument + length) {
                    key = std::string(argument + 2, assignmentDelimiter);
                    value = std::string(assignmentDelimiter + 1);
                    return true;
                }

                key = argument + 2;
                value = "";
                return false;
            }


            std::unordered_map<std::string, std::size_t> longnames;
            std::size_t shortnames[256];
            std::vector<OptionDefinition<T>> definitions;

            std::string errorMessage;
            std::vector<OptionData<T>> options;
    };
}

#endif
#include <string>
#include <vector>

namespace sb2cpp {

/**
 * Tokenizes Small Basic code. Does not detect any syntax errors.
 * Strips all comments.
 * 
 * @param source Small Basic code.
 * @returns Vector of tokens.
*/
std::vector<std::wstring> tokenize(std::wstring const& source) {
    std::vector<std::wstring> tokens;
    std::wstring last_token, token;
    const std::wstring special = L"()[]<>+-/*=.',:";
    size_t len = source.length();
    for (int i=0; i<len; i++) {
        wchar_t c = source[i];
        if (iswspace(c)) {
            // whitespace
            if (c == L'\n' && last_token != L"\n") {
                token = L"\n";
            }
            else {
                continue;
            }
        }
        else if (c == '\'') {
            // comments
            while (i+1 < len && source[++i] != '\n');
            i--;
            continue;
        }
        else if (c == '>' || c == '<') {
            // >, <, >=, <=, <>
            token += c;
            if (i+1 < len) {
                wchar_t nextc = source[i+1];
                if ((c == '<' && nextc == '>') || nextc == '=') {
                    token += nextc;
                    i++;
                }
            }
        }
        else if (isdigit((int)c)) {
            // number
            bool parsed_dot = false;
            do { token += c; c = source[++i]; }
            while (isdigit((int)c) || (!parsed_dot && (parsed_dot = (c == L'.'))));
            i--;
        }
        else if (special.find(c) != std::wstring::npos) {
            // operators, etc.
            token += c;
        }
        else if (c == '"') {
            // strings
            int start = i;
            do token += source[i];
            while ((i+1 < len) && (source[++i] != '"') && (source[i] != '\n'));
            if (start != i && source[i] == '"') token += '"';
            else if (source[i] == '\n') i--;
        }
        else {
            // variable names, keywords, etc.
            do {
                token += source[i++];
            }
            while (i < len && (special.find(source[i]) == std::wstring::npos)
                && !iswspace(source[i]));
            i--;
        }
        tokens.push_back(token);
        last_token = token;
        token.clear();
    }
    return tokens;
}

}
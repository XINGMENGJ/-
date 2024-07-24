#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int main() {
    FILE *file = fopen("news.html", "r");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }


    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);


    char *input = (char *)malloc(file_size + 1);
    if (!input) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }

  
    if (fread(input, 1, file_size, file) != file_size) {
        fprintf(stderr, "Failed to read file\n");
        fclose(file);
        free(input);
        return 1;
    }
    fclose(file);
    input[file_size] = '\0';

    regex_t regex;
    int reti;
    regmatch_t matches[4];
    const char *rstr = "<a\\s*href=\"([^\"]*)\"><img\\s*src=\"([^\"]*)\"><div class=\"title\">([^<]*)</div>";

    
    reti = regcomp(&regex, rstr, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        free(input);
        return 1;
    }

  
    int offset = 0;
    while ((reti = regexec(&regex, input + offset, 4, matches, 0)) == 0) {
        if (matches[0].rm_so == -1) {
            break;
        }
        char href[100], img[100], title[100];
        int len;

        len = matches[1].rm_eo - matches[1].rm_so;
        strncpy(href, input + offset + matches[1].rm_so, len);
        href[len] = '\0';

        len = matches[2].rm_eo - matches[2].rm_so;
        strncpy(img, input + offset + matches[2].rm_so, len);
        img[len] = '\0';

        len = matches[3].rm_eo - matches[3].rm_so;
        strncpy(title, input + offset + matches[3].rm_so, len);
        title[len] = '\0';

        printf("网址: %s\n", href);
        printf("图像: %s\n", img);
        printf("标题: %s\n", title);

        offset += matches[0].rm_eo;
    }

    if (reti != REG_NOMATCH) {
        char error_message[100];
        regerror(reti, &regex, error_message, sizeof(error_message));
        fprintf(stderr, "失败: %s\n", error_message);
        free(input);
        regfree(&regex);
        return 1;
    }

    
    free(input);
    regfree(&regex);

    return 0;
}
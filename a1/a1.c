#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct SECTION_HEADER {
    
    char SECT_NAME[20];
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;
} SECTION_HEADER;

int max_no_of_sections = 16;

void listRecursive(const char *path, char *name_end, int flag_filter, int flag_perm_write)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("ERROR\ninvalid directory path\n");
        return;
    }

    // printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);

            if (lstat(fullPath, &statbuf) == 0)
            {
                if (flag_filter == 1)
                {
                    int length_full_name = strlen(fullPath);
                    int length_name_end = strlen(name_end);
                    int mistake = 0;

                    if (length_name_end < length_full_name)
                    {
                        for (int i = 0; i < length_name_end; i++)
                        {
                            if (fullPath[i + length_full_name - length_name_end] != name_end[i])
                            {
                                mistake++;
                            }
                        }
                        if (mistake == 0)
                        {
                            if (flag_perm_write == 0 || (flag_perm_write == 1 && (statbuf.st_mode & S_IWUSR))) {
                                    printf("%s\n", fullPath);
                            }
                                
                            
                        } 
                    }
                } else if (flag_perm_write == 0 || (flag_perm_write == 1 && (statbuf.st_mode & S_IWUSR))) {
                    printf("%s\n", fullPath);
                } 

                if (S_ISDIR(statbuf.st_mode))
                {
                    listRecursive(fullPath, name_end, flag_filter, flag_perm_write);
                }
            }
        }
    }
    closedir(dir);
}

void listUnrecursive(const char *path, char *name_end, int flag_filter, int flag_perm_write)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("ERROR\ninvalid directory path\n");
        return;
    }

    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);

            if (lstat(fullPath, &statbuf) == 0)
            {
               if (flag_filter == 1)
                {
                    int length_full_name = strlen(fullPath);
                    int length_name_end = strlen(name_end);
                    int mistake = 0;

                    if (length_name_end < length_full_name)
                    {
                        for (int i = 0; i < length_name_end; i++)
                        {
                            if (fullPath[i + length_full_name - length_name_end] != name_end[i])
                            {
                                mistake++;
                            }
                        }
                        if (mistake == 0)
                        {
                                 if (flag_perm_write == 0 || (flag_perm_write == 1 && (statbuf.st_mode & S_IWUSR))) {
                                    printf("%s\n", fullPath);
                            }
                            
                        } 
                    }
                } else if (flag_perm_write == 0 || (flag_perm_write == 1 && (statbuf.st_mode & S_IWUSR))) {
                    printf("%s\n", fullPath);
                } 
            }
        }
    }
    closedir(dir);
}

int parseSF(const char *path) {
    
    char MAGIC[2] = "";
    int HEADER_SIZE = 0;
    int VERSION = 0;
    int NO_OF_SECTIONS = 0;

    SECTION_HEADER SECTION_HEADERS[max_no_of_sections];
    
    int fd = open(path, O_RDONLY);

    if(fd == -1) {
        printf("ERROR opening the file\n");
        return 0;
    }

    lseek(fd, -1, SEEK_END);
    read(fd, &MAGIC, 1);
    MAGIC[1] = '\0';
    if(strcmp("4", MAGIC) != 0) {
        printf("ERROR\nwrong magic\n");
        return 0;
    } 

    lseek(fd, -3, SEEK_END);
    read(fd, &HEADER_SIZE, 2);
    
    lseek(fd, -HEADER_SIZE, SEEK_END);
    read(fd, &VERSION, 4);

    if(VERSION < 17 || VERSION > 109) {
        printf("ERROR\nwrong version\n");
        return 0;
    } 

    read(fd, &NO_OF_SECTIONS, 1);

    if(NO_OF_SECTIONS < 3 || NO_OF_SECTIONS > 16) {
        printf("ERROR\nwrong sect_nr\n");
        return 0;
    } 

    for(int i = 0; i < NO_OF_SECTIONS; i++) {

        SECTION_HEADERS[i].SECT_SIZE = 0;
        SECTION_HEADERS[i].SECT_OFFSET = 0;
        SECTION_HEADERS[i].SECT_TYPE = 0;
    }

    for(int i = 0; i < NO_OF_SECTIONS; i++) {

        read(fd, SECTION_HEADERS[i].SECT_NAME, 19);
        SECTION_HEADERS[i].SECT_NAME[19] = '\0';
        read(fd, &SECTION_HEADERS[i].SECT_TYPE, 2);
        read(fd, &SECTION_HEADERS[i].SECT_OFFSET, 4);
        read(fd, &SECTION_HEADERS[i].SECT_SIZE, 4);

        if(SECTION_HEADERS[i].SECT_TYPE != 94 && SECTION_HEADERS[i].SECT_TYPE != 69 && SECTION_HEADERS[i].SECT_TYPE != 74 && SECTION_HEADERS[i].SECT_TYPE != 31 && SECTION_HEADERS[i].SECT_TYPE != 67 && SECTION_HEADERS[i].SECT_TYPE != 60) {
            printf("ERROR\nwrong sect_types\n");
            return 0;     
        }
    }

    printf("SUCCESS\nversion=%d\nnr_sections=%d\n", VERSION, NO_OF_SECTIONS);
    for(int i = 0; i < NO_OF_SECTIONS; i++) {
        printf("section%d: %s %d %d\n", i + 1, SECTION_HEADERS[i].SECT_NAME, SECTION_HEADERS[i].SECT_TYPE, SECTION_HEADERS[i].SECT_SIZE);
    }
    

    close(fd);

    return 0;
}

const char* validateSF(const char *path) {
    
    char MAGIC[2] = "";
    int HEADER_SIZE = 0;
    int VERSION = 0;
    int NO_OF_SECTIONS = 0;

    SECTION_HEADER SECTION_HEADERS[max_no_of_sections];
    
    int fd = open(path, O_RDONLY);

    if(fd == -1) {
        printf("ERROR opening the file\n");
        return 0;
    }

    lseek(fd, -1, SEEK_END);
    read(fd, &MAGIC, 1);
    MAGIC[1] = '\0';
    if(strcmp("4", MAGIC) != 0) {
        printf("ERROR\nwrong magic\n");
        return 0;
    } 

    lseek(fd, -3, SEEK_END);
    read(fd, &HEADER_SIZE, 2);
    
    lseek(fd, -HEADER_SIZE, SEEK_END);
    read(fd, &VERSION, 4);

    if(VERSION < 17 || VERSION > 109) {
        printf("ERROR\nwrong version\n");
        return 0;
    } 

    read(fd, &NO_OF_SECTIONS, 1);

    if(NO_OF_SECTIONS < 3 || NO_OF_SECTIONS > 16) {
        printf("ERROR\nwrong sect_nr\n");
        return 0;
    } 

    for(int i = 0; i < NO_OF_SECTIONS; i++) {

        SECTION_HEADERS[i].SECT_SIZE = 0;
        SECTION_HEADERS[i].SECT_OFFSET = 0;
        SECTION_HEADERS[i].SECT_TYPE = 0;
    }

    for(int i = 0; i < NO_OF_SECTIONS; i++) {

        read(fd, SECTION_HEADERS[i].SECT_NAME, 19);
        SECTION_HEADERS[i].SECT_NAME[19] = '\0';
        read(fd, &SECTION_HEADERS[i].SECT_TYPE, 2);
        read(fd, &SECTION_HEADERS[i].SECT_OFFSET, 4);
        read(fd, &SECTION_HEADERS[i].SECT_SIZE, 4);

        if(SECTION_HEADERS[i].SECT_TYPE != 94 && SECTION_HEADERS[i].SECT_TYPE != 69 && SECTION_HEADERS[i].SECT_TYPE != 74 && SECTION_HEADERS[i].SECT_TYPE != 31 && SECTION_HEADERS[i].SECT_TYPE != 67 && SECTION_HEADERS[i].SECT_TYPE != 60) {
            printf("ERROR\nwrong sect_types\n");
            return 0;     
        }
    }

    close(fd);

    return 0;
}

void extractSF(const char *path, int sect_nr, int line_nr) {

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("ERROR\ninvalid file\n");
        return;
    }

    int validation_result = validateSF(fd);
    if (validation_result == -1) {
        perror("ERROR\ninvalid file\n");
        close(fd);
        return;
    }
    else if (validation_result == -2) {
        perror("ERROR\ninvalid section\n");
        close(fd);
        return;
    }

    SECTION_HEADER section_header;
    if (lseek(fd, section_offset[sect_nr - 1], SEEK_SET) == -1 || read(fd, &section_header, sizeof(SECTION_HEADER)) != sizeof(SECTION_HEADER)) {
        perror("ERROR\ninvalid file|section|line\n");
        close(fd);
        return;
    }

    if (section_header.SECT_TYPE == 0x0001) {
        int offset = section_offset[sect_nr - 1] + section_header.SECT_OFFSET;
        int line_count = 1;

        char c;
        int bytes_read = pread(fd, &c, 1, offset);
        while (bytes_read == 1 && line_count != line_nr) {
            if (c == '\n') {
                line_count++;
            }
            offset++;
            bytes_read = pread(fd, &c, 1, offset);
        }

        if (line_count == line_nr && bytes_read == 1) {
            
            char line[512];
            int line_offset = offset + 1;
            int line_length = 0;
            bytes_read = pread(fd, &c, 1, line_offset);
            while (bytes_read == 1 && c != '\n') {
                line[line_length++] = c;
                bytes_read = pread(fd, &c, 1, ++line_offset);
            }
            line[line_length] = '\0';

            printf("SUCCESS\n%s\n", line);
            close(fd);
            return;
        }
    }

    perror("ERROR\ninvalid file|section|line\n");
    close(fd);
}


int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("61482\n");
        }

        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "list") == 0)
            {
                const char *path = NULL;
                char *string_end = NULL;
                int flag_recursive = 0;
                int flag_filter_name = 0;
                int flag_perm_write = 0;

                for (int j = 1; j < argc; j++)
                {
                    if (strncmp(argv[j], "path=", 5) == 0)
                    {
                        path = argv[j] + 5;
                        break;
                    }
                }
                if (path == NULL)
                {
                    printf("No argument found to match the path= \n");
                    return 1;
                }
                for (int k = 0; k < argc; k++)
                {
                    if (strcmp(argv[k], "recursive") == 0)
                    {
                        flag_recursive = 1;
                    }
                }
                for (int j = 0; j < argc; j++)
                {
                    if (strncmp(argv[j], "name_ends_with=", 15) == 0)
                    {
                        flag_filter_name = 1;
                        string_end = argv[j] + 15;
                    }
                }

                for (int j = 0; j < argc; j++)
                {
                    if (strcmp(argv[j], "has_perm_write") == 0)
                    {
                        flag_perm_write = 1;
                    }
                }

                struct stat sb;
                if (lstat(path, &sb) == -1)
                {
                    perror("lstat");
                    return -1;
                }

                if (!S_ISDIR(sb.st_mode))
                {
                    return -1;
                }

                if (flag_perm_write && ((sb.st_mode & S_IWUSR) == 0))
                {
                    return -1;
                }

                if (flag_recursive == 1)
                {
                    printf("SUCCESS\n");
                    listRecursive(path, string_end, flag_filter_name, flag_perm_write);
                }
                else
                {
                    listUnrecursive(path, string_end, flag_filter_name, flag_perm_write);
                }
            }


            if (strcmp(argv[i], "parse") == 0) {

                const char *path = NULL;

                for (int j = 1; j < argc; j++)
                {
                    if (strncmp(argv[j], "path=", 5) == 0)
                    {
                        path = argv[j] + 5;
                        break;
                    }
                }
                if (path == NULL)
                {
                    printf("No argument found to match the path= \n");
                    return 1;
                }

                parseSF(path);
            }

            if (strcmp(argv[i], "extract") == 0) {

                int section_number = 0;
                int line_number = NULL;

                const char *path = NULL;

                for (int j = 1; j < argc; j++)
                {
                    if (strncmp(argv[j], "path=", 5) == 0)
                    {
                        path = argv[j] + 5;
                        break;
                    } else if (strncmp(argv[j], "section=", 8) == 0) {

                    }
                }
                if (path == NULL)
                {
                    printf("No argument found to match the path= \n");
                    return 1;
                }


                for (int j = 0; j < argc; j++)
                {
                    if (strncmp(argv[j], "section=", 8) == 0)
                    {
                        section_number = argv[j] + 8;
                    } else if (strncmp(argv[j], "line=", 5) == 0) {
                        line_number = argv[j] + 5;
                    }
                }

                extractSF(path, section_number, line_number);
            }
        }
    }
    return 0;
}

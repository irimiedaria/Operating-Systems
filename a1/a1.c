#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

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
        }
    }
    return 0;
}

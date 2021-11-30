#include <bits/stdc++.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
using namespace std;

int const TABLE_MAX_PAGES = 1024;
int const PAGE_SIZE = 4096;

typedef struct
{
    int file_descriptor;
    void *pages[TABLE_MAX_PAGES];

} Pager;

Pager *pager_open(const char *filename)
{
    int fd = open(filename,
                  O_RDWR |     // Read/Write mode
                      O_CREAT, // Create file if it does not exist
                  S_IWUSR |    // User write permission
                      S_IRUSR  // User read permission
    );

    if (fd == -1)
    {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager *pager = (Pager *)malloc(sizeof(Pager));
    pager->file_descriptor = fd;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pager->pages[i] = NULL;
    }
    return pager;
}

void *get_page(Pager *pager, uint32_t page_num)
{
    if (page_num > TABLE_MAX_PAGES)
    {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
               TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL)
    {
        // Cache miss. Allocate memory and load from file.
        void *page = malloc(PAGE_SIZE);

        lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
        ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
        if (bytes_read == -1)
        {
            printf("Error reading file: %d\n", errno);
            exit(EXIT_FAILURE);
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void pager_flush(Pager *pager, uint32_t page_num, uint32_t size)
{
    if (pager->pages[page_num] == NULL)
    {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if (offset == -1)
    {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);

    if (bytes_written == -1)
    {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void wrapUp(Pager *pager)
{
    off_t file_length = lseek(pager->file_descriptor, 0, SEEK_END);
    for (uint32_t i = 0; i < file_length / PAGE_SIZE; i++)
    {
        if (pager->pages[i] == NULL)
        {
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }
    int result = close(pager->file_descriptor);
    if (result == -1)
    {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        void *page = pager->pages[i];
        if (page)
        {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
}

// void serialize_row(Row* source, void* destination) {
//   memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
//   memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
//   memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
// }

// void deserialize_row(void *source, Row* destination) {
//   memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
//   memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
//   memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
// }

void addTable(string name, vector<pair<string, string>> metadata)
{
    int numColumn = metadata.size();
    Pager *pager = pager_open("tables");
    off_t file_length = lseek(pager->file_descriptor, 0, SEEK_END);
    void *page = get_page(pager, file_length / PAGE_SIZE);
    memcpy(page + file_length / PAGE_SIZE, &numColumn, sizeof(numColumn));
    off_t columnSize = 16;
}
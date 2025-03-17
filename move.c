#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/**
  ***************************************************************************
  *ssize_t read(int fd, void *buffer, size_t count);
  *         - Returns number of bytes read, 0 on EOF, or –1 on error
  ***************************************************************************
  *ssize_t write(int fd, void *buffer, size_t count);
  *         - Returns number of bytes written, or –1 on error
  ***************************************************************************
  *int rmdir(const char *pathname);
  *         - Returns 0 on success, or –1 on error
  ***************************************************************************
  *int unlink(const char *pathname);
  *         - Returns 0 on success, or –1 on error
  */

#include <sys/stat.h>
/**
  ***************************************************************************
  *int stat(const char *pathname, struct stat *statbuf);
  *         - Return 0 on success, or –1 on error
  ***************************************************************************
  *int mkdir(const char *pathname, mode_t mode);
  *         - Returns 0 on success, or –1 on error
  */

#include <fcntl.h>
/**
  ***************************************************************************
  *int open(const char *pathname, int flags, ... "mode_t mode");
  *         - Returns file descriptor on success, or –1 on error
  *
  ***************************************************************************
  *
  */
#include <dirent.h>
/**
  ***************************************************************************
  *DIR *opendir(const char *dirpath);
  *         - Returns directory stream handle, or NULL on error
  ***************************************************************************
  *int closedir(DIR *dirp);
  *         - Returns 0 on success, or –1 on error
  ***************************************************************************
  *struct dirent *readdir(DIR *dirp);
  *         - Returns pointer to a statically allocated structure describing
  *         next directory entry, or NULL on end-of-directory or error
  *         - struct dirent {
  *             ino_t d_ino; // File i-node number 
  *             char d_name[]; // Null-terminated name of file
  *           };
  */

#include <libgen.h>
#include <errno.h>


/**/
#define BUF_SIZE 1024
/**
 * @brief Kiểm tra xem đường dẫn có phải là thư mục không.
 * @param[in] path Đường dẫn cần kiểm tra.
 * @return 1 nếu là thư mục, 0 nếu không hoặc lỗi.
 */
int mv_isDirectory(const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        return 0;
    }

    return S_ISDIR(statbuf.st_mode);
}

/**
 * @brief Sao chép nội dung từ tệp nguồn sang tệp đích.
 * @param[in] srcPath Đường dẫn tệp nguồn.
 * @param[in] destPath Đường dẫn tệp đích.
 * @return 0 nếu sao chép thành công, -1 nếu có lỗi.
 */
int mv_copyFile(const char *srcPath, const char *destPath) {
    int srcFd, destFd;
    char buffer[BUF_SIZE];
    ssize_t bytesRead, bytesWritten;

    /* Open source files */
    srcFd = open(srcPath, O_RDONLY); // Open the file for writing only

    if (srcFd == -1) {
        perror("[ERROR]: OPENING SOURCE FILE IN THE COPY FILE");
        return -1;
    }

    struct stat srcStatus;
    if (stat(srcPath, &srcStatus) < 0) {
        perror("[ERROR]: RETRIEVING SOURCE FILE INFORMATION IN THE COPY FILE");
        close(srcFd);
        return -1;
    }
    destFd = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, srcStatus.st_mode & 0777);
    if (destFd < 0) {
        perror("[ERROR]: OPENING DESTINATION FILE IN THE COPY FILE");
        close(srcFd);
        return -1;
    }

    while ((bytesRead = read(srcFd, buffer, BUF_SIZE)) > 0) { 
        bytesWritten = write(destFd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            perror("[ERROR]: WRITING TO DESTINATION FILE IN THE COPY FILE");
            close(srcFd);
            close(destFd);
            return -1;
        }
    }
    
    /* EOF 0 OR ERROR READ -1 */
    if (bytesRead < 0) { // -1 ERROR READ
        perror("[ERROR]: READING SOURCE FILE IN THE COPY FILE");
    }

    /* Close source and destination*/
    close(srcFd);
    close(destFd);

    /* Success */
    return 0; 
}

/**
 * @brief Sao chép thư mục từ nguồn sang đích bằng cách đệ quy.
 * @param[in] srcPath Đường dẫn thư mục nguồn.
 * @param[in] destPath Đường dẫn thư mục đích.
 * @return 0 nếu sao chép thành công, -1 nếu có lỗi.
 */
 int mv_copyDirectory(const char *srcPath, const char *destPath) {
    struct dirent *dirEntryPath;
    DIR *dirp = opendir(srcPath);

    if (dirp == NULL) {
        perror("[ERROR]: OPENING SOURCE DIRECTORY IN THE COPY DIRECTORY");
        return -1;
    }

    struct stat srcStatus;
    if (stat(srcPath, &srcStatus) < 0) {
        perror("[ERROR] RETRIVING SOURCE DIRECTORY INFORMATION IN THE COPY DIRECTORY");
        closedir(dirp);
        return -1;
    }

    if (mkdir(destPath, srcStatus.st_mode & 0777) < 0 && errno != EEXIST) {
        perror("[ERROR] CREATING DESTINATION DIRECTORY IN THE COPY DIRECTORY");
        closedir(dirp);
    }

    while ((dirEntryPath = readdir(dirp)) != NULL) {
        if (strcmp(dirEntryPath->d_name, ".") == 0 || strcmp(dirEntryPath->d_name, "..") == 0) {
            continue; /* Skip . and .. */
        }

        char srcEntryPath[1024];
        char destEntryPath[1024];
        snprintf(srcEntryPath, sizeof(srcEntryPath), "%s/%s", srcPath, dirEntryPath->d_name);
        snprintf(destEntryPath, sizeof(destEntryPath), "%s/%s", destPath, dirEntryPath->d_name);

        if (mv_isDirectory(srcEntryPath)) {
            if (mv_copyDirectory(srcEntryPath, destEntryPath) == -1) {
                closedir(dirp);
                return -1;
            }
        } else {
            if (mv_copyFile(srcEntryPath, destEntryPath) == -1) {
                closedir(dirp);
                return -1;
            }
        }
    }

    closedir(dirp);
    return 0;
 }

/**
 * @brief Xóa file hoặc thư mục bằng đệ quy.
 * @param[in] path Đường dẫn file hoặc thư mục cần xóa.
 * @return 0 nếu thành công, -1 nếu có lỗi.
 */

int mv_deleteFileOrDirectory(const char *path) {
    struct stat pathStatus;

    // if (stat(path, pathStatus) < 0) {
    //     perror("[ERROR]: RETRIEVING FILE OR DIRECTORY INFORMATION IN THE DELETE FILE OR DIRECTORY")
    //     return -1;
    // }

    if (mv_isDirectory(path)) {
        struct dirent *dirEntryPath;
        DIR *dirp = opendir(path);
        if (dirp == NULL) {
            perror("[ERROR]: OPENING DIRECTORY IN THE DELETE FILE OR DIRECTORY");
            return -1;
        }

        while ((dirEntryPath = readdir(dirp)) != NULL) {
            if (strcmp(dirEntryPath->d_name, ".") == 0 || strcmp(dirEntryPath->d_name, "..") == 0) {
                continue;
            }

            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, dirEntryPath->d_name);

            if (mv_deleteFileOrDirectory(fullPath) == -1) {
                closedir(dirp);
                return -1;
            }
        }

        closedir(dirp);

        if (rmdir(path) == -1) { // In order for rmdir() to succeed, the directory must be empty
            perror("[ERROR]: REMOVING DIRECTORY IN THE DELETE FILE OR DIRECTORY");
            return -1;
        }
    } else {
        /*The unlink() system call removes a link (deletes a filename) */
        if (unlink(path) == -1) {
            perror("[ERROR]: REMOVING FILE IN THE DELETE FILE OR DIRECTORY");
            return -1;
        }
    }

    
    return 0;
    

}

/**
 * @brief Di chuyển file hoặc thư mục từ source đến destination.
 * @param[in] srcPath Đường dẫn nguồn.
 * @param[in] destPath Đường dẫn đích.
 * @return 0 nếu thành công, -1 nếu có lỗi.
 */
 int mv_move(const char *srcPath, const char *destPath) {
    struct stat srcStatus;
    struct stat destStatus;

    if (stat(srcPath, &srcStatus) < 0) {
        perror("[ERROR]: RETRIEVING SOURCE FILE OR DIRECTORY INFORMATION IN THE MOVE");
        return -1;
    }

    // Nếu destination tồn tại, kiểm tra loại tệp
    if (stat(destPath, &destStatus) == 0) {
        // Nếu source là thư mục và dest là file, báo lỗi
        if (S_ISDIR(srcStatus.st_mode) && !S_ISDIR(destStatus.st_mode)) {
            fprintf(stderr, "[ERROR]: CANNOT MOVE THE DIRECTORY INTO FILE IN THE MOVE\n");
            return -1;
        }

        // Nếu dest là thư mục, ghép tên file vào dest
        if (S_ISDIR(destStatus.st_mode)) {
            char newDest[1024];
            // snprintf(newDest, sizeof(newDest), "%s/%s", destPath, strrchr(srcPath, '/') + 1);
            snprintf(newDest, sizeof(newDest), "%s/%s", destPath, basename((char *)srcPath));
            destPath = newDest;
        }
    }

     // Nếu source là file, dùng rename hoặc copy & unlink
    if (S_ISREG(srcStatus.st_mode)) {
        if (rename(srcPath, destPath) == 0) {
            return 0;
        } else if (errno == EXDEV) {
            // Nếu khác filesystem, cần copy rồi xóa
            if (mv_copyFile(srcPath, destPath) == 0) {
                return unlink(srcPath);
            } else {
                return -1;
            }
        } else {
            perror("[ERROR]: CANNOT MOVE THE FILE IN THE MOVE");
            return -1;
        }
    }

    // Nếu source là thư mục, cần copy toàn bộ nội dung rồi xóa thư mục
    if (S_ISDIR(srcStatus.st_mode)) {
        if (mv_copyDirectory(srcPath, destPath) == 0) {
            return mv_deleteFileOrDirectory(srcPath);
        } else {
            return -1;
        }
    }

    fprintf(stderr, "[ERROR]: THIS FILE TYPE IS NOT SUPPORTED IN THE MOVE\n");
    return -1;
}

 int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc - 1; i++) {
        if (mv_move(argv[i], argv[argc - 1]) == -1) {
            fprintf(stderr, "Error: Failed to move %s to %s\n", argv[i], argv[argc - 1]);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

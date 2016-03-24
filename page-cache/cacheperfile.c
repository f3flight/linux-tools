#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
int main (int argc, char *argv[]) {
    if(argc < 2) {
        printf("Shows page cache ratio and size for provided files.\nUsage: ./cacheperfile filename [filename...] | column -t\n");
        return 0;
    }
    int pagesize = sysconf(_SC_PAGESIZE);
    setlocale(LC_ALL, "");
    printf("filename\tsize(bytes)\tpages\tcached\tpercent\tcached(bytes)\n");
    while(argc-- > 1) {
        argv++;
        struct stat st;
        int fd = open(argv[0], O_RDONLY);
        fstat(fd, &st);

        void *pp = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        int howmuch = (st.st_size + pagesize -1) / pagesize;
        char here[howmuch];
        memset(here, 0, howmuch);
        mincore(pp, st.st_size, here);
        munmap(pp, st.st_size);
        close(fd);

        printf("%s\t", argv[0]);
        int sum=0, i=0;
        for(i=0; i < howmuch; i++){
            sum += here[i]&1;
        }
        printf("%.0f\t%d\t%d\t%.2f\t%.0f\n", (float)st.st_size, howmuch, sum, sum/(float)howmuch, sum/(float)howmuch*(float)st.st_size);
//        printf(" length: %.2f ;; pagesize: %d\n", (float)st.st_size, sysconf(_SC_PAGESIZE));
    }
}

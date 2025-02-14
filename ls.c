#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <limits.h>



// ANSI escape codes for colors
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define PINK "\033[0;35m"
#define max(x,y) ( (x>y) ?( x ):( y ) )
#define RESET "\033[0m"  





#define MAX_SIZE 1024

#define MAX_FILES 1024
#define MAX_PATH 1024
extern int errno;

typedef struct filee{
	char name[MAX_SIZE];
	struct stat stat;
}FileInfo;
 

int greatest_strlen(FileInfo files_name[],int size){
	int maxx=INT_MIN;
	int l;
	for(int i=0;i<size;i++){
		l=strlen(files_name[i].name);
		maxx=max(l,maxx);
	}
	return maxx;
}


static int cmpstringp(const void *p1, const void *p2) {
    return strcmp(*(const char **) p1, *(const char **) p2);
}

char is_directory(mode_t mode){
	return ( mode & S_IFDIR ) ? ('d') : ('-');
}

char is_elf(mode_t mode){
	return ( (mode & S_IXUSR) && (mode & S_IXGRP) ) ? ('x'): ('-');
}
int compare_mod_time(const void *a, const void *b) {
    return ((FileInfo *)b)->stat.st_mtime - ((FileInfo *)a)->stat.st_mtime;
}

int compare_acc_time(const void *a, const void *b) {
    return ((FileInfo *)b)->stat.st_atime - ((FileInfo *)a)->stat.st_atime;
}

int compare_chg_time(const void *a, const void *b) {
    return ((FileInfo *)b)->stat.st_ctime - ((FileInfo *)a)->stat.st_ctime;
}



void do_ls(char *dir,int show_all,int list_long,int sort_by_mod_time,int sort_by_acc_time,int sort_by_chg_time,int only_files,int only_dirs,int one_per_line,int inode_numbers);
void print_permissions(mode_t mode);
void print_file_info(FileInfo *file_info,int inode_numbers);



int main(int argc, char* argv[]) {
   int list_long = 0;
    int show_all = 0;
    int sort_by_mod_time = 0;
    int sort_by_acc_time = 0;
    int sort_by_chg_time = 0;
    int only_files = 0;
    int only_dirs = 0;
    int one_per_line = 0;
    int inode_numbers=0;

    int opt;
    while ((opt = getopt(argc, argv, "latucifd1")) != -1) {
        switch (opt) {
            case 'l': list_long = 1; break;
            case 'a': show_all = 1; break;
            case 't': sort_by_mod_time = 1; break;
            case 'u': sort_by_acc_time = 1; break;
            case 'c': sort_by_chg_time = 1; break;
            case 'f': only_files = 1; break;
            case 'd': only_dirs = 1; break;
	    case 'i': inode_numbers=1; break;      
            case '1': one_per_line = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-latucifd1] [directories...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        do_ls(".", show_all, list_long, sort_by_mod_time, sort_by_acc_time, sort_by_chg_time, only_files, only_dirs, one_per_line,inode_numbers);
    } else {
        for (int i = optind; i < argc; i++) {
            do_ls(argv[i], show_all, list_long, sort_by_mod_time, sort_by_acc_time, sort_by_chg_time, only_files, only_dirs, one_per_line,inode_numbers);
        }
    }

    return 0;


}

void do_ls(char *dir,int show_all,int list_long,int sort_by_mod_time,int sort_by_acc_time,int sort_by_chg_time,int only_files,int only_dirs,int one_per_line,int inode_numbers) {
    struct dirent *entry;
    
    FileInfo files[MAX_SIZE]= {NULL};
    
    int file_counter = 0;
    char *files_name[MAX_FILES] = {NULL};
    int total_blocks=0;  
    DIR *dp = opendir(dir);
    
    if (dp == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return;
    }

    errno = 0;
    while ((entry = readdir(dp)) != NULL) {
    
     	if (errno != 0) {
            perror("readdir failed");
            closedir(dp);
            exit(1);
        }
     		if (!show_all && entry->d_name[0] == '.') continue;
        	if (only_dirs && entry->d_type != DT_DIR) continue;
        	if (only_files && entry->d_type != DT_REG) continue;

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);

	struct stat st;
        if (stat(full_path, &st) == -1) {
            perror("stat");
            continue;
        }

        strncpy(files[file_counter].name, entry->d_name, sizeof(files[file_counter].name));
        files[file_counter].stat = st;

        files_name[file_counter] = strdup(entry->d_name);

       
       
        if (file_counter >= MAX_FILES) {
            fprintf(stderr, "Too many files\n");
            break;
        }

        if (files_name[file_counter] == NULL) {
            perror("strdup failed");
            closedir(dp);
            exit(1);
        }
                file_counter++;       
    }

    closedir(dp);
    
	

    if (sort_by_mod_time) {
        qsort(files, file_counter, sizeof(FileInfo), compare_mod_time);
    } else if (sort_by_acc_time) {
        qsort(files, file_counter, sizeof(FileInfo), compare_acc_time);
    } else if (sort_by_chg_time) {
        qsort(files, file_counter, sizeof(FileInfo), compare_chg_time);
    }else{
   	qsort(files_name, file_counter, sizeof(char *), cmpstringp);
    }

    
    int max_len=greatest_strlen(files,file_counter)+3;
    
    
    int file_in_oneline=0;

    if (list_long) {
    	for(int i=0;i<file_counter;i++){
    		total_blocks+=files[i].stat.st_blocks;
    	}
    	printf("total: %d \n",total_blocks/2);
        for (int i = 0; i < file_counter; i++) {
            print_file_info(&files[i] , inode_numbers);
        }
    }

    else {
        if (one_per_line) {
                for (int i = 0; i < file_counter; i++) {
 			
            		if(inode_numbers) {
            			printf("%ld  " ,files[i].stat.st_ino);
        		}
     	           	if(is_directory(files[i].stat.st_mode)=='d'){
     				printf(BLUE "%s" RESET, files[i].name); 
     				              
                	}
                	else if(is_elf(files[i].stat.st_mode)=='x'){
     				printf(GREEN "%s" RESET, files[i].name);
     		        }
                	else{
                	printf(PINK "%s" RESET, files[i].name);                 
                	}
                	
                	printf("\n");    
                
            }
        } else{
            	for (int i = 0; i < file_counter; ) {
             	    for(int j=0;j<6;j++){
             	
        			if(inode_numbers) {
            				printf("%ld  " ,files[i].stat.st_ino);
                		}
                	
                		if( is_directory(files[i].stat.st_mode) == 'd' ){
     					printf(BLUE "%-*s" RESET,max_len, files[i].name);   
                		}
                		else if( is_elf(files[i].stat.st_mode) == 'x'){
     					printf(GREEN "%-*s" RESET, max_len,files[i].name);   
               			}else{
                			printf(PINK "%-*s" RESET,max_len,files[i].name);                 
                		}
                		i++;
                	}
        	    	printf("\n");
                    }
               }
    }

   
}


void print_file_info(FileInfo *file_info,int inode_numbers) {
    struct tm *timeinfo;
    char time_buf[256];
    
    if(inode_numbers){
    	printf("%ld ",file_info->stat.st_ino);
    }
    print_permissions(file_info->stat.st_mode);
    
    printf(" %ld", file_info->stat.st_nlink);
    
    struct passwd *pwd = getpwuid(file_info->stat.st_uid);
    struct group *grp = getgrgid(file_info->stat.st_gid);
    if (pwd) {
        printf(" %s", pwd->pw_name);
    } else {
        printf(" %d", file_info->stat.st_uid);
    }
    
    if (grp) {
        printf(" %s", grp->gr_name);
    } else {
        printf(" %d", file_info->stat.st_gid);
    }
    printf(" %5ld", file_info->stat.st_size);

    timeinfo = localtime(&file_info->stat.st_mtime);
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", timeinfo);
    printf(" %s\t", time_buf);

    if( is_directory(file_info->stat.st_mode) == 'd' ){
	printf(BLUE "%s\t" RESET, file_info->name);   

    }
    else if( is_elf(file_info->stat.st_mode) == 'x'){
     	printf(GREEN "%s\t" RESET, file_info->name);   

    }else{
     	printf(PINK "%s\t" RESET, file_info->name);                 
    }
    printf("\n");
}



void print_permissions(mode_t mode) {

    printf("%c%c%c%c%c%c%c%c%c%c",
           ((S_ISDIR(mode) )? ('d') : (S_ISCHR(mode) )? ('c') : ( S_ISBLK(mode) ) ? ( 'b' ): (S_ISLNK(mode)) ? ('s') :('-')),
           (mode & S_IRUSR) ? 'r' : '-',
           (mode & S_IWUSR) ? 'w' : '-',
           (mode & S_IXUSR) ? 'x' : '-',
           (mode & S_IRGRP) ? 'r' : '-',
           (mode & S_IWGRP) ? 'w' : '-',
           (mode & S_IXGRP) ? 'x' : '-',
           (mode & S_IROTH) ? 'r' : '-',
           (mode & S_IWOTH) ? 'w' : '-',
           (mode & S_IXOTH) ? 'x' : '-');
}


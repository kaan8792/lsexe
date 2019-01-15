#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "jrb.h"
#include "dllist.h"
void lsexe(char *fn, JRB inodes, int *maxlen)
{
	DIR *d;
	struct dirent *de;
	struct stat buf;
	int exists;
	char *s;

	Dllist directories, tmp;
	directories = new_dllist();

	d = opendir(fn); //Dizini d değişkenine atama işlemi.
	if (d == NULL) {
		perror("prsize");
		exit(1);
	}

	s = (char *) malloc(sizeof(char)*(strlen(fn)+258)); //Tam yolları tutacak olan değişken yeterli miktarda yer ayırma işlemi.
	
	for (de = readdir(d); de != NULL; de = readdir(d)) { //Açılan dizindeki dosyaların bilgilerini sırası ile de değişkenine atar.
		sprintf(s, "%s/%s", fn, de->d_name); //Seçilen dosyanın tam yolunu belirler. "./.." gibis
		exists = lstat(s, &buf);
		if (exists < 0) {
			fprintf(stderr, "Couldn't stat %s\n", s);
		} else {
			char iname[50];
			sprintf(iname, "%d %s", buf.st_ino,de->d_name);
			if (jrb_find_str(inodes, iname) == NULL) { //Daha önceden aynı dosya jrbye eklenmişmi kontrolü.
				/* Programın kilit noktası çalıştırılabilir dosyaların st_mode nosu 33261 ve 33277 dir burada sadece onları seçer.*/
				if (buf.st_mode == 33261 || buf.st_mode == 33277){
					char size[10];
					sprintf(size, "%d", buf.st_size);
					if (strlen(iname) > *maxlen){*maxlen = strlen(iname);}
					jrb_insert_str(inodes, strdup(iname), new_jval_s(strdup(size))); //İf bloğunun en iç katmanında jrbye ekler.
				}
			}
		}
		/* Alt dizinleri belirleyip listeye ekleme işlemi */
		if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
		dll_append(directories, new_jval_s(strdup(s)));
		}
	}
	closedir(d);
	dll_traverse(tmp, directories) { //Özyinelemeli olarak olarak alt kalsörleri çağırır.
		lsexe(tmp->val.s, inodes , maxlen);
		free(tmp->val.s);
	}

	free_dllist(directories);
	free(s);
}

int main()
{
	int maxlen=0;
	JRB inodes,tmp;
	inodes = make_jrb();
	lsexe(".", inodes, &maxlen); // Fonksiyonun çağırılması
	
	jrb_traverse(tmp, inodes) { //Fonksiyonda jrbye eklenen bilgileri innode göre sıralı olarak ekrana yazma işlemi.
		printf("%*s %s\n",-maxlen , tmp->key.s,tmp->val.s);
	}
}
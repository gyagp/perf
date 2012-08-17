/* This program preloads elf format binaries designated in parameters 
 * Author: Yang Gu (gyagp0@gmail.com)
 * 
 */

#define _GNU_SOURCE
#include <elf.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
	int i, j;
	char buf[BUF_SIZE];
	int fd;
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	Elf32_Half phnum;
	size_t ra_end = 0;

	for (i = 1; i < argc; i++) {
		fd = open(argv[i], O_RDONLY);
		if (fd == -1)
			continue;

		if (read(fd, buf, BUF_SIZE) <= 0)
			continue;

		ehdr = (Elf32_Ehdr *)buf;
		
		/* Check elf header to see if it's a valid ELF32 binary */
		if (memcmp(ehdr, ELFMAG, 4) || 
				(ehdr->e_ident[EI_CLASS] != ELFCLASS32))
			goto error;
		
		phnum = ehdr->e_phnum;

		/* Check if we have read enough data */
		if (ehdr->e_phoff + ehdr->e_phentsize * phnum > BUF_SIZE)
			goto error;

		phdr = (Elf32_Phdr *)&buf[ehdr->e_phoff];

		/*
		 * Only the segments with type PT_LOAD need to be preloaded.
		 * The advantage of this is some unnecessary section, such as
		 * debug, would not be read so that the total time consumption 
		 * can be reduced.
		 * Program header table is read to know range of these segments.
		 */
		for (j = 0; j < phnum; j++, phdr++) {
			if (phdr->p_type != PT_LOAD)
				continue;

			if (phdr->p_offset + phdr->p_filesz > ra_end)
				ra_end = phdr->p_offset + phdr->p_filesz;
		}

		if (ra_end > 0)
		    readahead(fd, 0, ra_end);

error:
		close(fd);
	}

	return 0;
}

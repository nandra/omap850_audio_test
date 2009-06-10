/***************************************************************************
 *   Copyright (C) 2009 by open-nandra                                     *
 *   marek.belisko@open-nandra.com                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define WAV_RIFF     ('R' | ('I' << 8) | ('F' << 16) | ('F' << 24))
#define WAV_WAVE     ('W' | ('A' << 8) | ('V' << 16) | ('E' << 24))
#define WAV_FMT      ('f' | ('m' << 8) | ('t' << 16) | (' ' << 24))
#define WAV_DATA     ('d' | ('a' << 8) | ('t' << 16) | ('a' << 24))

#define WAV_HDR_SIZE          44
#define WAV_DATA_LEN_OFFSET   (WAV_HDR_SIZE - 4)

#define read_little_end_word_short(p) \
   ( (*((p) + 0) << 0) | (*((p) + 1) << 8) )

#define read_little_end_word(p) \
   ( (*((p) + 0) << 0) | (*((p) + 1) << 8)  | (*((p) + 2) << 16) | (*((p) + 3) << 24))


unsigned char  hdr[WAV_HDR_SIZE];

unsigned char audio_samples [8192];

unsigned char check_wav_header(unsigned char *hdr, unsigned long *size)
{
	unsigned char *p = NULL;
	unsigned char ret = 0;
	p = hdr;
   	
   	if (read_little_end_word(p) != WAV_RIFF) {
		printf("RIFF\n");
		goto end;
      	}
   	p += 8;
   
   	if (read_little_end_word(p) != WAV_WAVE) {
		printf("WAV_WAVE\n");
      		goto end;
   	}
   	p += 4;
      
   	if (read_little_end_word(p) != WAV_FMT) {
		printf("FORMAT\n");
      		goto end;
	}
   
   	p += 4;

   	if (read_little_end_word(p) != 16) {       // expect standard fmt chunk (length: 16 bytes)
		printf("Length\n");
      		goto end;
	}

   	p += 4;
   
  	p += 2;

   	p += 2;
   
   	p += 4;   

   	p += 4;
      
   	p += 2;

   	p += 2;
   
	if (read_little_end_word(p) != WAV_DATA) {
      		printf("DATA\n");
		goto end;
	}
   
   	p += 4;
   
   	*size = read_little_end_word_short(p);

   	if (*size == 0)
       		goto end;
	ret = 1;
end: 
	
	return ret;

}


int main (int argc, char **argv)
{
	int ret = 0;
	unsigned long size_global;
	unsigned long size = 8192;
	int count;
	int fd, fd_wav;
	const char * const device = "/dev/audio";
	int result;
	unsigned char buf[4];

	if (argc <= 1) {
		printf("wav file missisng\n");
		goto end;
	}
	/* open wav file */
	printf("%s\n", argv[1]);		
	fd_wav = open(argv[1], O_RDONLY);
	if (fd_wav < 0) {
		perror("open");
		goto end;	
	}
	if (read(fd_wav, hdr, sizeof(hdr)) != sizeof(hdr)) {
		perror("read");
		goto end_close;
	}
	if (!check_wav_header(hdr, &size_global)) {
		printf("Error header decoding\n");
		goto end_close;
	}
	
	fd = open(device, O_WRONLY);
	if (fd < 0) {
		perror("open");
		goto end;
	}
	
	while (1) {
		
		result = read(fd_wav, audio_samples, size);
		switch (result) {
		case 0:
			printf("End of file rewind\n");
			if (lseek(fd_wav, WAV_DATA_LEN_OFFSET, SEEK_SET) != WAV_DATA_LEN_OFFSET)
				goto end_close;

			if (read(fd_wav, buf, sizeof(buf)) != sizeof(buf))
				goto end_close;
			size_global = read_little_end_word(buf);
			if (size_global == 0)
				goto end_close;
			break;
		default:
			printf("Readed OK  size :%d \n", result);
			break;
		} 
// 		sleep(5);
		
	
		count = write(fd, audio_samples, sizeof(audio_samples));
		if (count <= 0 ) {
			perror("write");
			break;
		} else {
			printf("Data successfully written\n");
		} 
	}
end_close:	
	close(fd_wav);
	close(fd);
		
end:
	return ret;
}

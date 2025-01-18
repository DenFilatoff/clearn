#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t read_file_byte(FILE *fp){
  uint8_t datar;
  fread(&datar, 1, 1, fp);
  return datar;
}

 typedef struct answer {
int encoding;
char inFileName[512] ;
char outFileName[512]  ;
};

typedef struct koir {
unsigned short koi_code;
unsigned short  utf_1;
unsigned short  utf_2;
};

typedef struct kutf {
unsigned short  utf_1;
unsigned short  utf_2;
};




struct kutf find_KOI2UTF (unsigned short val   ) {
struct kutf ret = {0, 0};
unsigned short kc;
 struct koir koirArr[]    = {
{225 , 208 , 144} ,
{226 , 208 , 145} ,
{247 , 208 , 146} ,
{231 , 208 , 147} ,
{228 , 208 , 148} ,
{229 , 208 , 149} ,
{246 , 208 , 150} ,
{250 , 208 , 151} ,
{233 , 208 , 152} ,
{234 , 208 , 153} ,
{235 , 208 , 154} ,
{236 , 208 , 155} ,
{237 , 208 , 156} ,
{238 , 208 , 157} ,
{239 , 208 , 158} ,
{240 , 208 , 159} ,
{242 , 208 , 160} ,
{243 , 208 , 161} ,
{244 , 208 , 162} ,
{245 , 208 , 163} ,
{230 , 208 , 164} ,
{232 , 208 , 165} ,
{227 , 208 , 166} ,
{254 , 208 , 167} ,
{251 , 208 , 168} ,
{253 , 208 , 169} ,
{255 , 208 , 170} ,
{249 , 208 , 171} ,
{248 , 208 , 172} ,
{252 , 208 , 173} ,
{224 , 208 , 174} ,
{241 , 208 , 175} ,
{193 , 208 , 176} ,
{194 , 208 , 177} ,
{215 , 208 , 178} ,
{199 , 208 , 179} ,
{196 , 208 , 180} ,
{197 , 208 , 181} ,
{214 , 208 , 182} ,
{218 , 208 , 183} ,
{201 , 208 , 184} ,
{202 , 208 , 185} ,
{203 , 208 , 186} ,
{204 , 208 , 187} ,
{205 , 208 , 188} ,
{206 , 208 , 189} ,
{207 , 208 , 190} ,
{208 , 208 , 191} ,
{210 , 209 , 128} ,
{211 , 209 , 129} ,
{212 , 209 , 130} ,
{213 , 209 , 131} ,
{198 , 209 , 132} ,
{200 , 209 , 133} ,
{195 , 209 , 134} ,
{222 , 209 , 135} ,
{219 , 209 , 136} ,
{221 , 209 , 137} ,
{223 , 209 , 138} ,
{217 , 209 , 139} ,
{216 , 209 , 140} ,
{220 , 209 , 141} ,
{192 , 209 , 142} ,
{209 , 209 , 143} ,
{179 , 208 , 129} ,
{163 , 209 , 145} };
for (int i =0; i < 65; i++ ) {
kc = (unsigned short)koirArr[i].koi_code;
if (kc == val) {
ret.utf_1 = koirArr[i].utf_1;
ret.utf_2 = koirArr[i].utf_2;
  break;
   }

}
return ret;
}


struct answer getAnswer() {
struct answer ans;
printf("Please input number of encoding of output file: \n  1.CP1251 \n 2.KOI8R \n  3.ISO8859 \n ");
scanf(" %d", &ans.encoding);
printf("Please input full path of input file: \n");
scanf("%s", ans.inFileName);
printf("Please input full path of output file: \n");
scanf("%s", ans.outFileName);
return ans;
}


void write_file_byte(FILE *fpout, uint8_t data){
  fwrite(&data, 1, 1, fpout);
}

void write_CP1251_2_UTF (char inFileName[], char outFileName[] ) {
FILE *fpin;
FILE *fpout;
fpin = fopen(inFileName, "rb");
fpout = fopen(outFileName, "w");
   	if(fpout == NULL) {
		printf("file can't be opened\n");
		exit(1);
	}

unsigned short sample;
  fseek(fpin, 0, SEEK_END);
  fseek(fpout, 0, SEEK_END);
  long tam = ftell(fpin);
  rewind(fpin);
  rewind(fpout);
  long c = 0;
  while(c!=tam){
    sample = read_file_byte(fpin);
     if ( sample >= 192  && sample <= 239)  { write_file_byte(fpout,(char)208); write_file_byte(fpout,(char)(sample-48));  }
              else if (sample > 239)  { write_file_byte(fpout, (char)209); write_file_byte(fpout,(char)(sample-48)); }
              else if (sample ==184) { write_file_byte(fpout,(char)209); write_file_byte(fpout,(char)209);}
              else if (sample  == 168)   { write_file_byte(fpout,(char)208); write_file_byte(fpout,(char)129);}
               else  { write_file_byte(fpout, (char)sample );}

    c++;
  }
  fclose(fpin);
  fclose(fpout);
}


void write_KOI8R_2_UTF (char inFileName[], char outFileName[]) {
FILE *fpin;
FILE *fpout;
fpin = fopen(inFileName, "rb");
fpout = fopen(outFileName, "w");
   	if(fpout == NULL) {
		printf("file can't be opened\n");
		exit(1);
	}

unsigned short sample;
  fseek(fpin, 0, SEEK_END);
  fseek(fpout, 0, SEEK_END);
struct kutf s_out;
  long tam = ftell(fpin);
  rewind(fpin);
  rewind(fpout);
  long c = 0;
  while(c!=tam){
    sample = read_file_byte(fpin);
    if (sample > (unsigned short)127) {
     s_out = find_KOI2UTF ( (unsigned short)sample ) ;
    write_file_byte(fpout, (char)s_out.utf_1); write_file_byte(fpout, (char)s_out.utf_2);
    }
    else write_file_byte(fpout, (char)sample);
    c++;
  }
  fclose(fpin);
  fclose(fpout);
}


void write_ISO8859_2_UTF (char inFileName[], char outFileName[]) {
FILE *fpin;
FILE *fpout;
fpin = fopen(inFileName, "rb");
fpout = fopen(outFileName, "w");
   	if(fpout == NULL) {
		printf("file can't be opened\n");
		exit(1);
	}

unsigned short sample;
  fseek(fpin, 0, SEEK_END);
  fseek(fpout, 0, SEEK_END);
  long tam = ftell(fpin);
  rewind(fpin);
  rewind(fpout);
  long c = 0;
  while(c!=tam){
    sample = read_file_byte(fpin);

    if (sample >127) {
   if (sample >= 176 && sample <= 223) {write_file_byte(fpout,(char)208); write_file_byte(fpout,(char)(sample-32));  }
   else if (sample >= 224 && sample <= 239) {write_file_byte(fpout,(char)209); write_file_byte(fpout,(char)(sample-96));  }
   else if (sample == 161){write_file_byte(fpout,(char)208); write_file_byte(fpout,(char)(129));  }
   else if (sample == 241){write_file_byte(fpout,(char)209); write_file_byte(fpout,(char)(143));  }
     }  else { write_file_byte(fpout, (char)sample ); }
    c++;
  }
  fclose(fpin);
  fclose(fpout);
}


int main() {

struct answer uans;
uans =  getAnswer() ;
printf("number of encoding: %d \n", uans.encoding);
printf("full path of input file: %s \n", uans.inFileName);
printf("number of encoding of output file: %s \n", uans.outFileName);

    switch ( uans.encoding)  {
     case 1:
       write_CP1251_2_UTF(uans.inFileName, uans.outFileName);
       break;
     case 2:
       write_KOI8R_2_UTF (uans.inFileName, uans.outFileName);
       break;
     case 3:
       write_ISO8859_2_UTF (uans.inFileName, uans.outFileName);
       break;
     default:
       printf("No such encoding.");
       break;
     }
}

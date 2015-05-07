/* CalcRuntime 
 * by Foone Turing <foone@foone.org>
 * This code is under the zlib license, see copying.txt or visit 
 * http://www.opensource.org/licenses/zlib-license.php for details.
 * 
 * This is not the cleanest code I've ever written, but it shouldn't be TOO hard to figure out what's going on.
 */


#include <stdio.h>
#include <string.h>
#include "main.h"
#include <vector>
#include <stdlib.h>
#include <string>

std::vector<NECS> necs;
bool Quiet=false;
bool Verbose=false;
double mult98=6;
double multXP=6.25f;
bool UseXP=false;
ulint scenecount=0;

int main(int argc,char*argv[]){
	ProcessOptions(argc,argv);
	unsigned long int frames;
	frames=CalcFrames(argv[1]);
	if(frames!=0){
		double seconds=CalcRunTime(frames);
		if(!Quiet){
			printf("%u scenes.\n",scenecount);
			printf("%u frames.\n",frames);
			
			if(seconds<60){
				printf("%0.2f seconds.\n",seconds);
			}else if(seconds<3600){
				printf("%0.2f minutes. ",seconds/60.0f);
				printf("(%0.2f seconds)\n",seconds);
			}else{
				printf("%0.2f hours. ",seconds/3600.0f);
				printf("(%0.2f minutes) ",seconds/60.0f);
				printf("(%0.2f seconds)\n",seconds);
			}
			if(UseXP)printf("\n(Lengths adjusted for XP timebug)\n");
		}else{
			printf("%u:%u:%0.2f\n",scenecount,frames,seconds);
		}
	}else{
		printf("0:0:0\n");
	}
	system("pause");
return 0;
}

double CalcRunTime(unsigned long int frames){
	if(UseXP){
		return (double)frames/multXP;
	}else{
		return (double)frames/mult98;
	}
}

void ProcessOptions(int argc,char *argv[]){
	bool help=false;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"-q")==0)Quiet=true;
		if(strcmp(argv[i],"-v")==0)Verbose=true;
		if(strcmp(argv[i],"-xp")==0)UseXP=true;
		if(strcmp(argv[i],"/?")==0){argc=0;help=true;}
		
		if(strcmp(argv[i],"-h")==0){argc=0;help=true;}
		if(strcmp(argv[i],"--help")==0){argc=0;help=true;}
	}
	if(!Quiet)printf("CalcRuntime 1.2 by Travis Wells <cr@3dmm2.com>\n");
	if(argc<2){
		printf("Usage:\nCalcRuntime <file.3mm> [-q][-v][-xp]\n(Or drag a file onto me)\n");
		if(help){
			printf("-q\tQuiet/compact output\n");
			printf("-v\tVerbose output\n");
			printf("-xp\tCalculate using XP's frame rate.\n");
		}
		system("pause");
		exit(0);
	}
	//printf("Verbose output enabled\n");
}

ulint CalcFrames(const char *filename){
	FILE *fp=fopen(filename,"rb");
	if(!fp){
		std::string temp=filename;
		temp+=".3mm";
		fp=fopen(temp.c_str(),"rb");
		if(!fp){
			printf("Couldn't open!\n");
			return 0;
		}
		printf("Loading [%s].\n",temp.c_str());
	}else{
		printf("Loading [%s].\n",filename);
	}
	ulint temp;int ret;
	ret=fread(&temp,sizeof(ulint),1,fp);
	if(ret!=1){
		printf("Couldn't read file!\n");
		fclose(fp);
		return 0;
	}
	if(temp!=843991107){
		printf("Not a 3dmm file, or corrupt.\n");
		fclose(fp);
		return 0;
	}
	fseek(fp,20,SEEK_SET);
	ulint index_start;
	ulint index_length;
	ret=fread(&index_start,sizeof(ulint),1,fp);
	ret+=fread(&index_length,sizeof(ulint),1,fp);
	if(ret!=2){
		printf("Couldn't read file!\n");
		fclose(fp);
		return 0;
	}
	u8*index=new u8[index_length];
	fseek(fp,index_start,SEEK_SET);
	ret=fread(index,sizeof(u8),index_length,fp);
	if(ret<0 || (ulint)ret!=index_length){
		printf("Couldn't read index!\n");
		delete [] index;
		fclose(fp);
		return 0;
	}
	ulint i;
	int lastnext=-1;
	for(i=0;i<index_length-16;i++){
		if(b8toulint(&index[i])==1396917582){
			NECS n;
			n.pos=i;
			n.firstafter=b8toulint(&index[i+4]);
			n.nextafter=b8toulint(&index[i+8]);

			if(n.nextafter==(ulint)(lastnext+1)){
				lastnext=n.nextafter;
				continue;
			}
			lastnext=n.nextafter;
			if(Verbose)printf("Found at %u: [%u][%u]\n",i,n.firstafter,n.nextafter);
			necs.push_back(n);
		}
	}
	delete [] index;
	ulint frames=0;
	for(i=0;i<necs.size();i++){
		ulint pos=necs[i].nextafter;
		fseek(fp,pos,SEEK_SET);
		ret=fread(&temp,sizeof(ulint),1,fp);
		if(ret!=1 || temp!=50528257)continue;
		ret=fread(&temp,sizeof(ulint),1,fp);
		frames+=temp;
		scenecount++;
	}
	fclose(fp);
	return frames;
}

ulint b8toulint(u8*c){
return *((ulint*)c);
}
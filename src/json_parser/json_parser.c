#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <json_parser.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>



void _parse_json_str(json_parser_state_t* p,string_32bit_t* o){
	o->l=1;
	o->v=malloc(sizeof(char));
	*(o->v)=0;
	char c=**p;
	(*p)++;
	while (c!='\"'){
		if (c!='\\'){
			o->l++;
			o->v=realloc(o->v,o->l*sizeof(char));
			*(o->v+o->l-2)=c;
		}
		else{
			c=**p;
			(*p)++;
			if (c=='/'||c=='\\'||c=='\''||c=='\"'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=c;
			}
			else if (c=='b'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=8;
			}
			else if (c=='f'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=12;
			}
			else if (c=='n'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=10;
			}
			else if (c=='r'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=13;
			}
			else if (c=='t'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=9;
			}
			else if (c=='v'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=11;
			}
			else if (c=='x'){
				char a=**p;
				(*p)++;
				char b=**p;
				(*p)++;
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-2)=((a>47&&a<58?a-48:(a>64&&a<71?a-55:a-87))<<4)|(b>47&&b<58?b-48:(b>64&&b<71?b-55:b-87));
			}
			else{
				printf("Unknown Escape: \\%c\n",c);
			}
		}
		c=**p;
		(*p)++;
	}
	o->l--;
	*(o->v+o->l)=0;
}



uint8_t parse_json(json_parser_state_t* p,json_object_t* o){
	char c=**p;
	(*p)++;
	while (c==' '||c=='\t'||c=='\n'||c=='\r'){
		c=**p;
		(*p)++;
	}
	if (c=='{'){
		o->t=JSON_OBJECT_TYPE_MAP;
		o->dt.m.l=0;
		o->dt.m.dt=NULL;
		while (1){
			while (c!='\"'){
				c=**p;
				(*p)++;
			}
			o->dt.m.l++;
			o->dt.m.dt=realloc(o->dt.m.dt,o->dt.m.l*sizeof(json_map_keypair_t));
			json_map_keypair_t* k=o->dt.m.dt+o->dt.m.l-1;
			_parse_json_str(p,&(k->k));
			c=**p;
			(*p)++;
			while (c!=':'){
				c=**p;
				(*p)++;
			}
			if (parse_json(p,&(k->v))){
				return 1;
			}
			c=**p;
			(*p)++;
			while (c!=','){
				if (c=='}'){
					return 0;
				}
				c=**p;
				(*p)++;
			}
		}
	}
	if (c=='['){
		o->t=JSON_OBJECT_TYPE_ARRAY;
		o->dt.a.l=0;
		o->dt.a.dt=NULL;
		while (1){
			o->dt.a.l++;
			o->dt.a.dt=realloc(o->dt.a.dt,o->dt.a.l*sizeof(json_object_t));
			if (parse_json(p,o->dt.a.dt+o->dt.a.l-1)){
				return 1;
			}
			c=**p;
			(*p)++;
			while (c!=','){
				if (c==']'){
					return 0;
				}
				c=**p;
				(*p)++;
			}
		}
	}
	if (c=='\"'){
		o->t=JSON_OBJECT_TYPE_STRING;
		_parse_json_str(p,&(o->dt.s));
		return 0;
	}
	if (c=='t'&&**p=='r'&&*((*p)+1)=='u'&&*((*p)+2)=='e'){
		(*p)+=3;
		o->t=JSON_OBJECT_TYPE_TRUE;
		return 0;
	}
	if (c=='f'&&**p=='a'&&*((*p)+1)=='l'&&*((*p)+2)=='s'&&*((*p)+3)=='e'){
		(*p)+=4;
		o->t=JSON_OBJECT_TYPE_FALSE;
		return 0;
	}
	if (c=='n'&&**p=='u'&&*((*p)+1)=='l'&&*((*p)+2)=='l'){
		(*p)+=3;
		o->t=JSON_OBJECT_TYPE_NULL;
		return 0;
	}
	int8_t s=1;
	if (c=='-'){
		s=-1;
		c=**p;
		(*p)++;
	}
	double v=0;
	while (c>47&&c<58){
		v=v*10+(c-48);
		c=**p;
		(*p)++;
	}
	if (c!='.'&&c!='e'&&c!='E'){
		(*p)--;
		o->t=JSON_OBJECT_TYPE_INTEGER;
		o->dt.i=(int64_t)(v*s);
		return 0;
	}
	if (c=='.'){
		double pw=0.1;
		c=**p;
		(*p)++;
		while (c>47&&c<58){
			v+=pw*(c-47);
			pw*=0.1;
			c=**p;
			(*p)++;
		}
	}
	if (c=='e'||c=='E'){
		c=**p;
		(*p)++;
		int8_t pw_s=1;
		if (c=='+'){
			c=**p;
			(*p)++;
		}
		else if (c=='-'){
			c=**p;
			(*p)++;
			pw_s=-1;
		}
		int64_t pw=0;
		while (c>47&&c<58){
			pw=pw*10+(c-48);
			c=**p;
			(*p)++;
		}
		pw*=pw_s;
		v*=pow(2,(double)pw)*pow(5,(double)pw);
		(*p)--;
	}
	(*p)--;
	o->t=JSON_OBJECT_TYPE_FLOAT;
	o->dt.f=v*s;
	return 0;
}



json_object_t* get_by_key(json_object_t* json,const char* k){
	for (uint32_t i=0;i<json->dt.m.l;i++){
		json_map_keypair_t* e=json->dt.m.dt+i;
		for (uint32_t j=0;j<e->k.l+1;j++){
			if (*(e->k.v+j)!=*(k+j)){
				goto _check_next_key;
			}
		}
		return &(e->v);
_check_next_key:;
	}
	return NULL;
}



void write_json(json_object_t* json,string_32bit_t* o){
	if (json->t==JSON_OBJECT_TYPE_ARRAY){
		o->l++;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-1)='[';
		for (uint32_t i=0;i<json->dt.a.l;i++){
			if (i){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-1)=',';
			}
			write_json(json->dt.a.dt+i,o);
		}
		o->l++;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-1)=']';
	}
	else if (json->t==JSON_OBJECT_TYPE_FALSE){
		o->l+=5;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-5)='f';
		*(o->v+o->l-4)='a';
		*(o->v+o->l-3)='l';
		*(o->v+o->l-2)='s';
		*(o->v+o->l-1)='e';
	}
	else if (json->t==JSON_OBJECT_TYPE_FLOAT){
		char tmp[64];
		int sz=sprintf(tmp,"%lg",json->dt.f);
		uint32_t j=o->l;
		o->l+=sz;
		o->v=realloc(o->v,o->l*sizeof(char));
		for (uint8_t i=0;i<sz;i++){
			*(o->v+j)=tmp[i];
			j++;
		}
	}
	else if (json->t==JSON_OBJECT_TYPE_INTEGER){
		int64_t v=json->dt.i;
		if (v<0){
			o->l++;
			o->v=realloc(o->v,o->l*sizeof(char));
			*(o->v+o->l-1)='-';
			v=-v;
		}
		char bf[20];
		uint8_t i=0;
		do{
			bf[i]=v%10;
			i++;
			v/=10;
		} while (v);
		uint32_t j=o->l;
		o->l+=i;
		o->v=realloc(o->v,o->l*sizeof(char));
		while (i){
			i--;
			*(o->v+j)=bf[i]+48;
			j++;
		}
	}
	else if (json->t==JSON_OBJECT_TYPE_MAP){
		o->l++;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-1)='{';
		for (uint32_t i=0;i<json->dt.m.l;i++){
			if (i){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+o->l-1)=',';
			}
			json_map_keypair_t* e=json->dt.m.dt+i;
			uint32_t j=o->l+1;
			o->l+=e->k.l+3;
			o->v=realloc(o->v,o->l*sizeof(char));
			*(o->v+j-1)='\"';
			for (uint32_t k=0;k<e->k.l;k++){
				*(o->v+j+k)=*(e->k.v+k);
			}
			*(o->v+o->l-2)='\"';
			*(o->v+o->l-1)=':';
			write_json(&(e->v),o);
		}
		o->l++;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-1)='}';
	}
	else if (json->t==JSON_OBJECT_TYPE_NULL){
		o->l+=4;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-4)='n';
		*(o->v+o->l-3)='u';
		*(o->v+o->l-2)='l';
		*(o->v+o->l-1)='l';
	}
	else if (json->t==JSON_OBJECT_TYPE_STRING){
		uint32_t i=o->l+1;
		o->l+=json->dt.s.l+2;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+i-1)='\"';
		for (uint32_t j=0;j<json->dt.s.l;j++){
			uint8_t c=*(json->dt.s.v+j);
			if (c>31&&c<127){
				*(o->v+i)=c;
				i++;
			}
			else if (c=='\b'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='b';
				i+=2;
			}
			else if (c=='\f'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='f';
				i+=2;
			}
			else if (c=='\n'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='n';
				i+=2;
			}
			else if (c=='\r'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='r';
				i+=2;
			}
			else if (c=='\t'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='t';
				i+=2;
			}
			else if (c=='\v'){
				o->l++;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='v';
				i+=2;
			}
			else{
				o->l+=3;
				o->v=realloc(o->v,o->l*sizeof(char));
				*(o->v+i)='\\';
				*(o->v+i+1)='x';
				*(o->v+i+2)=(c>>4)+((c>>4)>9?87:48);
				*(o->v+i+3)=(c&15)+((c&15)>9?87:48);
				i+=4;
			}
		}
		*(o->v+o->l-1)='\"';
	}
	else if (json->t==JSON_OBJECT_TYPE_TRUE){
		o->l+=4;
		o->v=realloc(o->v,o->l*sizeof(char));
		*(o->v+o->l-4)='t';
		*(o->v+o->l-3)='r';
		*(o->v+o->l-2)='u';
		*(o->v+o->l-1)='e';
	}
}



void free_json(json_object_t* json){
	if (json->t=JSON_OBJECT_TYPE_STRING){
		if (json->dt.s.v){
			free(json->dt.s.v);
		}
	}
	else if (json->t==JSON_OBJECT_TYPE_ARRAY){
		for (uint32_t i=0;i<json->dt.a.l;i++){
			free_json(json->dt.a.dt+i);
		}
		if (json->dt.a.dt){
			free(json->dt.a.dt);
		}
	}
	else if (json->t==JSON_OBJECT_TYPE_MAP){
		for (uint32_t i=0;i<json->dt.m.l;i++){
			json_map_keypair_t* e=json->dt.m.dt+i;
			if (e->k.v){
				free(e->k.v);
			}
			free_json(&(e->v));
		}
		if (json->dt.m.dt){
			free(json->dt.m.dt);
		}
	}
}

#include <json_parser.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc,const char** argv){
	const char* dt="{\"key\":[\"value\",10,0.555,-842.9e-50,{\"a\":[\"c\",30,\"d€\",\"\\n\x01\"],\"other\":\"else\"},true,false,null,{},[]]}";
	json_object_t json;
	json_parser_state_t p=(json_parser_state_t)dt;
	if (parse_json(&p,&json)){
		printf("Error: Offset %u: %s\n",p-1-dt,p-1);
		return 1;
	}
	string_32bit_t o={
		0,
		NULL
	};
	write_json(&json,&o);
	o.v=realloc(o.v,(o.l+1)*sizeof(char));
	*(o.v+o.l)=0;
	printf("<%u> %s\n",o.l,o.v);
	free(o.v);
	free_json(&json);
	return 0;
}

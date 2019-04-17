#ifndef _ONENET_H_
#define _ONENET_H_


//记得修改
#define DEVID	"520735430"
//记得修改
#define APIKEY	"wQwOfRj211ng53xZS=8HRKymCkk="
//这里是在网上添加了masterkey 不知道正确与否

#define Heart_OK 1
#define Heart_err 0




enum data_type{data_stream=0,picture,heart,Cmd};


//需要上傳的數據流
typedef struct
{
	unsigned char humidt;
	unsigned char temp;
	unsigned char body_temp;
	unsigned char heart_beats;
	unsigned char blood_oxygen;
	unsigned char beep;
	unsigned char warning;
	
}my_data_stream;



_Bool OneNet_DevLink(void);

void OneNet_SendData(my_data_stream *my_data_STREAM);
void OneNet_RevPro(unsigned char *cmd);

void OneNet_SendData_Picture(char *devid,  char * pic_name);
void Send_heart(void);

#endif

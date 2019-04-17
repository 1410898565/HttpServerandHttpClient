/*****************************************************************
 ** �ļ��� : http.h
 ** 2015-2017 �����˿Ƽ����Źɷ����޹�˾ Co.Ltd
 
 ** ������: ������
 ** ��  ��: 2016-11-xx
 ** ��  ��: httpӦ�ó���ͷ�ļ�
 ** ��  ��: V1.0

 ** �޸���:
 ** ��  ��:
 ** �޸�����:
 ** ��  ��: 
******************************************************************/

#ifndef __HTTPCLIENT_H
#define __HTTPCLIENT_H	 

#ifdef __cplusplus 
extern "C" { 
#endif

/*****************************************************************
* ����ͷ�ļ�(���������Ҫ��H�ļ��������������H�ļ�)
******************************************************************/


/*****************************************************************
* �궨��
******************************************************************/
#define HTTP_STRING							"http://"
#define HTTP_POST_ONE_SHOT_PIC_MEM			0

#define HTTP_BODY 						0  //http����
#define HTTP_HEAD						1  //httpͷ


/*****************************************************************
* �ṹ��ö�ٶ���
******************************************************************/
typedef struct _stHttpUserInfo
{
	int nOperation;				//��������
	int ucFlag;					//��������ͷ���������ݲ���
	int nSedSize;				//�������ݴ�С	
	int nRcvBufLen;				//����������
	int nRcvSize;				//����body���ݴ�С
	
	char *pRcvBuffer;			//ָ���û������洢���ݵ�buf
	char *pSendBuffer;			//ָ���û��������ݵ�buf
}stHttpUserInfo;



typedef struct _stHttpClientInfo
{
	int nClientPort;			//http�����������˿ں�
	char chClientIP[32];		//http������IP
	char chClientPath[64];		//http�������ļ�·��
	char chUserName[32];		//http��������¼�û�
	char chUserPassWord[32];	//http��������¼����
	char chUrl[128];			//http������url

	stHttpUserInfo stHttpUserHead;
	stHttpUserInfo stHttpUserBody;
}stHttpClientInfo;


/*****************************************************************
* ȫ�ֱ�������
******************************************************************/

/*****************************************************************
* ����ԭ������
******************************************************************/
int httpClient_httpPost(stHttpClientInfo *pstUserArg,long *pRespCode);
int httpClient_httpGet(stHttpClientInfo *pstUserArg,long *pRespCode);


#ifdef __cplusplus
}
#endif

#endif


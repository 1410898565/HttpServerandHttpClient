/*****************************************************************
 ** �ļ��� �� httpserver.h
 ** 2015-2017 �����˿Ƽ����Źɷ����޹�˾ Co.Ltd
 
 ** ������: �����
 ** ��  ��: 2019-04-13
 ** ��  ��: httpserverģ��
 ** ��  ��: V1.0

 ** �޸���:
 ** ��  ��:
 ** �޸�����:
 ** ��  ��: 
******************************************************************/

#ifndef __HTTPSERVER_H
#define __HTTPSERVER_H	 
/*****************************************************************
* ͷ�ļ�
******************************************************************/


/*****************************************************************
* �궨��
******************************************************************/
#define HTTP_RCV_BUFFER_SIZE (4*1024*1024)

#define MYHTTPD_SIGNATURE		"GxxHttpd v0.1"

/*****************************************************************
* �ṹ����(���ڵ�ǰC�ļ�ʹ�õĽṹ��д�ڵ�ǰC�ļ��У�������д��H�ļ���)
******************************************************************/
typedef int (*fnHttpCallback)(void *pst,void *pArg0,void *pArg1);


typedef struct _stHttpServerInfo
{
	int httpd_ListenPort;
	int httpd_timeout;
	char httpd_ListenIP[32];

	/*http�ص�����*/
	fnHttpCallback faceInfoDelFuc;		/*������Ϣ����ص�����*/
	fnHttpCallback keepAliveDelFunc; 	/*��������ص�����*/
	fnHttpCallback defaultDelFunc;		/*ͨ�ô���ص�����*/	
	fnHttpCallback respondDelFunc;		/*��������Ӧ�ص�����*/

	/*����ָ���ض�����ص�������ƥ���*/
	char httpd_KeepAliveMatchStr[32];// ��"/faceinfo"
	char httpd_FaceInfoMatchStr[32];	// ��"/keepalive"
	
	int httpd_RcvBufSize;
	char http_RcvBuffer[HTTP_RCV_BUFFER_SIZE];

	
	struct evhttp *httpd;	//һ��http server
	void *pUserStArg;		//ָ��user�ṹ��
}stHttpServer;



/*****************************************************************
* ����ԭ������
* ��������
*ע�⣬��д���������ȶ������еľֲ������ȣ���������
*�������м�����µı����Ķ��塣
******************************************************************/
int httpServer_Create(stHttpServer *pstHttpServer);
int httpServer_Remove(stHttpServer *pstHttpServer);


#endif




/*****************************************************************
 ** �ļ��� �� httpserver.c
 ** 2015-2017 �����˿Ƽ����Źɷ����޹�˾ Co.Ltd
 
 ** ������: �����
 ** ��  ��: 2018-08-13
 ** ��  ��: httpserverģ��
 ** ��  ��: V1.0

 ** �޸���:
 ** ��  ��:
 ** �޸�����:
 ** ��  ��: 
******************************************************************/

/*****************************************************************
* ����ͷ�ļ�
******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/queue.h>
#include <event.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/buffer_compat.h>
#include <event2/util.h>
#include <event2/event_compat.h>
#include <event2/buffer.h>
#include "cJSON.h"
#include "httpServer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************
* �궨��(���ڵ�ǰC�ļ�ʹ�õĺ궨��д�ڵ�ǰC�ļ��У�������д��H�ļ���)
******************************************************************/


/*****************************************************************
* �ṹ����(���ڵ�ǰC�ļ�ʹ�õĽṹ��д�ڵ�ǰC�ļ��У�������д��H�ļ���)
******************************************************************/


/*****************************************************************
* ȫ�ֱ�������
******************************************************************/
int gnEventInitialFlag = 0;

/*****************************************************************
* ��̬��������
******************************************************************/

/*****************************************************************
* �ⲿ�������������ȫ�ֱ���û����������H�ļ�����������ʱ���ڴ˴�������
*�����������H�ļ���������ֻ�������H�ļ����ɣ�
******************************************************************/


/*************************************************
** Function��httpServer_CommonDel
** Description��ͨ�ô���ص�����
** Input����
** Output����

** Return��-1-ʧ��	0-�ɹ�
** Author�������
** Date��2018-11-27

** Modification History��
** Author��
** Date��
** Description��
*************************************************/
void httpServer_CommonDel(struct evhttp_request *req, void *arg) 
{
	int nRet = 0;
	char nlengthBuffer[16]={0};
	char respondBuffer[128]={0};
	stHttpServer *pstHttpServer = (stHttpServer *)arg;

	/*��ȡPOST����������*/
	size_t post_size = EVBUFFER_LENGTH(req->input_buffer);
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
	
	printf("%s get post_data, len =%ld\n",__FUNCTION__, post_size);
	printf("RCV:\n%s\n",post_data);

	int nSize = ((post_size<=pstHttpServer->httpd_RcvBufSize)?post_size:(pstHttpServer->httpd_RcvBufSize));
	memset(pstHttpServer->http_RcvBuffer,0,pstHttpServer->httpd_RcvBufSize);
	memcpy(pstHttpServer->http_RcvBuffer, post_data, nSize);

	/*����û���ע�ᴦ���������ݽ����û�����*/
	if(pstHttpServer->defaultDelFunc != NULL)
	{
		nRet = pstHttpServer->defaultDelFunc(req,pstHttpServer,NULL);	
		/*������Ӧ��Ϣ*/
		if(pstHttpServer->respondDelFunc !=NULL){
			nRet = pstHttpServer->respondDelFunc(req,respondBuffer,&nRet);
		}
	}

	//HTTP header
	evutil_snprintf(nlengthBuffer, sizeof(nlengthBuffer)-1, "%lu", strlen(respondBuffer));
	//evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Content-Length",nlengthBuffer);
	evhttp_add_header(req->output_headers, "Connection", "close");

	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "%s", respondBuffer);
	
	//����װ�õ�evbuffer ���͸��ͻ���
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}


/*************************************************
** Function��httpServer_KeepAliveDel
** Description������������
** Input����
** Output����

** Return��-1-ʧ��	0-�ɹ�
** Author�������
** Date��2018-11-27

** Modification History��
** Author��
** Date��
** Description��
*************************************************/
void httpServer_KeepAliveDel(struct evhttp_request *req, void *arg) 
{
	int nRet = 0;
	char nlengthBuffer[16]={0};
	char respondBuffer[128]={0};
	stHttpServer *pstHttpServer = (stHttpServer *)arg;

	/*��ȡPOST����������*/
	size_t post_size = EVBUFFER_LENGTH(req->input_buffer);
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
	
	printf("%s get post_data, len =%ld\n",__FUNCTION__, post_size);
	printf("RCV:\n%s\n",post_data);
	
	int nSize = ((post_size<=pstHttpServer->httpd_RcvBufSize)?post_size:(pstHttpServer->httpd_RcvBufSize));
	memset(pstHttpServer->http_RcvBuffer,0,pstHttpServer->httpd_RcvBufSize);
	memcpy(pstHttpServer->http_RcvBuffer, post_data, nSize);

	/*����û���ע�ᴦ���������ݽ����û�����*/
	if(pstHttpServer->keepAliveDelFunc != NULL)
	{
		nRet = pstHttpServer->keepAliveDelFunc(req,pstHttpServer,NULL);	
		/*������Ӧ��Ϣ*/
		if(pstHttpServer->respondDelFunc !=NULL){
			nRet = pstHttpServer->respondDelFunc(req,respondBuffer,&nRet);
		}
	}

	//HTTP header
	evutil_snprintf(nlengthBuffer, sizeof(nlengthBuffer)-1, "%lu", strlen(respondBuffer));
	//evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Content-Length",nlengthBuffer);
	evhttp_add_header(req->output_headers, "Connection", "close");

	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "%s", respondBuffer);
	
	//����װ�õ�evbuffer ���͸��ͻ���
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}


/*************************************************
** Function��httpServer_FaceInfoDel
** Description��������Ϣ������
** Input����
** Output����

** Return��-1-ʧ��	0-�ɹ�
** Author�������
** Date��2018-11-27

** Modification History��
** Author��
** Date��
** Description��
*************************************************/
void httpServer_FaceInfoDel(struct evhttp_request *req, void *arg) 
{
	int nRet = 0;
	char nlengthBuffer[16]={0};
	char respondBuffer[128]={0};
	stHttpServer *pstHttpServer = (stHttpServer *)arg;

	/*��ȡPOST����������*/
	size_t post_size = EVBUFFER_LENGTH(req->input_buffer);
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
	
	printf("%s get post_data, len =%ld\n",__FUNCTION__, post_size);
	printf("RCV:\n%s\n",post_data);
	
	int nSize = ((post_size<=pstHttpServer->httpd_RcvBufSize)?post_size:(pstHttpServer->httpd_RcvBufSize));
	memset(pstHttpServer->http_RcvBuffer,0,pstHttpServer->httpd_RcvBufSize);
	memcpy(pstHttpServer->http_RcvBuffer, post_data, nSize);

	/*����û���ע�ᴦ���������ݽ����û�����*/
	if(pstHttpServer->faceInfoDelFuc != NULL)
	{
		nRet = pstHttpServer->faceInfoDelFuc(req,pstHttpServer,NULL);	
		/*������Ӧ��Ϣ*/
		if(pstHttpServer->respondDelFunc !=NULL){
			nRet = pstHttpServer->respondDelFunc(req,respondBuffer,&nRet);
		}
	}

	//HTTP header
	evutil_snprintf(nlengthBuffer, sizeof(nlengthBuffer)-1, "%lu", strlen(respondBuffer));
	//evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Content-Length",nlengthBuffer);
	evhttp_add_header(req->output_headers, "Connection", "close");

	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "%s", respondBuffer);
	
	//����װ�õ�evbuffer ���͸��ͻ���
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}



/*************************************************
** Function��httpServer_Create
** Description������һ��http������
** Input:��
** Output��

** Return��-1-ʧ��	0-�ɹ�
** Author�������
** Date��2018-11-27

** Modification History��
** Author��
** Date��
** Description��
*************************************************/
int httpServer_Create(stHttpServer *pstHttpServer)
{
	//��ʼ��event_init
	event_init();

	printf("httpServer_ListenIP:%s\n",pstHttpServer->httpd_ListenIP);
	printf("httpServer_ListenPort:%d\n",pstHttpServer->httpd_ListenPort);
	printf("httpServer_FaceInfoMatchStr:%s\n",pstHttpServer->httpd_FaceInfoMatchStr);
	printf("httpServer_KeepAliveMatchStr:%s\n",pstHttpServer->httpd_KeepAliveMatchStr);

	pstHttpServer->httpd = evhttp_start(pstHttpServer->httpd_ListenIP, pstHttpServer->httpd_ListenPort);

	evhttp_set_timeout(pstHttpServer->httpd, pstHttpServer->httpd_timeout);
	//�ض���URIָ��callback
	evhttp_set_cb(pstHttpServer->httpd, pstHttpServer->httpd_FaceInfoMatchStr, httpServer_FaceInfoDel, (void*)pstHttpServer);
	evhttp_set_cb(pstHttpServer->httpd, pstHttpServer->httpd_KeepAliveMatchStr, httpServer_KeepAliveDel, (void*)pstHttpServer);

	/*ͨ�ô���ص�����ָ��*/
	evhttp_set_gencb(pstHttpServer->httpd,httpServer_CommonDel, (void*)pstHttpServer);

	//ѭ������events
	event_dispatch();

	evhttp_free(pstHttpServer->httpd);
	return 0;
}

/*************************************************
** Function��httpServer_Remove
** Description��httpServer_Remove
** Input:��
** Output��

** Return��-1-ʧ��	0-�ɹ�
** Author�������
** Date��2018-11-27

** Modification History��
** Author��
** Date��
** Description��
*************************************************/
int httpServer_Remove(stHttpServer *pstHttpServer)
{
	return 0;
}


#ifdef __cplusplus
}
#endif



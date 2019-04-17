/*****************************************************************
 ** �ļ��� �� http.c
 ** 2015-2017 �����˿Ƽ����Źɷ����޹�˾ Co.Ltd
 
 ** ������: �����
 ** ��  ��: 2019-04-10
 ** ��  ��: http�ͻ���
 ** ��  ��: V1.0

 ** �޸���:
 ** ��  ��:
 ** �޸�����:
 ** ��  ��: 
******************************************************************/
#ifdef __cplusplus
	extern "C" {
#endif
	

/*****************************************************************
* ����ͷ�ļ�
******************************************************************/
#include <string.h>
#include "curl.h"
#include "httpClient.h"

/*****************************************************************
* �궨��(���ڵ�ǰC�ļ�ʹ�õĺ궨��д�ڵ�ǰC�ļ��У�������д��H�ļ���)
******************************************************************/

/*****************************************************************
* �ṹ����(���ڵ�ǰC�ļ�ʹ�õĽṹ��д�ڵ�ǰC�ļ��У�������д��H�ļ���)
******************************************************************/

/*****************************************************************
* ȫ�ֱ�������
******************************************************************/


/*****************************************************************
* ��̬��������
******************************************************************/

/*****************************************************************
* �ⲿ�������������ȫ�ֱ���û����������H�ļ�����������ʱ���ڴ˴�������
*�����������H�ļ���������ֻ�������H�ļ����ɣ�
******************************************************************/
																						
/*****************************************************************
* ����ԭ������
******************************************************************/

/*****************************************************************
* ��������
*ע�⣬��д���������ȶ������еľֲ������ȣ���������
*�������м�����µı����Ķ��塣
******************************************************************/

/*************************************************
** Function��httpClient_writeBack
** Description��http�ص����ݽ��պ���
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
size_t httpClient_writeBack(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int writeLength = 0;
	int written  = size*nmemb;
	stHttpUserInfo *pSt = (stHttpUserInfo*)stream;	
	
	if(1 == pSt->ucFlag) //�˴���¼ͷ��
	{
		if(pSt->pRcvBuffer != NULL && (written <= (pSt->nRcvBufLen - pSt->nRcvSize)))
		{
			writeLength = (written <= (pSt->nRcvBufLen - pSt->nRcvSize))?(written):(pSt->nRcvBufLen - pSt->nRcvSize);
			memcpy((char*)(pSt->pRcvBuffer) +pSt->nRcvSize, ptr, writeLength);
			pSt->nRcvSize += writeLength;
			//printf("%d line,nWRLength=%d written=%d nUsedLength=%d writeLength=%d\n",__LINE__,pSt->nRcvBufLen, written,pSt->nRcvSize,writeLength);
		}
		return writeLength;
	}
	else if(0 == pSt->ucFlag)//�˴���¼body����
	{
		if(pSt->pRcvBuffer != NULL && (written <= (pSt->nRcvBufLen - pSt->nRcvSize)))
		{
			writeLength = (written <= (pSt->nRcvBufLen - pSt->nRcvSize))?(written):(pSt->nRcvBufLen - pSt->nRcvSize);
			memcpy((char*)(pSt->pRcvBuffer) +pSt->nRcvSize, ptr, writeLength);
			pSt->nRcvSize += writeLength;
			//printf("%d line,nWRLength=%d written=%d nUsedLength=%d writeLength=%d\n",__LINE__,pSt->nRcvBufLen, written,pSt->nRcvSize,writeLength);
		}
		return writeLength;
	}
	return 0;
}


/*************************************************
** Function��httpClient_httpPost
** Description��httpPost
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
int httpClient_httpPost(stHttpClientInfo *pstUserArg,long *pRespCode)
{
	int nRet = 0;
	CURL *curl = NULL;
	CURLcode code = 0;
	long response_code = -1;
	struct curl_slist *chunk = NULL;

	*pRespCode = 0;

	if(strlen(pstUserArg->chUrl) < 14){
		printf("%s func Invalid parameter,chUrl=%s at (%d) line\n",__FUNCTION__,pstUserArg->chUrl,__LINE__);
		return -1;
	}

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, pstUserArg->chUrl);
		switch(pstUserArg->stHttpUserHead.nOperation)
		{
			case HTTP_POST_ONE_SHOT_PIC_MEM:
				//chunk = curl_slist_append(chunk, "Content-Type: image/jpeg;");
				chunk = curl_slist_append(chunk, "Content-Type: application/json;");
				break;
			break;
			default:
				break;
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0L);		//�ֿ�����head��bodyʱ����Ҫ�������Ϊ0L
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);	//ע�⣬���볬ʱһ��Ҫ�������  
		curl_easy_setopt(curl, CURLOPT_POST, 1L); 

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpClient_writeBack);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pstUserArg->stHttpUserBody); 	/*����body����*/

		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,httpClient_writeBack);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &pstUserArg->stHttpUserHead);	/*����ͷ����*/
		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10*1000);	//������ʱ30s
		switch(pstUserArg->stHttpUserHead.nOperation)
		{
			case HTTP_POST_ONE_SHOT_PIC_MEM:  //һ���Է�
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pstUserArg->stHttpUserBody.pSendBuffer);
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pstUserArg->stHttpUserBody.nSedSize);
				break;
			default:
				break;
		}
		code = curl_easy_perform(curl);
		if(code != CURLE_OK)
		{
			nRet = -1;
			printf("%s func curl_easy_perform failed:%s at (%d) line\n",__FUNCTION__,curl_easy_strerror(code),__LINE__);
		} 
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			if(200 != response_code)
			{
				nRet = -1;
				printf("%s func curl_easy_getinfo RESPONSE_CODE: %d at (%d) line\n",__FUNCTION__,response_code,__LINE__);
			}
		}
		curl_slist_free_all(chunk);
		curl_easy_cleanup(curl);

		*pRespCode = response_code;

		return nRet;
	}

	return -1;
}

/*************************************************
** Function��httpClient_httpGet
** Description��httpget
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
int httpClient_httpGet(stHttpClientInfo *pstUserArg,long *pRespCode)
{
	int nRet		= 0;
	CURL *curl		= NULL;
	CURLcode code	= 0;
	long response_code = -1;

	*pRespCode = 0;

	if(strlen(pstUserArg->chUrl) < 14){
		printf("%s func Invalid parameter,chUrl=%s at (%d) line\n",__FUNCTION__,pstUserArg->chUrl,__LINE__);
		return -1;
	}
	
	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_HEADER, 0L);							/*�ֿ�����head��bodyʱ����Ҫ�������Ϊ0L*/
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);						/*ע�⣬���볬ʱһ��Ҫ�������*/
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);		
		
		curl_easy_setopt(curl, CURLOPT_URL, pstUserArg->chUrl);
		curl_easy_setopt(curl, CURLOPT_USERNAME, pstUserArg->chUserName);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, pstUserArg->chUserPassWord);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpClient_writeBack);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pstUserArg->stHttpUserBody); 	/*����body����*/

		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,httpClient_writeBack);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &pstUserArg->stHttpUserHead);	/*����ͷ����*/
		
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10*1000);			/*������ʱ10s*/		
		code = curl_easy_perform(curl);
		if(code != CURLE_OK)
		{
			nRet = -1;
			printf("%s func curl_easy_perform failed:%s at (%d) line\n",__FUNCTION__,curl_easy_strerror(code),__LINE__);
		} 
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			if(200 != response_code)
			{
				nRet = -1;				
				printf("%s func curl_easy_getinfo RESPONSE_CODE: %d at (%d) line\n",__FUNCTION__,response_code,__LINE__);
			}
		}
		
		curl_easy_cleanup(curl);
		*pRespCode = response_code;

		return nRet;
	}
	return -1;
}



#ifdef __cplusplus
}
#endif


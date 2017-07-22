/**********************************************************
 * \file json_test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version
 ** \author Allen.L
**********************************************************/

#include <stdio.h>
#include <iostream>
#include <exception>
#include <memory.h>
#include "json/json.h"

// 测试使用的结构体
typedef struct {
	int nNum;
	char szFile[20];
}TEST_ST_FILE;

typedef struct {
	char szMemo[20];
	int nCount;
	TEST_ST_FILE szFileList[5];
}TEST_ST_FILE_LIST;

// 结构体数据转换为JSON字符串
int StructDataToJsonString(TEST_ST_FILE_LIST *pStructData, char *pJsonData)
{
	try
	{
		Json::Value root;
		Json::Value arrayObj;
		Json::Value item;

		root["memo"] = pStructData->szMemo;
		root["file_count"] = pStructData->nCount;
		// 生成file_list数组
		for(int i = 0; i < pStructData->nCount; i ++)
		{
			item["num"] = pStructData->szFileList[i].nNum;
			item["file"] = pStructData->szFileList[i].szFile;
			arrayObj.append(item);
		}
		root["file_list"] = arrayObj;
		// JSON转换为JSON字符串（已格式化）
		std::string strOut = root.toStyledString();
		//  JSON转换为JSON字符串（未格式化）
		//Json::FastWriter writer;
		//std::string strOut = writer.write(root);

		strcpy(pJsonData, strOut.c_str());
	}
	catch(std::exception &ex)
	{
		printf("StructDataToJsonString exception %s.\n", ex.what());
		return -1;
	}
	return 0;
}

// JSON字符串转换为结构体数据
int JsonStringToStructData(char *pJsonData, TEST_ST_FILE_LIST *pStructData)
{
	try
	{
		bool bRet = false;
		std::string strTemp;
		Json::Reader reader;
		Json::Value value;
		Json::Value arrayObj;
		Json::Value item;

		// JSON字符串转换为JSON数据
		bRet = reader.parse(pJsonData, value);
		if(bRet == false)
		{
			printf("JsonStringToStructData reader parse error.\n");
			return -1;
		}
		// memo
		bRet = value["memo"].empty();
		if(bRet == true)
		{
			printf("JsonStringToStructData memo is not exist.\n");
			return -1;
		}
		strTemp = value["memo"].asString();
		strncpy(pStructData->szMemo, strTemp.c_str(), sizeof(pStructData->szMemo));
		// file_count
		bRet = value["file_count"].empty();
		if(bRet == true)
		{
			printf("JsonStringToStructData file_count is not exist.\n");
			return -1;
		}
		pStructData->nCount = value["file_count"].asInt();

		// 解析file_list数组
		arrayObj = value["file_list"];
		pStructData->nCount = arrayObj.size();
		for(int nIdx = 0; nIdx < pStructData->nCount; nIdx++)
		{
			item = arrayObj[nIdx];
			// num
			pStructData->szFileList[nIdx].nNum = item["num"].asInt();
			// file
			strTemp = item["file"].asString();
			strncpy(pStructData->szFileList[nIdx].szFile, strTemp.c_str(),
				sizeof(pStructData->szFileList[nIdx].szFile));
		}
	}
	catch(std::exception &ex)
	{
		printf( "JsonStringToStructData exception %s.\n", ex.what());
		return -1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	TEST_ST_FILE_LIST stFileList;
	char szJsonData[4096];

	memset(&stFileList, 0, sizeof(TEST_ST_FILE_LIST));
	memset(szJsonData, 0, sizeof(szJsonData));

	// 测试数据
    strncpy(stFileList.szMemo, "jsoncppmemo", sizeof(stFileList.szMemo));
    stFileList.nCount = 5;
    for(int i = 0; i < stFileList.nCount; i++)
    {
        stFileList.szFileList[i].nNum = i + 1;
        sprintf(stFileList.szFileList[i].szFile, "file%d", i + 1);
    }
    printf("struct data to json string.\n");
    printf("memo:%s count:%d.\n", stFileList.szMemo, stFileList.nCount);
    for(int i = 0; i < stFileList.nCount; i++)
    {
        printf("num:%d file:%s.\n", stFileList.szFileList[i].nNum,
            stFileList.szFileList[i].szFile);
    }

    // 结构体数据转换为JSON字符串
    StructDataToJsonString(&stFileList, szJsonData);
    printf("json string:%s.\n", szJsonData);

    // JSON字符串转换为结构体数据
    memset(&stFileList, 0, sizeof(TEST_ST_FILE_LIST));
    JsonStringToStructData(szJsonData, &stFileList);
    printf("json string to struct data.\n");
    printf("memo:%s count:%d.\n", stFileList.szMemo, stFileList.nCount);
    for(int i = 0; i < stFileList.nCount; i++)
    {
        printf("num:%d file:%s.\n", stFileList.szFileList[i].nNum,
            stFileList.szFileList[i].szFile);
    }

	return 0;
}

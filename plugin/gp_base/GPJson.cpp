/**********************************************************
 * \file GPJson.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include "gp_base/GPJson.h"
#include "gp_base/GPLogger.h"
#include "gp_base/GPStringUtil.h"
#include "gp_base/GPEncrypt.h"


namespace GPBase {

	GPString GPJsonTool::indexOfSplittingPath = ".";
	GPString GPJsonTool::indexOfSplittingKey = ",";

	GPJsonTool::GPJsonTool( ){;}
	GPJsonTool::~GPJsonTool( ){;}
	GPBOOL GPJsonTool::parseJs( const GPString &jsStr, Json::Value &jsValue ){
		Json::Reader jsReader;
		return jsReader.parse(  jsStr, jsValue );
	}


	Json::Value GPJsonTool::getJsValue( const Json::Value &jsValue, const GPString &path ){
		Json::Value jsValueTmp = jsValue;
		Json::Value* jsValueTmp_p = findJsValue( jsValueTmp, path );
		if(  jsValueTmp_p != NULL )
			return *jsValueTmp_p;
		else
			return Json::Value( );
	}


	Json::Value GPJsonTool::getJsValue( const Json::Value &jsArray, const GPString &key, const GPString &value ){
		Json::Value jsArrayTmp = jsArray;
		Json::Value* jsValueTmp_p = findJsValue( jsArrayTmp, key, value );
		if(  jsValueTmp_p != NULL )
			return *jsValueTmp_p;
		else
			return Json::Value( );
	}


	Json::Value* GPJsonTool::findJsValue( Json::Value &jsValue, const GPString &path ){
		if(  path.size() == 0 ) return &jsValue;

		GPStringVector keys;
		if(  !parseKeyPath( path, keys ) )  return NULL;

		GPString error( "");
		Json::Value* jsFoundValue_p = &jsValue;
		for(  GPStringVector::iterator iterKey = keys.begin(); iterKey != keys.end(); ++iterKey ){
			error += " key --> " + *iterKey + " ||";
			jsFoundValue_p = findJsValueByKey( *jsFoundValue_p, *iterKey );
			if(  jsFoundValue_p == NULL ){
				GPLOG_DEBUG("found a empty json value.key path is [%s]	already go [%s]", path.c_str(), error.c_str());
				return NULL;
			}
		}  //for
		return jsFoundValue_p;
	}


	Json::Value* GPJsonTool::findJsValue( Json::Value &jsArray, const GPString &key, const GPString &value ){
		if(  jsArray.isArray() ){
			long jsCount = jsArray.size( );
			for(  int i=0; i<jsCount; ++i ){
				Json::Value::Members vKeys = jsArray[i].getMemberNames( );
				for(  Json::Value::Members::iterator iterJson = vKeys.begin(); iterJson != vKeys.end(); ++iterJson ){
					if(  key == (*iterJson) ){
						if(  jsArray[i][*iterJson].isString() && jsArray[i][*iterJson].asString() == value )
							return &jsArray[i];
					}
				} //for
			} //for
		}
		return NULL;
	}


	GPString GPJsonTool::getJsString( const Json::Value &jsValue, GPString path ){
		Json::Value jsValueTmp = getJsValue( jsValue, path );
		GPString value(NULL);
		if( jsValueTmp.isString() )
			value = jsValueTmp.asString( );
		else if( jsValueTmp.isUInt() || jsValueTmp.isInt() )
			value = GPUniversal::GPStringUtil::intToStr( jsValueTmp.asUInt() );
		else if( jsValueTmp.isDouble() )
			value = GPUniversal::GPStringUtil::intToStr( (long)jsValueTmp.asDouble() );
		else
			return value;
		return value;
	}
	GPBOOL GPJsonTool::getJsStrList( const Json::Value &jsArray, const GPString &key, GPStringVector &strList ){
		strList.clear( );
		int jsCount = jsArray.size( );
		for(  int i=0; i<jsCount; ++i )
			strList.push_back( getJsString(jsArray[i], key ) );
		return true;
	}
	GPBOOL GPJsonTool::convertJsToMap( const Json::Value &jsValue, map<GPString, GPString> &mapResult, GPString keyList ){
		if(  jsValue.isArray() ){
			mapResult.clear( );
			mapResult.insert( pair<GPString,GPString>("array", getJsString(jsValue)) );
			GPLOG_DEBUG( "Json is a array.");
			return false;
		}
		if(  jsValue.isNull() ){
			GPLOG_DEBUG( "Json data has empty.");
			return false;
		}

		GPBOOL doFilterData = false;
		GPStringVector needKeys;
		if(  keyList.size() != 0 ){
			doFilterData = true;
			if(  !parseKeyList(keyList, needKeys ) )  return false;
		}

		mapResult.clear( );
		Json::Value::Members vKeys = jsValue.getMemberNames( );
		for(  Json::Value::Members::iterator iterKey = vKeys.begin(); iterKey != vKeys.end(); ++iterKey ){
			if(  !doFilterData )
				mapResult.insert( pair<GPString,GPString>(*iterKey, getJsString(jsValue[*iterKey])) );
			else if(  std::find(needKeys.begin(), needKeys.end(), *iterKey ) != needKeys.end() )
				mapResult.insert( pair<GPString,GPString>(*iterKey, getJsString(jsValue[*iterKey])) );
		}  // for
		return true;
	}
	GPBOOL GPJsonTool::convertJsToMapList( const Json::Value &jsValue, vector<map<GPString, GPString> > &mapListResult, GPString keyList ){
		mapListResult.clear( );
		map<GPString, GPString> content;
		if(  jsValue.isArray() ){
			for(  int i=0; i<jsValue.size(); ++i ){
				convertJsToMap( jsValue[i], content, keyList );
				mapListResult.push_back( content);
			}
		}
		else{
			GPLOG_DEBUG( "Json value is not a array." );
			return false;
		}
		return true;
	}
	GPBOOL GPJsonTool::convertJsToMapList( const Json::Value &jsValue, list<map<GPString, GPString> > &mapListResult, GPString keyList ){
		mapListResult.clear( );
		map<GPString, GPString> content;
		if(  jsValue.isArray() ){
			for(  int i=0; i<jsValue.size(); ++i ){
				convertJsToMap( jsValue[i], content, keyList );
				mapListResult.push_back( content);
			}
		}
		else{
			GPLOG_DEBUG( "Json value is not a array." );
			return false;
		}
		return true;
	}
	GPString GPJsonTool::convertMapToJsStr( const map<GPString, GPString> mapData ){
		GPString defaultJsStr = "\t{\n";

		map<GPString, GPString>::const_iterator citerJsItem = mapData.begin( );
		while(  citerJsItem != mapData.end() ){
			GPString valueTmp = citerJsItem->second;
			GPUniversal::GPStringUtil::strReplaceAll( valueTmp, "\"", "\\\"" );
			GPUniversal::GPStringUtil::strReplaceAll( valueTmp, "\r", "\\r" );
			GPUniversal::GPStringUtil::strReplaceAll( valueTmp, "\n", "\\n" );
			GPUniversal::GPEncrypt::convertFullToHalf( valueTmp );

			defaultJsStr += "\t\t\"" + citerJsItem->first + "\" : \"" + valueTmp + "\"";
			if(  ++citerJsItem == mapData.end() )
				defaultJsStr += "\n";
			else
				defaultJsStr += ",\n";
		}

		defaultJsStr += "\t}";

		return defaultJsStr;
	}
	GPString GPJsonTool::convertMapListToJsStr( const vector<map<GPString, GPString> > &mapList, GPString arrayName ){
		if(  mapList.size() == 0 )  return "";

		GPString defaultJsStr;
		defaultJsStr += "[\n";
		for( vector<map<GPString,GPString> >::const_iterator citerJsBlock = mapList.begin(); citerJsBlock != mapList.end(); ){
			defaultJsStr += GPJsonTool::convertMapToJsStr( *citerJsBlock );
			if(  ++citerJsBlock != mapList.end() )
				defaultJsStr += ",";
		}
		defaultJsStr += "\n]\n";

		if(  arrayName.size() != 0 ){
			defaultJsStr = "{\n\t\"" + arrayName + "\" : " + defaultJsStr + "}";
		}
		return defaultJsStr;
	}
	GPString GPJsonTool::convertMapListToJsStr( const list<map<GPString, GPString> > &mapList, GPString arrayName ){
		if(  mapList.size() == 0 )  return "";

		GPString defaultJsStr;
		defaultJsStr += "[\n";
		for( list<map<GPString,GPString> >::const_iterator citerJsBlock = mapList.begin(); citerJsBlock != mapList.end(); ){
			defaultJsStr += GPJsonTool::convertMapToJsStr( *citerJsBlock );
			if(  ++citerJsBlock != mapList.end() )
				defaultJsStr += ",";
		}
		defaultJsStr += "\n]\n";

		if(  arrayName.size() != 0 ){
			defaultJsStr = "{\n\t\"" + arrayName + "\" : " + defaultJsStr + "}";
		}
		return defaultJsStr;
	}
	GPBOOL GPJsonTool::parseKeyPath( const GPString &path, GPStringVector &keys ){
		return GPUniversal::GPStringUtil::splitString( path, indexOfSplittingPath, keys );
	}
	GPBOOL GPJsonTool::parseKeyList( const GPString &keyList, GPStringVector &keys ){
		if(  !GPUniversal::GPStringUtil::splitString(keyList, indexOfSplittingKey, keys ) ){
			GPLOG_DEBUG( "key list format is wrong.key list is [%s]", keyList.c_str());
			return false;
		}
		return true;
	}
	Json::Value* GPJsonTool::findJsValueByKey( Json::Value &jsValue, GPString &key ){
		Json::Value *jsValue_p = &jsValue;
		if(  jsValue_p->isString() || jsValue_p->empty() || jsValue_p->isNull() )
			return NULL;
		if(  jsValue_p->isArray() ){
			if( !GPUniversal::GPStringUtil::checkNumber(key) ){
				GPLOG_DEBUG( "Key is not a number. Key name is [%s]", key.c_str());
				return NULL;
			}
			int index = GPUniversal::GPStringUtil::strToInt( key.c_str(), 0);
			if(  (GPUInt32&) index >= jsValue_p->size() ) return NULL;
			return &( *jsValue_p)[index];
		}
		else{
			Json::Value::Members vKeys = jsValue_p->getMemberNames( );
			for(  Json::Value::Members::iterator iterJson = vKeys.begin(); iterJson != vKeys.end(); ++iterJson ){
				if(  key == (*iterJson) ){
					return &( *jsValue_p)[key];
				}
			} //for
		}
		return NULL;
	}
}


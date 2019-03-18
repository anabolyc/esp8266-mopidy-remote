#include <SmingCore/SmingCore.h>

#ifndef _VAR_SETTINGS_H_
#define _VAR_SETTINGS_H_

#define VAR_SETTINGS_FILE ".varsettings.conf"

struct KeyValuePair {
	String Key;
	String Value;

	KeyValuePair() { }

	KeyValuePair(String key, String value) {
		Key = key;
		Value = value;
	}
};

struct VariableSettingsStorage {
	
	String src = VAR_SETTINGS_FILE;

	uint8_t size = 0;
	KeyValuePair* variables = NULL;

	void add(String key, String value) {
		debugf("current size is: %i", size);
		KeyValuePair* arr = new KeyValuePair[size + 1];
		if (size > 0) {
			debugf("copy %i bytes", size * sizeof(KeyValuePair));
			memcpy(arr, variables, size * sizeof(KeyValuePair));
			delete[] variables;
		}

		debugf("creating new struct");
		KeyValuePair* x = new KeyValuePair(key, value);
		arr[size++] = *x;
		debugf("created: %s, %s", arr[size].Key.c_str(), arr[size].Value.c_str());

		variables = arr;
	}

	void load()
	{
		DynamicJsonBuffer jsonBuffer;
		if(exist()) {
			int size = fileGetSize(VAR_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(VAR_SETTINGS_FILE, jsonString, size + 1);
			debugf("Read from %s \n%s ", VAR_SETTINGS_FILE, jsonString);
			JsonObject& root = jsonBuffer.parseObject(jsonString);
			JsonArray& keys = root["keys"];
			JsonArray& values = root["values"];

			size = keys.size();
			variables = new KeyValuePair[size];

			for (uint8_t i = 0; i < size; i++) {
				KeyValuePair* x = new KeyValuePair(keys[i].asString(), values[i].asString());
				variables[i] = *x;
			}

			delete[] jsonString;
		}
	}

	void save()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		JsonArray& keys = root.createNestedArray("keys");
		JsonArray& values = root.createNestedArray("values");
		 
		for (uint8_t i = 0; i < size; i++) {
			keys.add(variables[i].Key);
			values.add(variables[i].Value);
		}

		String rootString;
		root.printTo(rootString);
		fileSetContent(VAR_SETTINGS_FILE, rootString);
		debugf("Write to %s \n%s ", VAR_SETTINGS_FILE, rootString.c_str());
	}

	bool exist()
	{
		return fileExist(VAR_SETTINGS_FILE);
	}
};

static VariableSettingsStorage VarSettings;

#endif
#include "tess.h"

#include "common.h"

tesseract::TessBaseAPI* api;


const char* toCString(JNIEnv* env, jstring text) {

	const jchar* jchars = env->GetStringChars(text, NULL);
	int len = env->GetStringLength(text);

	char* result = new char[len+1];

	for (int k = 0; k < len; ++k) {
		result[k] = (char)jchars[k];
	}

	result[len] = 0;

	return result;
}

const jchar* toJString(const char* text) {
	size_t len = strlen(text);

	jchar* result = new jchar[len+1];

	for (int k = 0; k < len; ++k) {
		result[k] = (jchar)text[k];
	}

	result[len] = 0;

	return result;
}


JNIEXPORT void JNICALL Java_imbacad_model_ocr_Tesseract_init(JNIEnv* env, jclass c, jstring file, jstring lang) {
	api = new tesseract::TessBaseAPI();

	const char* filechars = toCString(env, file);
	const char* langchars = toCString(env, lang);

	if (api->Init(filechars, langchars)) {
		fprintf(stderr, "Could not initialize tesseract.\n");
	}

	delete[] filechars;
	delete[] langchars;
}


JNIEXPORT jstring JNICALL Java_imbacad_model_ocr_Tesseract_getUTF8Text(JNIEnv* env, jclass c, jintArray data, jint w, jint h) {

	const jint* jdata = env->GetIntArrayElements(data, NULL);

	api->SetImage((BYTE*)jdata, w, h, 4, w * 4);

	const char* tessText = api->GetUTF8Text();
	const jchar* jTessText = toJString(tessText);

	jstring result = env->NewString(jTessText, (jsize)strlen(tessText));

	delete[] tessText;
	delete[] jTessText;

	return result;
}


JNIEXPORT void JNICALL Java_imbacad_model_ocr_Tesseract_end(JNIEnv* env, jclass c) {
	api->End();
}
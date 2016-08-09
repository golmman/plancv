#include "text.h"
#include "simpleedge.h"
#include "opencv2/features2d/features2d.hpp"

void tess_init(TessBaseAPI& api, const char* language) {

	

	if (api.Init("./", language)) {
		cout << "Could not initialize tesseract." << endl;
		exit(1);
	}
}

void tess_end(TessBaseAPI& api) {
	api.End();
}

void tess_set_image(TessBaseAPI& api, const Mat& img) {
	api.SetImage((uchar*)img.data, img.size().width, img.size().height, img.channels(), (int)img.step1());
}



void test_blob() {
	Mat img_src = imread("test_plan5.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_thr, img_out;
	threshold(img_src, img_thr, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
	cvtColor(img_thr, img_out, CV_GRAY2BGR);

	// set up the parameters (check the defaults in opencv's code in blobdetector.cpp)
	cv::SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = 0.0f;
	params.filterByInertia = true;
	params.minInertiaRatio = 0.2f;
	params.maxInertiaRatio = 1.0f;

	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;

	params.filterByArea = true;
	params.minArea = 200.0f;
	params.maxArea = 1000.0f;
	// ... any other params you don't want default value

	// set up and create the detector using the parameters
	cv::SimpleBlobDetector blob_detector(params);
	// or cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params)

	// detect!
	vector<cv::KeyPoint> keypoints;
	blob_detector.detect(img_thr, keypoints);

	// extract the x y coordinates of the keypoints: 

	for (int i=0; i<keypoints.size(); i++){
		float X = keypoints[i].pt.x; 
		float Y = keypoints[i].pt.y;
		
		cv::circle(img_out, Point((int)X, (int)Y), (int)keypoints[i].size, Scalar(0, 0, 255));
	}

	image_write(img_out, "debug_blob.png");
}



void test_blob2() {
	// crashes
	return;
    Mat img_src = imread("test_plan5.png", 1);
	Mat img_thr = img_src.clone();
	//threshold(img_src, img_thr, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	vector<vector<Point>> regions;

    cv::MSER ms;

    ms(img_thr, regions);

	cout << regions.size() << endl;
    for (int i = 0; i < regions.size(); i++) {
        rectangle(img_thr, cv::boundingRect(regions[i]), Scalar(0, 0, 255));
    }

	image_write(img_thr, "debug_blob2.png");
}


void test_text() {
	test_blob2();
	return;

	TessBaseAPI api;

	tess_init(api, "deu");

	//api.SetVariable("tessedit_char_whitelist", ".:&0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZßäöüÄÖÜ");
	api.SetVariable("tessedit_char_whitelist", ".0123456789");
	
	//api.SetVariable("textord_no_rejects", "1");
	//api.SetPageSegMode(tesseract::PSM_AUTO);
	//api.SetVariable("textord_space_size_is_variable", "1");
	//api.SetVariable("textord_no_rejects", "1");
	//api.SetVariable("words_default_fixed_limit", "1.0");
	//api.SetVariable("textord_heavy_nr", "1");


	Mat img_src = imread("test_plan5.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_thr, img_out;
	threshold(img_src, img_thr, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);



	img_thr = img_src.clone();
	cvtColor(img_thr, img_out, CV_GRAY2BGR);

	img_src.release();

	tess_set_image(api, img_thr);

	api.Recognize(0);

	tesseract::ResultIterator* ri = api.GetIterator();
	//tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;
	if (ri) {
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
				word, conf, x1, y1, x2, y2);
			
			//if (conf > 80.0f && word[0] != 0) {
				Scalar color(0.0f, 2.55f * conf, 2.55f * (100.0f - conf));
				cv::rectangle(img_out, Point(x1, y1), Point(x2, y2), color);
				cv::putText(img_out, word, Point(x1, y1), cv::FONT_HERSHEY_PLAIN, 1.0, Scalar(160, 160, 160));
			//}

			delete[] word;
		} while (ri->Next(level));
	}
	
	image_write(img_out, "debug_tess.png");

	//char* text = api.GetUTF8Text();
	//cout << text << endl;
	//delete[] text;


	tess_end(api);
}
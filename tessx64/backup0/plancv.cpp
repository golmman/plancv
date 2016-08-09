#include "plancv.h"

#include "simpleedge.h"
#include "bresenham.h"
#include "lineeval.h"
#include "connectlines.h"
#include "hough.h"
#include "smooth_angle.h"
#include "imgcc.h"



bool compare_polar_lines(const Vec2f& l0, const Vec2f& l1) {
	return (l0[0] < l1[0]);
}




int plancv(PlanData& data) {

	Mat img_src, img_thr, img_edg, img_out, img_outw, img_outw2;

	vector<LineSeg> lines;
	vector<Vec2f> lines_polar;
	vector<double> partition;
	vector<double> peaks;

	TessBaseAPI tess_api;


	/*
	 * 1 Read the image data from a file.
	 */
	PCV_DEBUG(progress_msg("1 read file..."));
	img_src = imread(data.info.filename, CV_LOAD_IMAGE_GRAYSCALE);
	if(img_src.empty()) {
		cout << "can not open file" << endl;
		return -1;
	}
	data.width = img_src.cols;
	data.height = img_src.rows;



	

	/*
	 * 2 Convert to black-white image via otsu's threshold method.
	 */
	PCV_DEBUG(progress_msg("2 threshold detection..."));
	threshold(img_src, img_thr, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
	img_src.release();
	PCV_DEBUG(image_write(img_thr, "debug_thresh.png"));

	/*
	 * 3 Remove non edge pixels.
	 * Alternativley use canny edge detection, it blurs the image though.
	 */
	PCV_DEBUG(progress_msg("3 edge detection..."));
	edges_detect(img_thr, img_edg);
	//Canny(img_thr, img_edg, 50, 200, 5, true);
	//img_edg = img_thr.clone();
	img_thr.release();
	PCV_DEBUG(image_write(img_edg, "debug_edge.png"));

	/*
	 * 3.1 Remove small connected components
	 */
	PCV_DEBUG(progress_msg("3.1 Remove small connected components..."));
	cc_clear(img_edg, 0, data.info.cc_filter_width, 0, data.info.cc_filter_width);
	PCV_DEBUG(image_write(img_edg, "debug_clear_cc.png"));
	
	/*
	 * 4 Analysis before performing Hough transform.
	 */
	PCV_DEBUG(progress_msg("4 analyse image..."));
	vector<int> votes;
	vector<double> weights;

	// collect all angle votes above the threshold in hough space
	hough_analyse(img_edg, votes, data.info.hough_dres, data.info.hough_ares, data.info.hough_thr);
	
	peaks_find(votes, peaks, 0.5);
	cout << endl;
	cout << "peaks at: " << peaks;

	// normalise votes (their sum is 1 afterwards)
	votes_normalise(votes, weights, data.info.peak_smoother);

	// create a partition of [0, pi[ for use in hough transform
	//partition_equidistant(partition, data.info.hough_ares);
	partition_weighted(weights, partition);

	// create histogram of the votes
	PCV_DEBUG(histogram_create(weights));




	/*
	 * 5 Perform the hough algorithm on the edge image.
	 */
	
	//PCV_DEBUG(progress_msg("5 standard hough transform..."));
	//hough_standard(img_edg, lines_polar, 
	//	data.info.hough_dres,
	//	data.info.hough_ares,
	//	data.info.hough_thr);

	PCV_DEBUG(progress_msg("5 partitioned hough transform..."));
	hough_partition(img_edg, lines_polar, 
		data.info.hough_dres,
		partition,
		data.info.hough_thr);
	
	// Sort line by distance (ascendent/descendent does not matter),
	// this way connecting lines takes fewer steps since the dist_ll comparison
	// succeeds more often.
	std::sort(lines_polar.begin(), lines_polar.end(), compare_polar_lines);

	PCV_DEBUG(cout << lines_polar.size() << " polar lines, ");

	/*
	 * 6 Transform polar lines to line segments
	 */
	PCV_DEBUG(progress_msg("6 line transform: polar to segments..."));
	bhm_hough(img_edg, data.info.min_line_length, data.info.max_line_gap, lines, lines_polar);
	PCV_DEBUG(cout << lines.size() << " segment lines, ");



	
	/*
	 * 7 Connect lines
	 */
	PCV_DEBUG(progress_msg("7 connect lines..."));
	vector<LineSeg> lines_old(lines);

	lines_connect(lines, data.info.max_line_gap, 2.0);

	// test
	PCV_DEBUG (
		PixelStats pstats;
		bhm_difference(pstats, lines_old, lines);
		cout << endl;
		cout << "  differences:" << endl;
		cout << "  " << pstats.total1 << " total1, " << pstats.total2 << " total2" << endl;
		cout << "  " <<  pstats.overlap1 << " overlap1, " << pstats.overlap2 << " overlap2" << endl;
		cout << "  " <<  pstats.unique1 << " pixels removed, " << pstats.unique2 << " pixels added";
	);

	PCV_DEBUG(progress_msg("7.1 linesw_connect..."));
	vector<LineSegW> linesw;
	linesw_connect(lines_old, linesw, data.info.max_line_gap, 5.0);
	PCV_DEBUG(cout << lines_old.size() << "->" << linesw.size() << " ");

	PCV_DEBUG(progress_msg("7.2 linesw_connect..."));
	vector<LineSegW> linesw2;
	linesw_connect(linesw, linesw2, data.info.max_line_gap);
	PCV_DEBUG(cout << linesw.size() << "->" << linesw2.size() << " ");

	//PCV_DEBUG(progress_msg("7.3 linesw_connect..."));
	//linesw.clear();
	//linesw_connect(linesw2, linesw, data.info.max_line_gap);
	//PCV_DEBUG(cout << linesw2.size() << "->" << linesw.size() << " ");

	//PCV_DEBUG(progress_msg("7.4 linesw_connect..."));
	//linesw2.clear();
	//linesw_connect(linesw, linesw2, data.info.max_line_gap);
	//PCV_DEBUG(cout << linesw.size() << "->" << linesw2.size() << " ");


	/*
	 * 8 Evaluate lines
	 */
	PCV_DEBUG(progress_msg("8 line evaluation..."));
	lines_evaluate(data, lines);

	PCV_DEBUG (
		// prepare colour image version of the edge image
		cvtColor(img_edg, img_out, CV_GRAY2BGR);
		img_outw = img_out.clone();
		img_outw2 = img_out.clone();

		// write lines to output image
		for (int k = 0; k < data.chunks.size(); ++k) {
			Scalar color;
			if (data.chunks[k].linescore >= 3.0) {
				color = Scalar(0, 0, 255);
			} else {
				color = Scalar(255, 0, 0);
			}

			line(img_out, 
				Point(data.chunks[k].linex1, data.chunks[k].liney1), 
				Point(data.chunks[k].linex2, data.chunks[k].liney2), 
				color);
		}
		image_write(img_out, "debug_lines.png");
		img_out.release();


		// write linesw to output image
		for (int k = 0; k < linesw.size(); ++k) {
			Scalar color = Scalar(0, 0, 255);

			//if (linesw[k].width < 255 && linesw[k].width > 0) {
			//	line(img_outw, linesw[k].s, linesw[k].e, color, cvRound(linesw[k].width));
			//} else {
			//	cout << linesw[k] << endl;
			//}
			linesw[k].draw(img_outw, color);
		}
		image_write(img_outw, "debug_linesw.png");
		img_outw.release();

		// write linesw2 to output image
		for (int k = 0; k < linesw2.size(); ++k) {
			Scalar color = Scalar(0, 0, 255);

			//if (linesw2[k].width < 255 && linesw2[k].width > 0) {
			//	line(img_outw2, linesw2[k].s, linesw2[k].e, color, cvRound(linesw2[k].width));
			//} else {
			//	cout << linesw2[k] << endl;
			//}
			linesw2[k].draw(img_outw2, color);
		}
		image_write(img_outw2, "debug_linesw2.png");
		img_outw2.release();
	);

	img_edg.release();

	return 0;

}
#include "hough.h"



typedef struct CvLinePolar {
    float rho;
    float angle;
} CvLinePolar;

#define hough_cmp_gt(l1,l2) (aux[l1] > aux[l2])

static CV_IMPLEMENT_QSORT_EX(icvHoughSortDescent32s, int, hough_cmp_gt, const int*)


static void
icvHoughLinesStandard( const CvMat* img, float rho, float theta,
                       int threshold, CvSeq *lines, int linesMax )
{
    cv::AutoBuffer<int> _accum, _sort_buf;
    cv::AutoBuffer<float> _tabSin, _tabCos;

    const uchar* image;
    int step, width, height;
    int numangle, numrho;
    int total = 0;
    int i, j;
    float irho = 1 / rho;
    double scale;

    CV_Assert( CV_IS_MAT(img) && CV_MAT_TYPE(img->type) == CV_8UC1 );

    image = img->data.ptr;
    step = img->step;
    width = img->cols;
    height = img->rows;

    numangle = cvRound(CV_PI / theta);
    //numrho = cvRound(((width + height) * 2 + 1) / rho);
	numrho = cvRound((sqrt(width * width + height * height) * 2 + 1) / rho);

    _accum.allocate((numangle+2) * (numrho+2));
    _sort_buf.allocate(numangle * numrho);
    _tabSin.allocate(numangle);
    _tabCos.allocate(numangle);

    int *accum = _accum, *sort_buf = _sort_buf;
    float *tabSin = _tabSin, *tabCos = _tabCos;

    memset( accum, 0, sizeof(accum[0]) * (numangle+2) * (numrho+2) );

    float ang = 0;
    for(int n = 0; n < numangle; ang += theta, n++ )
    {
        tabSin[n] = (float)(sin((double)ang) * irho);
        tabCos[n] = (float)(cos((double)ang) * irho);
    }

    // stage 1. fill accumulator
    for( i = 0; i < height; i++ )
        for( j = 0; j < width; j++ )
        {
            if( image[i * step + j] != 0 )
                for(int n = 0; n < numangle; n++ )
                {
                    int r = cvRound( j * tabCos[n] + i * tabSin[n] );
                    r += (numrho - 1) / 2;
                    accum[(n+1) * (numrho+2) + r+1]++;
                }
        }


    // stage 2. find local maximums
    for(int r = 0; r < numrho; r++ )
        for(int n = 0; n < numangle; n++ )
        {
            int base = (n+1) * (numrho+2) + r+1;
            if( accum[base] > threshold &&
                accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
                accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2] )
                sort_buf[total++] = base;
        }

    // stage 3. sort the detected lines by accumulator value
    icvHoughSortDescent32s( sort_buf, total, accum );

    // stage 4. store the first min(total,linesMax) lines to the output buffer
    linesMax = MIN(linesMax, total);
    scale = 1./(numrho+2);
    for( i = 0; i < linesMax; i++ )
    {
        CvLinePolar line;
        int idx = sort_buf[i];
        int n = cvFloor(idx*scale) - 1;
        int r = idx - (n+1)*(numrho+2) - 1;
        line.rho = (r - (numrho - 1)*0.5f) * rho;
        line.angle = n * theta;
        cvSeqPush( lines, &line );
    }
}








static void
_hough_partition( const CvMat* img, float rho, const vector<double>& pa,
                       int threshold, CvSeq *lines, int linesMax )
{
    cv::AutoBuffer<int> _accum, _sort_buf;
    cv::AutoBuffer<double> _tabSin, _tabCos;

    const uchar* image;
    int step, width, height;
    int numangle, numrho;
    int total = 0;
    int i, j;
    double irho = 1 / rho;
    double scale;

    CV_Assert( CV_IS_MAT(img) && CV_MAT_TYPE(img->type) == CV_8UC1 );

    image = img->data.ptr;
    step = img->step;
    width = img->cols;
    height = img->rows;

    numangle = (int)pa.size();
    //numrho = cvRound(((width + height) * 2 + 1) / rho);
	numrho = cvRound((sqrt(width * width + height * height) * 2 + 1) / rho);

    _accum.allocate((numangle+2) * (numrho+2));
    _sort_buf.allocate(numangle * numrho);
    _tabSin.allocate(numangle);
    _tabCos.allocate(numangle);

    int *accum = _accum, *sort_buf = _sort_buf;
    double *tabSin = _tabSin, *tabCos = _tabCos;

    memset( accum, 0, sizeof(accum[0]) * (numangle+2) * (numrho+2) );

    for (int n = 0; n < numangle; ++n) {
        tabSin[n] = sin(pa[n]) * irho;
        tabCos[n] = cos(pa[n]) * irho;
    }

    // stage 1. fill accumulator
    for( i = 0; i < height; i++ )
        for( j = 0; j < width; j++ )
        {
            if( image[i * step + j] != 0 )
                for(int n = 0; n < numangle; n++ )
                {
                    int r = cvRound( (double)j * tabCos[n] + i * tabSin[n] );
                    r += (numrho - 1) / 2;
                    accum[(n+1) * (numrho+2) + r+1]++;
                }
        }


    // stage 2. find local maximums
    for(int r = 0; r < numrho; r++ )
        for(int n = 0; n < numangle; n++ )
        {
            int base = (n+1) * (numrho+2) + r+1;
            if( accum[base] > threshold &&
                accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
                accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2] )
                sort_buf[total++] = base;
        }

    // stage 3. sort the detected lines by accumulator value
    icvHoughSortDescent32s( sort_buf, total, accum );

    // stage 4. store the first min(total,linesMax) lines to the output buffer
    linesMax = MIN(linesMax, total);
    scale = 1./(numrho+2);
    for( i = 0; i < linesMax; i++ )
    {
        CvLinePolar line;
        int idx = sort_buf[i];
        int n = cvFloor(idx*scale) - 1;
        int r = idx - (n+1)*(numrho+2) - 1;
        line.rho = (r - (numrho - 1)*0.5f) * rho;
        line.angle = (float)pa[n];
        cvSeqPush( lines, &line );
    }
}










static void _hough_analyse( const CvMat* img, float rho, float theta,
                       int threshold, CvSeq *lines, int linesMax ) {
    cv::AutoBuffer<int> _accum, _sort_buf;
    cv::AutoBuffer<float> _tabSin, _tabCos;

    const uchar* image;
    int step, width, height;
    int numangle, numrho;
    int total = 0;
    int i, j;
    float irho = 1 / rho;
    //double scale;

    CV_Assert( CV_IS_MAT(img) && CV_MAT_TYPE(img->type) == CV_8UC1 );

    image = img->data.ptr;
    step = img->step;
    width = img->cols;
    height = img->rows;

    numangle = cvRound(CV_PI / theta);
    //numrho = cvRound(((width + height) * 2 + 1) / rho);
	numrho = cvRound((sqrt(width * width + height * height) * 2 + 1) / rho);

    _accum.allocate((numangle+2) * (numrho+2));
    _sort_buf.allocate(numangle * numrho);
    _tabSin.allocate(numangle);
    _tabCos.allocate(numangle);

    int *accum = _accum, *sort_buf = _sort_buf;
    float *tabSin = _tabSin, *tabCos = _tabCos;

    memset( accum, 0, sizeof(accum[0]) * (numangle+2) * (numrho+2) );

    float ang = 0;
    for(int n = 0; n < numangle; ang += theta, n++ )
    {
        tabSin[n] = (float)(sin((double)ang) * irho);
        tabCos[n] = (float)(cos((double)ang) * irho);
    }

    // stage 1. fill accumulator
    for( i = 0; i < height; i++ ) {
        for( j = 0; j < width; j++ )
        {
            if( image[i * step + j] != 0 )
                for(int n = 0; n < numangle; n++ )
                {
                    int r = cvRound( j * tabCos[n] + i * tabSin[n] );
                    r += (numrho - 1) / 2;
                    accum[(n+1) * (numrho+2) + r+1]++;
                }
        }
	}

	//int* aa = new int[numangle];
	//memset(aa, 0, sizeof(int) * numangle);
	//// stage 2. find local maximums
	//for(int r = 0; r < numrho; r++ ) {
	//	for(int n = 0; n < numangle; n++ )
	//	{
	//		int base = (n+1) * (numrho+2) + r+1;
	//		if( accum[base] > threshold &&
	//			accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
	//			accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2] ) {

	//				// sort_buf[total++] = base;
	//				aa[n] += accum[base];

	//		}

	//	}
	//}

	//// stage 4. store the first min(total,linesMax) lines to the output buffer
	//for (i = 0; i < numangle; i++) {
	//	cvSeqPush(lines, &aa[i]);
	//}
	//delete[] aa;


	int aaa;

	for (int n = 0; n < numangle; n++ ) {
		aaa = 0;
		for (int r = 0; r < numrho; r++ ) {

			int base = (n+1) * (numrho+2) + r+1;
			if( accum[base] > threshold &&
				accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
				accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2] ) {

					// sort_buf[total++] = base;
					aaa += accum[base];

			}
		}
		cvSeqPush(lines, &aaa);
	}


}




////////////////////////////////////////////////////////////////////////////////////////////////////

void icvHoughLinesProbabilistic( CvMat* image,
                            float rho, float theta, int threshold,
                            int lineLength, int lineGap,
                            CvSeq *lines, int linesMax )
{



    cv::Mat accum, mask;
    cv::vector<float> trigtab;
    cv::MemStorage storage(cvCreateMemStorage(0));

    CvSeq* seq;
    CvSeqWriter writer;
    int width, height;
    int numangle, numrho;
    float ang;
    int r, n, count;
    CvPoint pt;
    float irho = 1 / rho;
    CvRNG rng = cvRNG(-1);
    const float* ttab;
    uchar* mdata0;

    CV_Assert( CV_IS_MAT(image) && CV_MAT_TYPE(image->type) == CV_8UC1 );

    width = image->cols;
    height = image->rows;

    numangle = cvRound(CV_PI / theta);
    numrho = cvRound(((width + height) * 2 + 1) / rho);

    accum.create( numangle, numrho, CV_32SC1 );
    mask.create( height, width, CV_8UC1 );
    trigtab.resize(numangle*2);
    accum = cv::Scalar(0);

    for( ang = 0, n = 0; n < numangle; ang += theta, n++ )
    {
        trigtab[n*2] = (float)(cos(ang) * irho);
        trigtab[n*2+1] = (float)(sin(ang) * irho);
    }
    ttab = &trigtab[0];
    mdata0 = mask.data;

    cvStartWriteSeq( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage, &writer );

    // stage 1. collect non-zero image points
    for( pt.y = 0, count = 0; pt.y < height; pt.y++ )
    {
        const uchar* data = image->data.ptr + pt.y*image->step;
        uchar* mdata = mdata0 + pt.y*width;
        for( pt.x = 0; pt.x < width; pt.x++ )
        {
            if( data[pt.x] )
            {
                mdata[pt.x] = (uchar)1;
                CV_WRITE_SEQ_ELEM( pt, writer );
            }
            else
                mdata[pt.x] = 0;
        }
    }

    seq = cvEndWriteSeq( &writer );
    count = seq->total;

    // stage 2. process all the points in random order
    for( ; count > 0; count-- )
    {
        // choose random point out of the remaining ones
        int idx = cvRandInt(&rng) % count;
        int max_val = threshold-1, max_n = 0;
        CvPoint* point = (CvPoint*)cvGetSeqElem( seq, idx );
        CvPoint line_end[2] = {{0,0}, {0,0}};
        float a, b;
        int* adata = (int*)accum.data;
        int i, j, k, x0, y0, dx0, dy0, xflag;
        int good_line;
        const int shift = 16;

        i = point->y;
        j = point->x;

        // "remove" it by overriding it with the last element
        *point = *(CvPoint*)cvGetSeqElem( seq, count-1 );

        // check if it has been excluded already (i.e. belongs to some other line)
        if( !mdata0[i*width + j] )
            continue;

        // update accumulator, find the most probable line
        for( n = 0; n < numangle; n++, adata += numrho )
        {
            r = cvRound( j * ttab[n*2] + i * ttab[n*2+1] );
            r += (numrho - 1) / 2;
            int val = ++adata[r];
            if( max_val < val )
            {
                max_val = val;
                max_n = n;
            }
        }

        // if it is too "weak" candidate, continue with another point
        if( max_val < threshold )
            continue;

        // from the current point walk in each direction
        // along the found line and extract the line segment
        a = -ttab[max_n*2+1];
        b = ttab[max_n*2];
        x0 = j;
        y0 = i;
        if( fabs(a) > fabs(b) )
        {
            xflag = 1;
            dx0 = a > 0 ? 1 : -1;
            dy0 = cvRound( b*(1 << shift)/fabs(a) );
            y0 = (y0 << shift) + (1 << (shift-1));
        }
        else
        {
            xflag = 0;
            dy0 = b > 0 ? 1 : -1;
            dx0 = cvRound( a*(1 << shift)/fabs(b) );
            x0 = (x0 << shift) + (1 << (shift-1));
        }

        for( k = 0; k < 2; k++ )
        {
            int gap = 0, x = x0, y = y0, dx = dx0, dy = dy0;

            if( k > 0 )
                dx = -dx, dy = -dy;

            // walk along the line using fixed-point arithmetics,
            // stop at the image border or in case of too big gap
            for( ;; x += dx, y += dy )
            {
                uchar* mdata;
                int i1, j1;

                if( xflag )
                {
                    j1 = x;
                    i1 = y >> shift;
                }
                else
                {
                    j1 = x >> shift;
                    i1 = y;
                }

                if( j1 < 0 || j1 >= width || i1 < 0 || i1 >= height )
                    break;

                mdata = mdata0 + i1*width + j1;

                // for each non-zero point:
                //    update line end,
                //    clear the mask element
                //    reset the gap
                if( *mdata )
                {
                    gap = 0;
                    line_end[k].y = i1;
                    line_end[k].x = j1;
                }
                else if( ++gap > lineGap )
                    break;
            }
        }

        good_line = abs(line_end[1].x - line_end[0].x) >= lineLength ||
                    abs(line_end[1].y - line_end[0].y) >= lineLength;

        for( k = 0; k < 2; k++ )
        {
            int x = x0, y = y0, dx = dx0, dy = dy0;

            if( k > 0 )
                dx = -dx, dy = -dy;

            // walk along the line using fixed-point arithmetics,
            // stop at the image border or in case of too big gap
            for( ;; x += dx, y += dy )
            {
                uchar* mdata;
                int i1, j1;

                if( xflag )
                {
                    j1 = x;
                    i1 = y >> shift;
                }
                else
                {
                    j1 = x >> shift;
                    i1 = y;
                }

                mdata = mdata0 + i1*width + j1;

                // for each non-zero point:
                //    update line end,
                //    clear the mask element
                //    reset the gap
                if( *mdata )
                {
                    if( good_line )
                    {
                        adata = (int*)accum.data;
                        for( n = 0; n < numangle; n++, adata += numrho )
                        {
                            r = cvRound( j1 * ttab[n*2] + i1 * ttab[n*2+1] );
                            r += (numrho - 1) / 2;
                            adata[r]--;
                        }
                    }
                    *mdata = 0;
                }

                if( i1 == line_end[k].y && j1 == line_end[k].x )
                    break;
            }
        }

        if( good_line )
        {
            CvRect lr = { line_end[0].x, line_end[0].y, line_end[1].x, line_end[1].y };
            cvSeqPush( lines, &lr );
            if( lines->total >= linesMax )
                return;
        }
    }
}




/////////////////////////////////////////////////////////



CV_IMPL CvSeq*
cvHoughLines2( CvArr* src_image, void* lineStorage, int method,
               double rho, double theta, int threshold,
               double param1, double param2 )
{
    CvSeq* result = 0;

    CvMat stub, *img = (CvMat*)src_image;
    CvMat* mat = 0;
    CvSeq* lines = 0;
    CvSeq lines_header;
    CvSeqBlock lines_block;
    int lineType, elemSize;
    int linesMax = INT_MAX;
    int iparam1, iparam2;

    img = cvGetMat( img, &stub );

    if( !CV_IS_MASK_ARR(img))
        CV_Error( CV_StsBadArg, "The source image must be 8-bit, single-channel" );

    if( !lineStorage )
        CV_Error( CV_StsNullPtr, "NULL destination" );

    if( rho <= 0 || theta <= 0 || threshold <= 0 )
        CV_Error( CV_StsOutOfRange, "rho, theta and threshold must be positive" );

    if( method != CV_HOUGH_PROBABILISTIC )
    {
        lineType = CV_32FC2;
        elemSize = sizeof(float)*2;
    }
    else
    {
        lineType = CV_32SC4;
        elemSize = sizeof(int)*4;
    }

    if( CV_IS_STORAGE( lineStorage ))
    {
		// <---------------------
        lines = cvCreateSeq( lineType, sizeof(CvSeq), elemSize, (CvMemStorage*)lineStorage );
    }
    else if( CV_IS_MAT( lineStorage ))
    {
        mat = (CvMat*)lineStorage;

        if( !CV_IS_MAT_CONT( mat->type ) || (mat->rows != 1 && mat->cols != 1) )
            CV_Error( CV_StsBadArg,
            "The destination matrix should be continuous and have a single row or a single column" );

        if( CV_MAT_TYPE( mat->type ) != lineType )
            CV_Error( CV_StsBadArg,
            "The destination matrix data type is inappropriate, see the manual" );

        lines = cvMakeSeqHeaderForArray( lineType, sizeof(CvSeq), elemSize, mat->data.ptr,
                                         mat->rows + mat->cols - 1, &lines_header, &lines_block );
        linesMax = lines->total;
        cvClearSeq( lines );
    }
    else
        CV_Error( CV_StsBadArg, "Destination is not CvMemStorage* nor CvMat*" );

    iparam1 = cvRound(param1);
    iparam2 = cvRound(param2);

    switch( method )
    {
    case CV_HOUGH_STANDARD:
          icvHoughLinesStandard( img, (float)rho,
                (float)theta, threshold, lines, linesMax );
          break;
    case CV_HOUGH_MULTI_SCALE:
          //icvHoughLinesSDiv( img, (float)rho, (float)theta,
          //      threshold, iparam1, iparam2, lines, linesMax );
          break;
    case CV_HOUGH_PROBABILISTIC:
          icvHoughLinesProbabilistic( img, (float)rho, (float)theta,
                threshold, iparam1, iparam2, lines, linesMax );
          break;
    default:
        CV_Error( CV_StsBadArg, "Unrecognized method id" );
    }

    if( mat )
    {
        if( mat->cols > mat->rows )
            mat->cols = lines->total;
        else
            mat->rows = lines->total;
    }
    else
        result = lines;

    return result;
}



////////////////////////////////////////////

const int STORAGE_SIZE = 1 << 12;

static void seqToMat(const CvSeq* seq, cv::OutputArray _arr)
{
    if( seq && seq->total > 0 )
    {
        _arr.create(1, seq->total, seq->flags, -1, true);
        cv::Mat arr = _arr.getMat();
        cvCvtSeqToArray(seq, arr.data);
    }
    else
        _arr.release();
}

void hough_transform_p_opencv(cv::InputArray _image, cv::OutputArray _lines,
                      double rho, double theta, int threshold,
                      double minLineLength, double maxGap )
{
    cv::Ptr<CvMemStorage> storage = cvCreateMemStorage(STORAGE_SIZE);
    cv::Mat image = _image.getMat();
    CvMat c_image = image;
    CvSeq* seq = cvHoughLines2( &c_image, storage, CV_HOUGH_PROBABILISTIC,
                    rho, theta, threshold, minLineLength, maxGap );
    seqToMat(seq, _lines);
}


void hough_transform_opencv(cv::InputArray _image, cv::OutputArray _lines,
                     double rho, double theta, int threshold,
                     double srn, double stn )
{
    cv::Ptr<CvMemStorage> storage = cvCreateMemStorage(STORAGE_SIZE);
    cv::Mat image = _image.getMat();
    CvMat c_image = image;
    CvSeq* seq = cvHoughLines2( &c_image, storage, srn == 0 && stn == 0 ?
                    CV_HOUGH_STANDARD : CV_HOUGH_MULTI_SCALE,
                    rho, theta, threshold, srn, stn );
    seqToMat(seq, _lines);
}

void hough_standard(cv::InputArray _image, cv::OutputArray _lines, double rho, double theta, int threshold) {

	cv::Ptr<CvMemStorage> storage = cvCreateMemStorage(STORAGE_SIZE);
	cv::Mat image = _image.getMat();
	CvMat c_image = image;
	CvMat* img = &c_image;
	CvMat stub;
	img = cvGetMat(img, &stub);

	CvSeq* lines = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(float)*2, (CvMemStorage*)storage);

	icvHoughLinesStandard(img, (float)rho, (float)theta, threshold, lines, INT_MAX);

	CvSeq* seq = lines;//cvHoughLines2(&c_image, storage, CV_HOUGH_STANDARD, rho, theta, threshold, 0, 0);

	seqToMat(seq, _lines);
}

void hough_partition(cv::InputArray _image, cv::OutputArray _lines, double rho, const vector<double>& partition, int threshold) {

	cv::Ptr<CvMemStorage> storage = cvCreateMemStorage(STORAGE_SIZE);
	cv::Mat image = _image.getMat();
	CvMat c_image = image;
	CvMat* img = &c_image;
	CvMat stub;
	img = cvGetMat(img, &stub);

	CvSeq* lines = cvCreateSeq(CV_32FC2, sizeof(CvSeq), sizeof(float)*2, (CvMemStorage*)storage);

	_hough_partition(img, (float)rho, partition, threshold, lines, INT_MAX);

	CvSeq* seq = lines;//cvHoughLines2(&c_image, storage, CV_HOUGH_STANDARD, rho, theta, threshold, 0, 0);

	seqToMat(seq, _lines);
}

// collect all angle votes above the threshold in hough space and store them in vector votes
void hough_analyse(cv::InputArray _image, cv::OutputArray votes, double rho, double theta, int threshold) {

	cv::Ptr<CvMemStorage> storage = cvCreateMemStorage(STORAGE_SIZE);
	cv::Mat image = _image.getMat();
	CvMat c_image = image;
	CvMat* img = &c_image;
	CvMat stub;
	img = cvGetMat(img, &stub);

	CvSeq* lines = cvCreateSeq(CV_32SC1, sizeof(CvSeq), sizeof(int)*1, (CvMemStorage*)storage);

	_hough_analyse(img, (float)rho, (float)theta, threshold, lines, INT_MAX);

	CvSeq* seq = lines;//cvHoughLines2(&c_image, storage, CV_HOUGH_STANDARD, rho, theta, threshold, 0, 0);

	seqToMat(seq, votes);
}
// **********************************************************************************
//
// BSD License.
// This file is part of a Hough Transformation tutorial,
// see: http://www.keymolen.com/2013/05/hough-transformation-c-implementation.html
//
// Copyright (c) 2013, Bruno Keymolen, email: bruno.keymolen@gmail.com
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or other
// materials provided with the distribution.
// Neither the name of "Bruno Keymolen" nor the names of its contributors may be
// used to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// **********************************************************************************

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "hough.h"
#include "hough_circle.h"

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

std::string img_path;
int threshold = 0;

const char* CW_IMG_ORIGINAL 	= "Result";
const char* CW_IMG_EDGE		= "Canny Edge Detection";
const char* CW_ACCUMULATOR  	= "Accumulator";

void doTransformCircle(std::string file_path, int threshold);

struct SortCirclesDistance
{
    bool operator()( const std::pair< std::pair<int, int>, int>& a, const std::pair< std::pair<int, int>, int>& b )
    {
    	//int dist_a = sqrt( pow(a.first.first, 2) + pow(a.first.second, 2));
    	//int dist_b = sqrt( pow(b.first.first, 2) + pow(b.first.second, 2));

    	int d = sqrt( pow(abs(a.first.first - b.first.first), 2) + pow(abs(a.first.second - b.first.second), 2) );
    	if(d <= a.second + b.second)
    	{
    		//overlap
    		return a.second > b.second;
    	}
   		return false;

//    	return dist_a < dist_b;
    }

};


void usage(char * s)
{

	fprintf( stderr, "\n");
    	fprintf( stderr, "%s -s <source file> [-t <threshold>] - hough transform. build: %s-%s \n", s, __DATE__, __TIME__);
	fprintf( stderr, "   s: path image file\n");
	fprintf( stderr, "   t: hough threshold\n");
	fprintf( stderr, "\nexample:  ./hough -s ./img/russell-crowe-robin-hood-arrow.jpg -t 195\n");
	fprintf( stderr, "\n");
}

int main(int argc, char** argv) {

	int c;
	while ( ((c = getopt( argc, argv, "s:t:?" ) ) ) != -1 )
	{
	    switch (c)
	    {
	    case 's':
	    	img_path = optarg;
	    	break;
	    case 't':
	    	threshold = atoi(optarg);
	    	break;
	    case '?':
        default:
			usage(argv[0]);
			return -1;
	    }
	}

	if(img_path.empty())
	{
		usage(argv[0]);
		return -1;
	}

    cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_IMG_EDGE, 	 cv::WINDOW_AUTOSIZE);
    cv::namedWindow(CW_ACCUMULATOR,	 cv::WINDOW_AUTOSIZE);

    cvMoveWindow(CW_IMG_ORIGINAL, 10, 10);
    cvMoveWindow(CW_IMG_EDGE, 680, 10);
    cvMoveWindow(CW_ACCUMULATOR, 1350, 10);

    doTransformCircle(img_path, threshold);

return 0;
}


void doTransformCircle(std::string file_path, int threshold)
{
	cv::Mat img_edge;
	cv::Mat img_blur;

	cv::Mat img_ori = cv::imread( file_path, 1 );
	cv::blur( img_ori, img_blur, cv::Size(5,5) );
	cv::Canny(img_blur, img_edge, 175, 200, 3);

	int w = img_edge.cols;
	int h = img_edge.rows;


	//Transform
	keymolen::HoughCircle hough;

	std::vector< std::pair< std::pair<int, int>, int> > circles;

	for(int r=20;r<100/*h/2*/;r=r+5)
	{
	hough.Transform(img_edge.data, w, h, r);

	std::cout<< r << " / " << h/2;

	if(threshold == 0)
		threshold = 0.95 * (2.0 * (double)r * M_PI);

//		threshold = 200;

	//while(1)
	{

		//Search the accumulator
		hough.GetCircles(threshold, circles);

		//Draw the results

		int aw, ah, maxa;
		aw = ah = maxa = 0;
		const unsigned int* accu = hough.GetAccu(&aw, &ah);

		for(int p=0;p<(ah*aw);p++)
		{
			if((int)accu[p] > maxa)
				maxa = accu[p];
		}
		double contrast = 1.0;
		double coef = 255.0 / (double)maxa * contrast;

		cv::Mat img_accu(ah, aw, CV_8UC3);
		for(int p=0;p<(ah*aw);p++)
		{
			unsigned char c = (double)accu[p] * coef < 255.0 ? (double)accu[p] * coef : 255.0;
			img_accu.data[(p*3)+0] = 255;
			img_accu.data[(p*3)+1] = 255-c;
			img_accu.data[(p*3)+2] = 255-c;
		}


		cv::imshow(CW_IMG_ORIGINAL, img_ori);
		cv::imshow(CW_IMG_EDGE, img_edge);
		cv::imshow(CW_ACCUMULATOR, img_accu);

		cv::waitKey(1);

	}
	} //r



	//Filter the results
	{
	std::sort(circles.begin(), circles.end(), SortCirclesDistance());
	int a,b,r;
	a=b=r=0;
	std::vector< std::pair< std::pair<int, int>, int> > result;
	std::vector< std::pair< std::pair<int, int>, int> >::iterator it;
	for(it=circles.begin();it!=circles.end();it++)
	{
		int d = sqrt( pow(abs(it->first.first - a), 2) + pow(abs(it->first.second - b), 2) );
		if( d > it->second + r)
		{
			result.push_back(*it);
			//ok
			a = it->first.first;
			b = it->first.second;
			r = it->second;
		}
	}

	//Visualize all
	cv::Mat img_res = img_ori.clone();
	for(it=result.begin();it!=result.end();it++)
	{
		std::cout << it->first.first << ", " << it->first.second << std::endl;
		cv::circle(img_res, cv::Point(it->first.first, it->first.second), it->second, cv::Scalar( 0, 0, 255), 2, 8);
	}
	cv::imshow(CW_IMG_ORIGINAL, img_res);
	cv::imshow(CW_IMG_EDGE, img_edge);
	//cv::imshow(CW_ACCUMULATOR, img_accu);
	cv::waitKey(360000);
	}

}


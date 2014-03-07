/*
 * hough_circle.h
 *
 *  Created on: May 4, 2013
 *      Author: bruno
 */

#ifndef HOUGH_CIRCLE_H_
#define HOUGH_CIRCLE_H_

#include <vector>

namespace keymolen {

	class HoughCircle {
	public:
		HoughCircle();
		virtual ~HoughCircle();
	public:
		//img_data: 8 bit edge image ( >250 is edge)
		int Transform(unsigned char* img_data, int w, int h, int r);
		//threshold: number of pixels in a circle; result[(x, y), r]
		int GetCircles(int threshold, std::vector< std::pair< std::pair<int, int>, int> >& result );
	    const unsigned int* GetAccu(int *w, int *h);
	private:
		unsigned int* _accu;
		int _accu_w;
		int _accu_h;
		int _img_w;
		int _img_h;
		int _r;

	};

}

#endif /* HOUGH_CIRCLE_H_ */

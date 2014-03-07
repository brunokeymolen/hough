/*
 * hough_circle.cpp
 *
 *  Created on: May 4, 2013
 *      Author: bruno
 */

#include "hough_circle.h"

#include <cmath>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#ifndef DEG2RAD
	#define DEG2RAD 0.017453293f
#endif

namespace keymolen {

	HoughCircle::HoughCircle():_accu(0), _accu_w(0), _accu_h(0), _img_w(0), _img_h(0)
	{

	}

	HoughCircle::~HoughCircle() {
		if(_accu)
			free(_accu);
	}

	int HoughCircle::Transform(unsigned char* img_data, int w, int h, int r)
	{
		_r = r;

		_img_w = w;
		_img_h = h;

		//Create the accu
		_accu_h = h;
		_accu_w = w;

		if(_accu)
			free(_accu);
		_accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));

		for(int y=0;y<h;y++)
		{
			for(int x=0;x<w;x++)
			{
				if( img_data[ (y*w) + x] > 250 )
				{
					for(int t=1;t<=360;t++)
					{
						int a = ((double)x - ((double)_r * cos((double)t * DEG2RAD)));
						int b = ((double)y - ((double)_r * sin((double)t * DEG2RAD)));

						if( (b>=0 && b<_accu_h) && (a>=0 && a<_accu_w))
							_accu[(b * _accu_w) + a]++;
					}
				}
			}
		}

		return 0;
	}

	const unsigned int* HoughCircle::GetAccu(int *w, int *h)
	{
		*w = _accu_w;
		*h = _accu_h;

		return _accu;
	}

	int HoughCircle::GetCircles(int threshold, std::vector< std::pair< std::pair<int, int>, int> >& result )
	{
		int found = 0;

		if(_accu == 0)
			return found;

		for(int b=0;b<_accu_h;b++)
		{
			for(int a=0;a<_accu_w;a++)
			{
				if((int)_accu[(b*_accu_w) + a] >= threshold)
				{
					//Is this point a local maxima (9x9)
					int max = _accu[(b*_accu_w) + a];
					for(int ly=-4;ly<=4;ly++)
					{
						for(int lx=-4;lx<=4;lx++)
						{
							if( (ly+b>=0 && ly+b<_accu_h) && (lx+a>=0 && lx+a<_accu_w)  )
							{
								if( (int)_accu[( (b+ly)*_accu_w) + (a+lx)] > max )
								{
									max = _accu[( (b+ly)*_accu_w) + (a+lx)];
									ly = lx = 5;
								}
							}
						}
					}
					if(max > (int)_accu[(b*_accu_w) + a])
						continue;

					result.push_back(std::pair< std::pair<int, int>, int>(std::pair<int, int>(a,b), _r));
					found++;

				}
			}
		}

		std::cout << "result: " << found << " " << threshold << std::endl;

		return found;
	}

}

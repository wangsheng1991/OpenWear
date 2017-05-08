#include "SeetaFace.h"

std::vector<FaceDetail> CSeetaFace::DetectandAlign(const cv::Mat &img)
{
	cv::Mat img_gray;
	if (img.channels() != 1)
		cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	else
		img_gray = img;
	seeta::ImageData image_data;
	image_data.data = img_gray.data;
	image_data.width = img_gray.cols;
	image_data.height = img_gray.rows;
	image_data.num_channels = 1;
	std::vector<seeta::FaceInfo> faces = pfd->Detect(image_data);
	std::vector<FaceDetail> fds;
	for (int i = 0; i < faces.size(); i++)
	{
		FaceDetail fd;
		fd.fi = faces[i];
		pfa->PointDetectLandmarks(image_data, faces[i], fd.points);
		fds.push_back(fd);
	}
	return fds;
}

void CSeetaFace::drawFaces(cv::Mat &img, std::vector<FaceDetail> &fds)
{
	for (int i = 0; i < fds.size(); i++)
	{
		auto fi = fds[i].fi;
		auto points = fds[i].points;
		cv::rectangle(img, cv::Rect(fi.bbox.x, fi.bbox.y, fi.bbox.width, fi.bbox.height), CV_RGB(0, 0, 255));
#if SHOW_LANDMARK
		for (int j = 0; j < 5; j++)
		{
			cv::circle(img, cv::Point(points[j].x, points[j].y), 2, CV_RGB(255, 0, 0), CV_FILLED);
		}
#endif
	}
}
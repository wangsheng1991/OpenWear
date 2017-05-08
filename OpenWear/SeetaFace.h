#pragma once
#include "opencv2/opencv.hpp"
#include "face_alignment.h"
#include "face_detection.h"
#include "mrutil.h"
#define SHOW_LANDMARK 1
class FaceDetail
{
public:
	seeta::FaceInfo fi;
	seeta::FacialLandmark points[5];
};

class CSeetaFace
{
public:
	static CSeetaFace *getInstance()
	{
		static CSeetaFace instance;
		return &instance;
	}
	std::vector<FaceDetail> DetectandAlign(const cv::Mat &img);
	void drawFaces(cv::Mat &img, std::vector<FaceDetail> &fds);
private:
	CSeetaFace(){
		pfd = new seeta::FaceDetection("model/seeta_fd_frontal_v1.0.bin");
		pfd->SetMinFaceSize(200);
		pfd->SetScoreThresh(2.f);
		pfd->SetImagePyramidScaleFactor(0.8f);
		pfd->SetWindowStep(4, 4);
		pfa = new seeta::FaceAlignment("model/seeta_fa_v1.0.bin");
	}
	~CSeetaFace()
	{
		delete pfd;
		delete pfa;
	}
	seeta::FaceDetection *pfd;
	seeta::FaceAlignment *pfa;
	const int pts_num = 5;
};

#include "energydetector.h"
#include "anglesolver.h"

double EnergyDetector::getDistance(Point A,Point B)
{
    double dis;
    dis=pow((A.x-B.x),2)+pow((A.y-B.y),2);
    return sqrt(dis);
}
//标准化并计算hog
vector<float> EnergyDetector::stander(Mat im)
{

    if(im.empty()==1)
    {
        cout<<"filed open"<<endl;
    }
    resize(im,im,Size(48,48));

    vector<float> result;

    HOGDescriptor hog(cvSize(48,48),cvSize(16,16),cvSize(8,8),cvSize(8,8),9,1,-1,
                      HOGDescriptor::L2Hys,0.2,false,HOGDescriptor::DEFAULT_NLEVELS);           //初始化HOG描述符
    hog.compute(im,result);
    return result;
}
//将图片转换为svm所需格式
Mat EnergyDetector::get(Mat input)
{
    vector<float> vec=stander(input);
    if(vec.size()!=900) cout<<"wrong not 900"<<endl;
    Mat output(1,900,CV_32FC1);

    Mat_<float> p=output;
    int jj=0;
    for(vector<float>::iterator iter=vec.begin();iter!=vec.end();iter++,jj++)
    {
        p(0,jj)=*(iter);
    }
    return output;
}

EnergyDetector::EnergyDetector()
{
    svm = cv::ml::SVM::create();
    svm->load("/home/dji/energy/camera_param/SVM_mode.xml");
}

bool EnergyDetector::run(const cv::Mat &src_img, cv::Mat &dst_img, EnergyBox &target, int &self_color){

    Point centerP;
    Point centerR;
    Point centerP_pre;
    int angle = 0;
    int count_ture = 0;
    int flag = 0;
//    double V;
//    double ptz_pitch;
//    double theta = 0.0;
//    double pitch_theta = 0.0;

    //分割颜色通道
    vector<Mat> imgChannels;
    split(src_img,imgChannels);
    Mat midImage2;
    //获得目标颜色图像的二值图
    if(self_color == common_utils::SELF_BLUE)
    {
        midImage2=imgChannels.at(0)-imgChannels.at(2);
    }
    else if(sekf_color == common_utils::SELF_RED)
    {
        midImage2=imgChannels.at(2)-imgChannels.at(0);
    }
    else
    {
        dst_img = src_img;
        return false;
    }


    //二值化，背景为黑色，图案为白色
    //用于查找扇叶
    threshold(midImage2,midImage2,100,255,CV_THRESH_BINARY);
    int structElementSize=1;
    Mat element=getStructuringElement(MORPH_RECT,Size(2*structElementSize+3,2*structElementSize+3),Point(structElementSize,structElementSize));
    //膨胀
    dilate(midImage2,midImage2,element);
    //开运算，消除扇叶上可能存在的小洞
    structElementSize=1;
    element=getStructuringElement(MORPH_RECT,Size(2*structElementSize+1,2*structElementSize+1),Point(structElementSize,structElementSize));
    morphologyEx(midImage2,midImage2, MORPH_CLOSE, element);

    //查找轮廓
    vector<vector<Point>> contours2;
    vector<Vec4i> hierarchy2;
    findContours(midImage2,contours2,hierarchy2,RETR_CCOMP,CHAIN_APPROX_SIMPLE);

    RotatedRect rect_tmp2;
    bool findTarget=0;

    //遍历轮廓
    if(hierarchy2.size())
        for(int i=hierarchy2.size()-1;i>=0;i--)
        {
            rect_tmp2=minAreaRect(contours2[i]);
            Point2f P[4];
            rect_tmp2.points(P);

            Point2f srcRect[4];
            Point2f dstRect[4];
            Point2f rRect[4];

            double width;
            double height;

            //矫正提取的叶片的宽高
            width=getDistance(P[0],P[1]);
            height=getDistance(P[1],P[2]);
            if(width>height)
            {
                srcRect[0]=P[0];
                srcRect[1]=P[1];
                srcRect[2]=P[2];
                srcRect[3]=P[3];
            }
            else
            {
                swap(width,height);
                srcRect[0]=P[1];
                srcRect[1]=P[2];
                srcRect[2]=P[3];
                srcRect[3]=P[0];
            }
            //通过面积筛选
            double area=height*width;
            //cout<<"area "<<area<<endl;
            if(area>100 && area<180){
                rRect[0]=Point2f(0,0);
                rRect[1]=Point2f(width,0);
                rRect[2]=Point2f(width,height);
                rRect[3]=Point2f(0,height);
                // 应用透视变换，矫正成规则矩形
                Mat transform_r = getPerspectiveTransform(srcRect,rRect);
                Mat perspectMat_r;
                warpPerspective(midImage2,perspectMat_r,transform_r,midImage2.size());

                Mat testR;
                testR = perspectMat_r(Rect(0,0,width,height));

                RotatedRect rect_R=minAreaRect(contours2[i]);
                //cout<<"beggg"<<endl;
                centerR=rect_R.center;
                //circle(srcImg,centerR,1,Scalar(0,255,0),2);
                //imshow("testR",testR);
            }
            if(area>2000){
                dstRect[0]=Point2f(0,0);
                dstRect[1]=Point2f(width,0);
                dstRect[2]=Point2f(width,height);
                dstRect[3]=Point2f(0,height);
                // 应用透视变换，矫正成规则矩形
                Mat transform = getPerspectiveTransform(srcRect,dstRect);
                Mat perspectMat;
                warpPerspective(midImage2,perspectMat,transform,midImage2.size());

                // 提取扇叶图片
                Mat testim;
                testim = perspectMat(Rect(0,0,width,height));

                if(testim.empty())
                {
                    cout<<"filed open"<<endl;
                    //return -1;
                }


                //转化为svm所要求的格式
                Mat test=get(testim);

                if(svm->predict(test)>=0.9)
                {
                    findTarget=true;
                    //查找装甲板
                    if(hierarchy2[i][2]>=0)
                    {
                        RotatedRect rect_tmp=minAreaRect(contours2[hierarchy2[i][2]]);
                        Point2f Pnt[4];
                        rect_tmp.points(Pnt);
                        const float maxHWRatio=0.7153846;
                        const float maxArea=1000;
                        const float minArea=250;

                        float width=rect_tmp.size.width;
                        float height=rect_tmp.size.height;
                        if(height>width)
                            swap(height,width);
                        float area=width*height;

                        if(height/width>maxHWRatio||area>maxArea ||area<minArea){

                             continue;
                        }
                        centerP=rect_tmp.center;

                        //打击点
                        //circle(srcImg,centerP,1,Scalar(0,255,0),2);



                        for(int j=0;j<4;++j)
                        {
                            line(dst_img,Pnt[j],Pnt[(j+1)%4],Scalar(0,255,255),2);
                        }
                        if(centerR.x!=0&&centerR.y!=0){
                            /*
                            cout << "up " << (centerP.y<centerR.y) << endl;
                            cout << "right " << (centerP.y<centerR.y) << endl;
                            cout << "ture " << (count_ture>6 && count_ture<50) << endl;
                            cout << "value " << count_ture << endl;
                            cout << "value_xp " <<  centerP_pre.x << endl;
                            cout << "value_x " <<  centerP.x << endl;
                            */


                            if((count_ture>4 && count_ture<50) && (centerP.y<centerR.y && centerP_pre.x<centerP.x))
                            {
                                flag = 1;
                            }
                            else if((count_ture>40 && count_ture<50) && (centerP.y>centerR.y && centerP_pre.x>centerP.x))
                            {
                                flag = 1;
                            }
                            else if(count_ture>40 && count_ture<50)
                            {
                                flag = -1;
                            }

                            if(centerP_pre.x != centerP.x){
                                count_ture++;
                                centerP_pre = centerP;
                            }
                            
                            Rect r(centerP.x, centerP.y, width, height);
//                            target = EnergyBox(r);
//                            return true;
                            Point2f tl = r.tl();
                            Point2f br = r.br();
                            //cout << "width" << width << endl;
                            //cout << "height" << height << endl;
                            float min_x, min_y, max_x, max_y;
                            min_x = tl.x;
                            max_x = br.x;
                            min_y = tl.y;
                            max_y = br.y;
                            ARMOR = {
                                cv::Point2f(min_x, min_y),  //tl
                                cv::Point2f(max_x, min_y),	//tr
                                cv::Point2f(max_x, max_y),	//br
                                cv::Point2f(min_x, max_y)	//bl
                            };
                            AngleSolverParam pnp;
                            pnp.read_XMLFile();
                            AngleSolver PNP(pnp);
                            vector<double>result = PNP.trajectorySolve_energy(ARMOR ,4,float target_fan_angle,
                                                                              Point centerR);
                            /*
                            for(int i = 0; i<result.size();i++)
                            {
                                cout << "resu" << result[i] << endl;
                            }
                            */
                            vector<double>result1 = PNP.trajectorySolve(result);


//                            yawErr = result[0];
//                            pitchErr = result[1]+pitch_theta;
//                            char buff[9];
//                            buff[0]=0xAA;
//                            buff[1]=0x00;
//                            buff[2]=0x01;
//                            buff[3]=yawErr>>8;
//                            buff[4]=yawErr;
//                            buff[5]=pitchErr>>8;
//                            buff[6]=pitchErr;
//                            buff[7]=0x01;//0x00-
//                            buff[8]=buff[0]+buff[1]+buff[2]+buff[3]+buff[4]+buff[5]+buff[6]+buff[7];
//                            sel.writeData(buff, 9);
                        }
                    }

                }
            }
}
